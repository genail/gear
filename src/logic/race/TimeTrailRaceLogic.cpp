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

#include "TimeTrailRaceLogic.h"

#include "common.h"
#include "common/Game.h"
#include "common/Player.h"
#include "logic/race/Car.h"
#include "logic/race/Progress.h"
#include "network/client/Client.h"
#include "network/client/RankingClient.h"

namespace Race
{

class TimeTrailRaceLogicImpl
{
	public:

		TimeTrailRaceLogic *m_parent;

		int m_lastLapNum;

		int m_bestServerTimeMs;

		int m_bestPlayerTimeMs;

		bool m_needToUpdateTimesFromRanking;

		CL_SlotContainer m_slots;


		TimeTrailRaceLogicImpl(TimeTrailRaceLogic *p_parent);

		void connectRankingSlots();


		void update(unsigned p_timeElapsedMs);

		void updateLocalTimesIfNeeded();

		void updateRemoteRankingTimesIfNeeded();


		void onRankingEntriesReceived(const std::vector<PlacedRankingEntry> &p_entries);

};

TimeTrailRaceLogic::TimeTrailRaceLogic() :
		m_impl(new TimeTrailRaceLogicImpl(this))
{
	// empty
}

TimeTrailRaceLogicImpl::TimeTrailRaceLogicImpl(TimeTrailRaceLogic *p_parent) :
		m_parent(p_parent),
		m_lastLapNum(0),
		m_bestServerTimeMs(-1),
		m_bestPlayerTimeMs(-1),
		m_needToUpdateTimesFromRanking(true)
{
	connectRankingSlots();
}

TimeTrailRaceLogic::~TimeTrailRaceLogic()
{
	// empty
}

void TimeTrailRaceLogicImpl::connectRankingSlots()
{
	Net::Client &networkClient = Game::getInstance().getNetworkConnection();
	Net::RankingClient &rankingClient = networkClient.getRankingClient();

	m_slots.connect(
			rankingClient.sig_entriesReceived(),
			this, &TimeTrailRaceLogicImpl::onRankingEntriesReceived);
}

void TimeTrailRaceLogic::update(unsigned p_timeElapsedMs)
{
	OnlineRaceLogic::update(p_timeElapsedMs);

	m_impl->update(p_timeElapsedMs);
}

void TimeTrailRaceLogicImpl::update(unsigned p_timeElapsedMs)
{
	updateLocalTimesIfNeeded();
	updateRemoteRankingTimesIfNeeded();
}

void TimeTrailRaceLogicImpl::updateLocalTimesIfNeeded()
{
	Net::Client &networkClient = Game::getInstance().getNetworkConnection();
	Net::RankingClient &rankingClient = networkClient.getRankingClient();

	if (m_needToUpdateTimesFromRanking) {
		rankingClient.requestEntry(1);
		m_needToUpdateTimesFromRanking = false;
	}
}

void TimeTrailRaceLogicImpl::updateRemoteRankingTimesIfNeeded()
{
	const Race::Progress &progress = m_parent->getProgress();
	const Player &localPlayer = Game::getInstance().getPlayer();
	const Race::Car &localCar = localPlayer.getCar();
	const int currentLapNum = progress.getLapNumber(localCar);

	if (m_lastLapNum > 0 && currentLapNum != m_lastLapNum) {
		int lapTimeMs = progress.getLapTime(localCar, m_lastLapNum);
		Net::Client &networkClient = Game::getInstance().getNetworkConnection();
		Net::RankingClient &rankingClient = networkClient.getRankingClient();

		rankingClient.sendTimeAdvance(lapTimeMs);
	}
}

void TimeTrailRaceLogicImpl::onRankingEntriesReceived(
		const std::vector<PlacedRankingEntry> &p_entries
) {
	foreach(const PlacedRankingEntry &entry, p_entries) {
		if (entry.place == 1) {
			m_bestServerTimeMs = entry.timeMs;
		}
	}
}

}
