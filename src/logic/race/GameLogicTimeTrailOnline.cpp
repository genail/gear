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

#include "GameLogicTimeTrailOnline.h"

#include "common/Game.h"
#include "logic/race/BasicGameClient.h"
#include "logic/race/Progress.h"
#include "network/client/Client.h"
#include "network/client/RankingClient.h"
#include "network/packets/GameState.h"
#include "network/packets/RankingAdvance.h"
#include "network/queries/RankingBrowseQuery.h"
#include "network/queries/RankingFindQuery.h"

namespace Race
{

class GameLogicTimeTrailOnlineImpl
{
	public:

		GameLogicTimeTrailOnline *m_parent;
		BasicGameClient m_basicClient;

		int m_lastLapNumber;

		RankingEntry m_firstPlaceRankingEntry;
		bool m_gotFirstPlaceRankingEntry;
		bool m_firstPlaceDirty;

		Net::RankingBrowseQuery m_queryRankingBrowse;

		PlacedRankingEntry m_playerRankingEntry;
		bool m_gotPlayerRankingEntry;
		bool m_playerPlaceDirty;

		Net::RankingFindQuery m_queryRankingFind;

		CL_SlotContainer m_slots;


		GameLogicTimeTrailOnlineImpl(GameLogicTimeTrailOnline *p_parent);
		void connectRankingSignals();

		~GameLogicTimeTrailOnlineImpl();

		void update(unsigned p_timeElapsedMs);
		void updateProgress();
		void updateRemoteRanking();
		void updateLocalRanking();

		void applyGameState(const Net::GameState &p_gameState);
		void requestLocalRankingUpdate();

		bool hasBestLapTime() const;
		int getBestLapTime() const;

};

GameLogicTimeTrailOnline::GameLogicTimeTrailOnline() :
		m_impl(new GameLogicTimeTrailOnlineImpl(this))
{
	// empty
}

GameLogicTimeTrailOnlineImpl::GameLogicTimeTrailOnlineImpl(GameLogicTimeTrailOnline *p_parent) :
		m_parent(p_parent),
		m_basicClient(p_parent),
		m_lastLapNumber(1),
		m_gotFirstPlaceRankingEntry(false),
		m_firstPlaceDirty(false),
		m_gotPlayerRankingEntry(false),
		m_playerPlaceDirty(false)
{
	requestLocalRankingUpdate();
}

GameLogicTimeTrailOnline::~GameLogicTimeTrailOnline()
{
	// empty
}

GameLogicTimeTrailOnlineImpl::~GameLogicTimeTrailOnlineImpl()
{
	// empty
}

void GameLogicTimeTrailOnline::update(unsigned p_timeElapsedMs)
{
	GameLogicTimeTrail::update(p_timeElapsedMs);
	m_impl->update(p_timeElapsedMs);
}

void GameLogicTimeTrailOnlineImpl::update(unsigned p_timeElapsedMs)
{
	m_basicClient.update();
	updateProgress();
	updateLocalRanking();
}

void GameLogicTimeTrailOnlineImpl::updateProgress()
{
	Game &game = Game::getInstance();
	Player &player = game.getPlayer();
	Car &car = player.getCar();

	const Progress &progress = m_parent->getProgressObject();
	const int currentLapNumber = progress.getLapNumber(car);

	if (currentLapNumber == m_lastLapNumber + 1) {
		updateRemoteRanking();
		m_lastLapNumber = currentLapNumber;
	}
}

void GameLogicTimeTrailOnlineImpl::updateRemoteRanking()
{
	Game &game = Game::getInstance();
	Player &player = game.getPlayer();
	Car &car = player.getCar();

	const Progress &progress = m_parent->getProgressObject();
	const int currentLapNumber = progress.getLapNumber(car);
	const int previousLapNumber = currentLapNumber - 1;

	const int lapTimeMs = progress.getLapTime(car, previousLapNumber);

	Net::Client &client = game.getNetworkConnection();
	Net::RankingClient &rankingClient = client.getRankingClient();

	rankingClient.sendTimeAdvance(lapTimeMs);

	requestLocalRankingUpdate();
}

void GameLogicTimeTrailOnlineImpl::updateLocalRanking()
{
	if (m_firstPlaceDirty && m_queryRankingBrowse.isDone()) {
		const int entryCount = m_queryRankingBrowse.getEntriesCount();
		if (entryCount == 1) {
			m_firstPlaceRankingEntry = m_queryRankingBrowse.getEntry(0);
			m_gotFirstPlaceRankingEntry = true;
		}

		m_firstPlaceDirty = false;
	}

	if (m_playerPlaceDirty && m_queryRankingFind.isDone()) {
		if (m_queryRankingFind.isFound()) {
			m_playerRankingEntry = m_queryRankingFind.getEntry();
			m_gotPlayerRankingEntry = true;
		}

		m_playerPlaceDirty = false;
	}
}

void GameLogicTimeTrailOnlineImpl::requestLocalRankingUpdate()
{
	m_queryRankingBrowse.setRange(1, 1);
	m_queryRankingBrowse.submit();

	Game &game = Game::getInstance();
	Player &player = game.getPlayer();

	m_queryRankingFind.setPlayerId(player.getId());
	m_queryRankingFind.submit();

	m_firstPlaceDirty = true;
	m_playerPlaceDirty = true;
}

void GameLogicTimeTrailOnline::applyGameState(const Net::GameState &p_gameState)
{
	m_impl->applyGameState(p_gameState);
}

void GameLogicTimeTrailOnlineImpl::applyGameState(const Net::GameState &p_gameState)
{
	m_basicClient.applyGameState(p_gameState);
}

bool GameLogicTimeTrailOnline::hasFirstPlaceRankingEntry() const
{
	return m_impl->m_gotFirstPlaceRankingEntry;
}

const RankingEntry &GameLogicTimeTrailOnline::getFirstPlaceRankingEntry() const
{
	G_ASSERT(hasFirstPlaceRankingEntry() && "entry not available");
	return m_impl->m_firstPlaceRankingEntry;
}

bool GameLogicTimeTrailOnline::hasBestLapTime() const
{
	return m_impl->hasBestLapTime();
}

bool GameLogicTimeTrailOnlineImpl::hasBestLapTime() const
{
	return m_gotPlayerRankingEntry;
}

int GameLogicTimeTrailOnline::getBestLapTime() const
{
	return m_impl->getBestLapTime();
}

int GameLogicTimeTrailOnlineImpl::getBestLapTime() const
{
	if (m_gotPlayerRankingEntry) {
		return m_playerRankingEntry.timeMs;
	} else {
		return 0;
	}
}

}
