/*
 * Copyright (c) 2009-2010, Piotr Korzuszek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "OnlineRaceLogic.h"

#include <assert.h>

#include "Car.h"
#include "common/Game.h"
#include "logic/race/Progress.h"
#include "network/packets/GameState.h"
#include "network/packets/CarState.h"

namespace Race {

class OnlineRaceLogicImpl
{
	public:
		typedef std::vector<CL_SharedPtr<RemotePlayer> > TPlayerList;


		OnlineRaceLogic *m_parent;

		/** Initialized state */
		bool m_initialized;

		/** Network client */
		Net::Client *m_client;

		/** Local player */
		Player &m_localPlayer;

		/** Network players */
		TPlayerList m_remotePlayers;

		/** Last iteration id when input was sent */
		int32_t m_lastIterInputSent;

		/** Do have input changed on the last iteration? */
		bool m_inputChanged;

		/** Slots container */
		CL_SlotContainer m_slots;


		// vote system

		bool m_voteRunning;

		CL_String m_voteMessage;

		int m_voteYesCount;

		int m_voteNoCount;

		unsigned m_voteTimeout;


		OnlineRaceLogicImpl(OnlineRaceLogic *p_parent);

		void connectLocalPlayerCarSlots();

		void connectNetworkClientSlots();

		~OnlineRaceLogicImpl();


		void initialize();

		void destroy();


		void display(const CL_String &p_text);


		bool needToSendCarState();

		bool iterationsPeriodReached();

		void sendCarState(const Car &p_car);


		// signal handlers

		void onConnected();

		void onDisconnected();

		void onGoodbye(GoodbyeReason p_reason, const CL_String &p_message);

		void onPlayerJoined(const CL_String &p_name);

		void onPlayerLeaved(const CL_String &p_name);

		void onGameState(const Net::GameState &p_gameState);

		void onCarState(const Net::CarState &p_carState);

		void onRaceStart(const CL_Pointf &p_carPosition, const CL_Angle &p_carRotation);

		void onInputChange(const Car &p_car);

		void onVoteStarted(VoteType p_voteType, const CL_String& p_subject, unsigned p_timeLimitSec);

		void onVoteEnded(VoteResult p_voteResult);

		void onVoteTick(VoteOption p_voteOption);
};

const unsigned RACE_START_DELAY = 3000;

// after how mush iterations car state must be resend
const int CAR_STATE_RESEND_DELTA = 60;

OnlineRaceLogic::OnlineRaceLogic() :
		m_impl(new OnlineRaceLogicImpl(this))
{
	// empty
}

OnlineRaceLogicImpl::OnlineRaceLogicImpl(OnlineRaceLogic *p_parent) :
		m_parent(p_parent),
		m_initialized(false),
		m_client(&Game::getInstance().getNetworkConnection()),
		m_localPlayer(Game::getInstance().getPlayer()),
		m_lastIterInputSent(-1),
		m_inputChanged(false),
		m_voteRunning(false)
{
	connectLocalPlayerCarSlots();
	connectNetworkClientSlots();
}

void OnlineRaceLogicImpl::connectLocalPlayerCarSlots()
{
	Car &car = m_localPlayer.getCar();
//	m_slots.connect(car.sig_inputChanged(), this, &OnlineRaceLogicImpl::onInputChange);
}

void OnlineRaceLogicImpl::connectNetworkClientSlots()
{
	m_slots.connect(m_client->sig_connected(),         this, &OnlineRaceLogicImpl::onConnected);
	m_slots.connect(m_client->sig_disconnected(),      this, &OnlineRaceLogicImpl::onDisconnected);
	m_slots.connect(m_client->sig_goodbyeReceived(),   this, &OnlineRaceLogicImpl::onGoodbye);
	m_slots.connect(m_client->sig_playerJoined(),      this, &OnlineRaceLogicImpl::onPlayerJoined);
	m_slots.connect(m_client->sig_playerLeaved(),      this, &OnlineRaceLogicImpl::onPlayerLeaved);
	m_slots.connect(m_client->sig_gameStateReceived(), this, &OnlineRaceLogicImpl::onGameState);
	m_slots.connect(m_client->sig_carStateReceived(),  this, &OnlineRaceLogicImpl::onCarState);
	m_slots.connect(m_client->sig_raceStartReceived(), this, &OnlineRaceLogicImpl::onRaceStart);
	m_slots.connect(m_client->sig_voteStarted(),       this, &OnlineRaceLogicImpl::onVoteStarted);
	m_slots.connect(m_client->sig_voteEnded(),         this, &OnlineRaceLogicImpl::onVoteEnded);
	m_slots.connect(m_client->sig_voteTickReceived(),  this, &OnlineRaceLogicImpl::onVoteTick);
}

OnlineRaceLogic::~OnlineRaceLogic()
{
	// empty
}

