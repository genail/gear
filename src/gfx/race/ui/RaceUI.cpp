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

#include "RaceUI.h"

#include <map>

#include "common/Game.h"
#include "gfx/Stage.h"
#include "gfx/Viewport.h"
#include "gfx/race/ui/Label.h"
#include "gfx/race/ui/PlayerList.h"
#include "gfx/race/ui/ScoreTable.h"
#include "gfx/race/ui/SpeedMeter.h"
#include "gfx/scenes/RaceScene.h"
#include "logic/VoteSystem.h"
#include "logic/race/Car.h"
#include "logic/race/GameLogic.h"
#include "logic/race/MessageBoard.h"
#include "logic/race/Progress.h"
#include "math/Time.h"

namespace Gfx {

struct Comparator {
	bool operator() (unsigned s1, unsigned s2) const
	{
		return s1 > s2; // reverse order
	}
};

class RaceUIImpl
{
	public:

		SpeedMeter m_speedMeter;
		PlayerList m_playerList;
		ScoreTable m_scoreTable;

		Label m_globMsgLabel;
		Label m_voteLabel;
		Label m_messageBoardLabel;
		Label m_lapLabel;
		Label m_lapTimesLabel;
		Label m_lapTimesLabelBold;
		Label m_carLabel;
		Label m_countdownLabel;

		// Race logic pointer
		const Race::GameLogic *m_logic;
		Race::RaceGameState m_lastState;

		// Viewport pointer
		const Gfx::Viewport *m_viewport;


		// slots container
		CL_SlotContainer m_slots;


		RaceUIImpl(
				const Race::GameLogic *p_logic,
				const Gfx::Viewport *p_viewport
		) :
			m_playerList(p_logic),
			m_scoreTable(p_logic),
			m_globMsgLabel(CL_Pointf(Stage::getWidth() / 2, Stage::getHeight() / 3), "", Label::F_BOLD, 36),
			m_voteLabel(CL_Pointf(100, 20), "", Label::F_BOLD, 20),
			m_messageBoardLabel(CL_Pointf(), "", Label::F_REGULAR, 14),
			m_lapLabel(CL_Pointf(Stage::getWidth() - 20, 5), "", Label::F_BOLD, 25),
			m_lapTimesLabel(CL_Pointf(), "", Label::F_REGULAR, 14),
			m_lapTimesLabelBold(CL_Pointf(), "", Label::F_BOLD, 14),
			m_carLabel(CL_Pointf(), "", Label::F_REGULAR, 14),
			m_countdownLabel(CL_Pointf(0.0f, 0.0f), "", Label::F_BOLD, 75),
			m_logic(p_logic),
			m_lastState(m_logic->getRaceGameState()),
			m_viewport(p_viewport)
		{
			m_globMsgLabel.setAttachPoint(Label::AP_CENTER | Label::AP_BOTTOM);
			m_playerList.setPosition(CL_Pointf(Stage::getWidth() - 200, 100));
			m_lapLabel.setAttachPoint(Label::AP_RIGHT | Label::AP_TOP);
			m_carLabel.setAttachPoint(Label::AP_CENTER | Label::AP_TOP);
			m_countdownLabel.setAttachPoint(Label::AP_CENTER);
		}


		void watchRaceGameStateForChanges();

		void handleRaceGameStateChanges(Race::RaceGameState p_from, Race::RaceGameState p_to);


		// draw routines

		void drawMeters(CL_GraphicContext &p_gc);

		void drawVote(CL_GraphicContext &p_gc);

		void drawMessageBoard(CL_GraphicContext &p_gc);

		void drawLapLabel(CL_GraphicContext &p_gc);

		void drawCarLabels(CL_GraphicContext &p_gc);

		void drawPlayerList(CL_GraphicContext &p_gc);

		void drawLapTimes(CL_GraphicContext &p_gc);

		void drawCountdown(CL_GraphicContext &p_gc);

		void drawCountdownBg(CL_GraphicContext &p_gc);

		void drawGlobalMessage(CL_GraphicContext &p_gc);