OnlineRaceLogicImpl::~OnlineRaceLogicImpl()
{
	destroy();
}

void OnlineRaceLogic::initialize()
{
	m_impl->initialize();
}

void OnlineRaceLogic::destroy()
{
	m_impl->destroy();
}

void OnlineRaceLogicImpl::initialize()
{
	if (!m_initialized) {
		m_initialized = true;
	}
}

void OnlineRaceLogicImpl::destroy()
{
	if (m_initialized) {
		// remove cars from level
		const int playerCount = m_parent->getPlayerCount();

		for (int i = 0; i < playerCount; ++i) {
			Player &player = m_parent->getPlayer(i);
			m_parent->getLevel().removeCar(&player.getCar());
		}

		m_parent->getProgress().destroy();
		m_parent->getLevel().destroy();
	}
}

void removeCarsFromLevel();
void destroyLevel();

void OnlineRaceLogic::update(unsigned p_timeElapsed)
{
	G_ASSERT(m_impl->m_initialized);

	RaceLogic::update(p_timeElapsed);

	// make sure that car is not locked when race is started
	Race::Car &car = m_impl->m_localPlayer.getCar();

	if (getLevel().hasCar(&car)) {
		// make sure that car is not locked when race is started
		if (getRaceState() == S_RUNNING && car.isLocked()) {
			car.setLocked(false);

			display(_("*** START! ***"));
			getProgress().resetClock();
		}

		// check is car state must be resent
		if (m_impl->needToSendCarState()) {
			m_impl->sendCarState(car);
			m_impl->m_inputChanged = false;
		}
	}
}

bool OnlineRaceLogicImpl::needToSendCarState()
{
	return iterationsPeriodReached()
			|| m_parent->localCarCollisionInThisIteration()
			|| m_inputChanged;
}

bool OnlineRaceLogicImpl::iterationsPeriodReached()
{
	const Race::Car &car = m_localPlayer.getCar();
	const int iterDelta = abs(car.getIterationId() - m_lastIterInputSent);
	return iterDelta >= CAR_STATE_RESEND_DELTA;
}

void OnlineRaceLogicImpl::onConnected()
{
}

void OnlineRaceLogicImpl::onDisconnected()
{
	display(_("Disconnected from host"));
}

void OnlineRaceLogicImpl::onGoodbye(GoodbyeReason p_reason, const CL_String &p_message)
{
	display(cl_format(_("Disconnected from server. Reason: %1"), p_message));
}

void OnlineRaceLogicImpl::onPlayerJoined(const CL_String &p_name)
{
	// check player existence

	if (!m_parent->hasPlayer(p_name)) {
		// create new player

		m_remotePlayers.push_back(
				CL_SharedPtr<RemotePlayer>(new RemotePlayer(p_name))
		);

		RemotePlayer &player = *m_remotePlayers.back();
		m_parent->addPlayer(&player);

		// add his car to the level
		m_parent->getLevel().addCar(&player.getCar());

		display(cl_format(_("Player %1 joined"), p_name));
	} else {
		cl_log_event(LOG_ERROR, "Player named '%1' already in list", p_name);
	}
}

void OnlineRaceLogicImpl::onPlayerLeaved(const CL_String &p_name)
{
	// get the player
	Player &player = m_parent->getPlayer(p_name);

	// remove from level
	m_parent->getLevel().removeCar(&player.getCar());

	// remove from game
	m_parent->removePlayer(player);

	TPlayerList::iterator itor;
	bool found = false;

	for (itor = m_remotePlayers.begin(); itor != m_remotePlayers.end(); ++itor) {

		if (reinterpret_cast<unsigned> (itor->get()) == reinterpret_cast<unsigned> (&player)) {
			m_remotePlayers.erase(itor);
			found = true;
			break;
		}
	}

	G_ASSERT(found);
	display(cl_format("Player %1 leaved", p_name));
}

void OnlineRaceLogicImpl::onGameState(const Net::GameState &p_gameState)
{
	// load level
	const CL_String &levelName = p_gameState.getLevel();
	m_parent->getLevel().initialize();
	m_parent->getLevel().load(levelName);

	// initialize progress object
	m_parent->getProgress().initialize();
	m_parent->getProgress().resetClock();

	// add rest of players
	const unsigned playerCount = p_gameState.getPlayerCount();

	Player *player;
	Car *car;

	for (unsigned i = 0; i < playerCount; ++i) {
		const CL_String &playerName = p_gameState.getPlayerName(i);

		if (playerName == m_localPlayer.getName()) {
			// this is local player, so it exists now
			player = &m_localPlayer;
		} else {
			// this is remote player
			m_remotePlayers.push_back(
					CL_SharedPtr<RemotePlayer>(new RemotePlayer(playerName))
			);

			player = m_remotePlayers.back();
		}

		// put player to player list
		m_parent->addPlayer(player);

		// prepare car and put it to level
		car = &player->getCar();
		car->resetIterationCounter();
		car->deserialize(p_gameState.getCarState(i).getSerializedData());

		m_parent->getLevel().addCar(&player->getCar());
	}


}

void OnlineRaceLogicImpl::onCarState(const Net::CarState &p_carState)
{
	const CL_String &playerName = p_carState.getName();

	if (m_parent->hasPlayer(playerName)) {
		const CL_NetGameEvent serialData = p_carState.getSerializedData();
		m_parent->getPlayer(playerName).getCar().deserialize(serialData);
	} else {
		cl_log_event(LOG_ERROR, "Player %1 do not exists", playerName);
	}
}

void OnlineRaceLogicImpl::onRaceStart(
const CL_Pointf &p_carPosition,
		const CL_Angle &p_carRotation
)
{
	cl_log_event(LOG_RACE, "race is starting");

	Car &car = Game::getInstance().getPlayer().getCar();

	car.reset();

	car.setPosition(p_carPosition);
	car.setAngle(p_carRotation);

	car.setLocked(true);

	// reset progress data
	m_parent->getProgress().reset(car);

	// send current state
	CL_NetGameEvent serialData("");
	car.serialize(&serialData);

	Net::CarState carState;
	carState.setSerializedData(serialData);
	carState.setIterationId(car.getIterationId());

	m_client->sendCarState(carState);

	// FIXME: where to store lap count?
	m_parent->startRace(3, CL_System::get_time() + RACE_START_DELAY);
}

void OnlineRaceLogicImpl::onInputChange(const Car &p_car)
{
	m_inputChanged = true;
}

void OnlineRaceLogicImpl::sendCarState(const Car &p_car)
{
	// do not send if already send
	if (p_car.getIterationId() == m_lastIterInputSent) {
		return;
	}

	// ignore sending when car is clocked
	if (p_car.isLocked()) {
		return;
	}

	CL_NetGameEvent serialData("");
	p_car.serialize(&serialData);

	Net::CarState carState;
	carState.setSerializedData(serialData);
	carState.setIterationId(p_car.getIterationId());

	if (m_parent->localCarCollisionInThisIteration()) {
		carState.setAfterCollision(true);
	}

	m_client->sendCarState(carState);

	m_lastIterInputSent = p_car.getIterationId();
}

void OnlineRaceLogic::callAVote(VoteType p_type, const CL_String &p_subject)
{
	m_impl->m_client->callAVote(p_type, p_subject);
}

const CL_String &OnlineRaceLogic::getVoteMessage() const
{
	return m_impl->m_voteMessage;
}

bool OnlineRaceLogic::isVoteRunning() const
{
	return m_impl->m_voteRunning;
}

int OnlineRaceLogic::getVoteNoCount() const
{
	return m_impl->m_voteNoCount;
}

int OnlineRaceLogic::getVoteYesCount() const
{
	return m_impl->m_voteYesCount;
}

unsigned OnlineRaceLogic::getVoteTimeout() const
{
	return m_impl->m_voteTimeout;
}

void OnlineRaceLogicImpl::onVoteStarted(
		VoteType p_voteType, const CL_String& p_subject, unsigned p_timeLimitSec
)
{
	switch (p_voteType) {
		case VOTE_RESTART_RACE:
			m_voteMessage = _("Restart Level?");
			break;

		default:
			assert(0 && "unknown VoteType");
	}

	m_voteRunning = true;
	m_voteYesCount = 0;
	m_voteNoCount = 0;
	m_voteTimeout = CL_System::get_time() + (p_timeLimitSec * 1000);

	display(_("New vote called"));

}

void OnlineRaceLogicImpl::onVoteEnded(VoteResult p_voteResult)
{
	switch (p_voteResult) {
		case VOTE_PASSED:
			display("Vote passed");
			break;
		case VOTE_FAILED:
			display("Vote failed");
			break;
		default:
			assert(0 && "unknown VoteResult");
	}

	m_voteRunning = false;
}

void OnlineRaceLogic::voteNo()
{
	m_impl->m_client->voteNo();
}

void OnlineRaceLogic::voteYes()
{
	m_impl->m_client->voteYes();
}

void OnlineRaceLogicImpl::onVoteTick(VoteOption p_voteOption)
{
	switch (p_voteOption) {
		case VOTE_YES:
			++m_voteYesCount;
			break;
		case VOTE_NO:
			++m_voteNoCount;
			break;
		default:
			assert(0 && "unknown VoteOption");
	}
}

void OnlineRaceLogicImpl::display(const CL_String &p_text)
{
	m_parent->display(p_text);
}

Net::Client &OnlineRaceLogic::getNetworkConnection()
{
	return *m_impl->m_client;
}

} // namespace