		void drawScoreTable(CL_GraphicContext &p_gc);

};

RaceUI::RaceUI(const Race::GameLogic *p_logic, const Gfx::Viewport *p_viewport) :
	m_impl(new RaceUIImpl(p_logic, p_viewport))
{
	// empty
}

RaceUI::~RaceUI()
{
	// empty
}

void RaceUI::update(unsigned p_timeElapsed)
{
	m_impl->watchRaceGameStateForChanges();
	m_impl->m_scoreTable.update(p_timeElapsed);
}

void RaceUIImpl::watchRaceGameStateForChanges()
{
	const Race::RaceGameState currentState = m_logic->getRaceGameState();
	if (currentState != m_lastState) {
		handleRaceGameStateChanges(m_lastState, currentState);

		m_lastState = currentState;
	}
}

void RaceUI::draw(CL_GraphicContext &p_gc)
{
	m_impl->drawMeters(p_gc);
	m_impl->drawVote(p_gc);
	m_impl->drawMessageBoard(p_gc);
	m_impl->drawLapLabel(p_gc);
	m_impl->drawLapTimes(p_gc);
	m_impl->drawCarLabels(p_gc);
	m_impl->drawPlayerList(p_gc);
//	m_impl->drawCountdown(p_gc); FIXME: reenable this
	m_impl->drawGlobalMessage(p_gc);
	m_impl->drawScoreTable(p_gc);
}

void RaceUIImpl::drawMeters(CL_GraphicContext &p_gc)
{
	// draw speed control
	m_speedMeter.draw(p_gc);
}

void RaceUIImpl::drawVote(CL_GraphicContext &p_gc)
{
	const VoteSystem &voteSystem = m_logic->getVoteSystem();

	if (voteSystem.isRunning()) {

		// calculate time left in seconds
		const unsigned now = CL_System::get_time();
		const unsigned deadline = voteSystem.getFinishTime();
		const unsigned timeLeftSec = deadline >= now ? (deadline - now) / 1000 : 0;

		const CL_String &subject = voteSystem.getVoteSubject();
		const int yesCount = voteSystem.getYesCount();
		const int noCount = voteSystem.getNoCount();

		m_voteLabel.setText(
				cl_format(
						_("VOTE (%1): %2 yes: %3 no: %4"),
						timeLeftSec, subject, yesCount, noCount
				)
		);

		m_voteLabel.setPosition(CL_Pointf(10, m_voteLabel.size(p_gc).height));
		m_voteLabel.draw(p_gc);

		m_voteLabel.setPosition(CL_Pointf(10, m_voteLabel.size(p_gc).height * 2));
		m_voteLabel.setText(_("To vote press F1 (YES) or F2 (NO)"));
		m_voteLabel.draw(p_gc);
	}
}

void RaceUIImpl::drawMessageBoard(CL_GraphicContext &p_gc)
{
	static const float START_POSITION_X = 0.3f;
	static const float START_POSITION_Y = 0.95f;

	static const int CHANGE_Y = -20;
	static const int ITEM_LIMIT = 10;

	static const unsigned VISIBLE_TIME_MS = 20000;
	static const unsigned FADE_TIME_MS = 5000;

	const Race::MessageBoard &board = m_logic->getMessageBoard();
	std::vector<int> messages = board.getMessageIdsYoungerThat(VISIBLE_TIME_MS, ITEM_LIMIT);

	// sort results in multimap by age
	typedef std::multimap<unsigned, CL_String, Comparator> TMessageMap;
	typedef std::pair<unsigned, CL_String> TMessagePair;

	TMessageMap messageMap;

	unsigned age;

	foreach(int id, messages) {
		age = board.getMessageCreationTime(id);
		const CL_String &msg = board.getMessageString(id);

		messageMap.insert(TMessagePair(age, msg));
	}

	// display in sorted order
	CL_Pointf position(Stage::getWidth() * START_POSITION_X, Stage::getHeight() * START_POSITION_Y);
	const unsigned now = CL_System::get_time();

	TMessagePair pair;
	foreach (pair, messageMap) {
		m_messageBoardLabel.setPosition(position);
		m_messageBoardLabel.setText(pair.second);

		// calculate color
		age = now - pair.first;

		if (age < VISIBLE_TIME_MS - FADE_TIME_MS) {
			m_messageBoardLabel.setColor(CL_Colorf::white);
		} else {
			const float alpha = 1.0f - ((age - (VISIBLE_TIME_MS - FADE_TIME_MS)) / (float) FADE_TIME_MS);
			m_messageBoardLabel.setColor(CL_Colorf(1.0f, 1.0f, 1.0f, alpha));
		}

		m_messageBoardLabel.draw(p_gc);
		position.y += CHANGE_Y;
	}

}

void RaceUIImpl::drawLapLabel(CL_GraphicContext &p_gc)
{
	Game &game = Game::getInstance();
	Player &player = game.getPlayer();
	Race::Car &car = player.getCar();

	const Race::Progress &progress = m_logic->getProgressObject();
	const int currentLap = progress.getLapNumber(car);
	const int lapsTotal = m_logic->getLapCount();

	CL_String text;

	if (lapsTotal == 0) {
		text = cl_format(_("Lap %1"), currentLap);
	} else {
		text = cl_format(_("Lap %1 / %2"), currentLap, lapsTotal);
	}

	m_lapLabel.setText(text);
	m_lapLabel.draw(p_gc);
}

void RaceUIImpl::drawLapTimes(CL_GraphicContext &p_gc)
{
	static const int TOP_MARGIN = 70;
	static const int RIGHT_MARGIN = 100;
	static const int Y_DELTA = 16;

	const Race::Car &car = Game::getInstance().getPlayer().getCar();
	const Race::Progress &pr = m_logic->getProgressObject();
	const int lap = pr.getLapNumber(car);

	if (lap == 0) {
		return;
	}

	// find best lap
	unsigned best = 0, nbest = 0;

	for (int i = 1; i < lap; ++i) {
		if (i == 1) {
			best = pr.getLapTime(car, i);
		} else {
			nbest = pr.getLapTime(car, i);
			if (nbest < best) {
				best = nbest;
			}
		}
	}

	// get current lap time
	unsigned curr;

	// display 0 time until race is started
	if (m_logic->getRaceGameState() == Race::GS_RUNNING) {
		curr = pr.getLapTime(car, lap);
	} else {
		curr = 0;
	}

	// display times
	const int x = Stage::getWidth() - RIGHT_MARGIN;
	int y = TOP_MARGIN;

	Math::Time tbest(best);
	Math::Time tcurr(curr);

	CL_Pointf pos(x, y);

	// Lap Time label
	m_lapTimesLabelBold.setPosition(pos);
	m_lapTimesLabelBold.setText("Lap Time");
	m_lapTimesLabelBold.draw(p_gc);

	// Lap Time
	pos.y += Y_DELTA;
	m_lapTimesLabel.setPosition(pos);
	m_lapTimesLabel.setText(tcurr.raceFormat());
	m_lapTimesLabel.draw(p_gc);

	// Best Time label
	pos.y += Y_DELTA * 2;
	m_lapTimesLabelBold.setPosition(pos);
	m_lapTimesLabelBold.setText("Best Time");
	m_lapTimesLabelBold.draw(p_gc);

	// Best Time

	pos.y += Y_DELTA;
	m_lapTimesLabel.setPosition(pos);

	if (best != 0) {
		m_lapTimesLabel.setText(tbest.raceFormat());
	} else {
		m_lapTimesLabel.setText("--:--:---");
	}

	m_lapTimesLabel.draw(p_gc);


}

void RaceUIImpl::drawCarLabels(CL_GraphicContext &p_gc)
{
	const Race::Level &level = m_logic->getLevel();
	const int carCount = level.getCarCount();

	CL_Pointf pos;

	for (int i = 0; i < carCount; ++i) {

		const Race::Car &car = level.getCar(i);
		pos = m_viewport->toScreen(car.getPosition());

		pos.y += 20;

		m_carLabel.setPosition(pos);
		m_carLabel.setText(car.getOwnerPlayer().getName());

		m_carLabel.draw(p_gc);
	}
}

void RaceUIImpl::drawPlayerList(CL_GraphicContext &p_gc)
{
	m_playerList.draw(p_gc);
}

void RaceUIImpl::drawCountdown(CL_GraphicContext &p_gc)
{
//	const unsigned startTime = m_logic->getRaceStartTime();
	const unsigned startTime = 0;
	G_ASSERT(0 && "disabled by now");

	if (startTime == 0) { // not started nor pending
		return;
	}

	const unsigned now = CL_System::get_time();


	if (now < startTime) {
		// race not started yet
		static const unsigned COUNTDOWN_START_TIME_SEC = 3;

		const int delta = static_cast<int> (startTime - now);
		const unsigned sec = (delta / 1000) + 1;

		if (sec <= COUNTDOWN_START_TIME_SEC) {
			// draw background
			drawCountdownBg(p_gc);

			// calculate fraction
			const float f = (delta - (sec - 1) * 1000) / 1000.0f;
			const float f2 = 1.0f - f;

			// draw foreground
			p_gc.push_translate(Stage::getWidth() / 2, Stage::getHeight() / 2);
//			p_gc.mult_scale(3.0f - f * 2, 3.0f - f * 2, 1.0f);
			p_gc.mult_scale(1.0f + f2 * 10, 1.0f + f2 * 10, 1.0f);

			m_countdownLabel.setText(CL_StringHelp::int_to_local8(sec));
			m_countdownLabel.setColor(CL_Colorf(1.0f, 1.0f, 1.0f, f));
			m_countdownLabel.draw(p_gc);

			p_gc.pop_modelview();
		}

	} else {
//		cl_log_event(LOG_DEBUG, "aa");
		// race already started
		static const int START_DISPLAY_TIME_MS = 2000;

		const int delta = static_cast<int> (now - startTime);
		if (delta <= START_DISPLAY_TIME_MS) {
			// draw background
			drawCountdownBg(p_gc);

			// draw foreground
			p_gc.push_translate(Stage::getWidth() / 2, Stage::getHeight() / 2);

			static const CL_Colorf START_COLOR(1.0f, 1.0f, 1.0f, 1.0f);

			m_countdownLabel.setText(_("START"));
			m_countdownLabel.setColor(START_COLOR);
			m_countdownLabel.draw(p_gc);

			p_gc.pop_modelview();
		}
	}
}

void RaceUIImpl::drawCountdownBg(CL_GraphicContext &p_gc)
{
	static const float BG_HEIGHT_PIX = 100;
	static const CL_Colorf BG_COLOR(0.0f, 0.0f, 0.0f, 0.3f);

	// draw bg
	const float x1 = 0.0f;
	const float y1 = Stage::getHeight() / 2 - BG_HEIGHT_PIX / 2;
	const float x2 = Stage::getWidth();
	const float y2 = y1 + BG_HEIGHT_PIX;

	CL_Draw::fill(p_gc, x1, y1, x2, y2, BG_COLOR);
}

void RaceUIImpl::drawGlobalMessage(CL_GraphicContext &p_gc)
{
	if (m_logic->getRaceGameState() == Race::GS_FINISHED_SINGLE) {
		m_globMsgLabel.setText(_("Race finished. Waiting for other players..."));
		m_globMsgLabel.draw(p_gc);
	}
}

void RaceUIImpl::drawScoreTable(CL_GraphicContext &p_gc)
{
	if (m_logic->getRaceGameState() == Race::GS_FINISHED_ALL) {
		m_scoreTable.draw(p_gc);
	}
}

void RaceUI::load(CL_GraphicContext &p_gc)
{
	m_impl->m_speedMeter.load(p_gc);
	m_impl->m_playerList.load(p_gc);
	m_impl->m_globMsgLabel.load(p_gc);
	m_impl->m_voteLabel.load(p_gc);
	m_impl->m_messageBoardLabel.load(p_gc);
	m_impl->m_lapLabel.load(p_gc);
	m_impl->m_lapTimesLabel.load(p_gc);
	m_impl->m_lapTimesLabelBold.load(p_gc);
	m_impl->m_carLabel.load(p_gc);
	m_impl->m_countdownLabel.load(p_gc);
	m_impl->m_scoreTable.load(p_gc);

}

void RaceUIImpl::handleRaceGameStateChanges(Race::RaceGameState p_from, Race::RaceGameState p_to)
{
	if (p_from == Race::GS_FINISHED_SINGLE && p_to == Race::GS_FINISHED_ALL) {
		// finish race action, invoke table display
		m_scoreTable.rebuild();
		m_scoreTable.restartAnimation();
	}
}

SpeedMeter &RaceUI::getSpeedMeter()
{
	return m_impl->m_speedMeter;
}

ScoreTable &RaceUI::getScoreTable()
{
	return m_impl->m_scoreTable;
}

} // namespace
