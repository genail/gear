/*
 * Copyright (c) 2009, Piotr Korzuszek
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
#include "logic/race/Car.h"
#include "logic/race/RaceLogic.h"
#include "gfx/scenes/RaceScene.h"
#include "gfx/Stage.h"
#include "gfx/Viewport.h"

namespace Gfx {

struct Comparator {
	bool operator() (unsigned s1, unsigned s2) const
	{
		return s1 > s2; // reverse order
	}
};

RaceUI::RaceUI(const Race::RaceLogic *p_logic, const Gfx::Viewport *p_viewport) :
	m_playerList(p_logic),
	m_voteLabel(CL_Pointf(100, 20), "", Label::F_BOLD, 20),
	m_messageBoardLabel(CL_Pointf(), "", Label::F_BOLD, 14),
	m_lapLabel(CL_Pointf(Stage::getWidth() - 20, 5), "", Label::F_BOLD, 25),
	m_carLabel(CL_Pointf(), "", Label::F_BOLD, 14),
	m_logic(p_logic),
	m_viewport(p_viewport)
{
	m_playerList.setPosition(CL_Pointf(Stage::getWidth() - 150, 100));
	m_lapLabel.setAttachPoint(Label::AP_RIGHT | Label::AP_TOP);
	m_carLabel.setAttachPoint(Label::AP_CENTER | Label::AP_TOP);
}

RaceUI::~RaceUI()
{
}

void RaceUI::draw(CL_GraphicContext &p_gc)
{
	drawMeters(p_gc);
	drawVote(p_gc);
	drawMessageBoard(p_gc);
	drawLapLabel(p_gc);
	drawCarLabels(p_gc);
	drawPlayerList(p_gc);
}

void RaceUI::drawMeters(CL_GraphicContext &p_gc)
{
	// draw speed control
	m_speedMeter.draw(p_gc);
}

void RaceUI::drawVote(CL_GraphicContext &p_gc)
{
	if (m_logic->isVoteRunning()) {

		// calculate time left in seconds
		const unsigned now = CL_System::get_time();
		const unsigned deadline = m_logic->getVoteTimeout();

		const unsigned timeLeftSec = deadline >= now ? (deadline - now) / 1000 : 0;

		m_voteLabel.setText(
				cl_format(
						_("VOTE (%1): %2 yes: %3 no: %4"),
						timeLeftSec,
						m_logic->getVoteMessage(),
						m_logic->getVoteYesCount(),
						m_logic->getVoteNoCount()
				)
		);

		m_voteLabel.setPosition(CL_Pointf(10, m_voteLabel.size(p_gc).height));
		m_voteLabel.draw(p_gc);

		m_voteLabel.setPosition(CL_Pointf(10, m_voteLabel.size(p_gc).height * 2));
		m_voteLabel.setText(_("To vote press F1 (YES) or F2 (NO)"));
		m_voteLabel.draw(p_gc);
	}
}

void RaceUI::drawMessageBoard(CL_GraphicContext &p_gc)
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

void RaceUI::drawLapLabel(CL_GraphicContext &p_gc)
{
	const int lapsTotal = m_logic->getRaceLapCount();
	int lapsCurrent = Game::getInstance().getPlayer().getCar().getLap();

	if (lapsCurrent > lapsTotal) {
		lapsCurrent = lapsTotal;
	}

	m_lapLabel.setText(cl_format(_("Lap %1 / %2"), lapsCurrent, lapsTotal));
	m_lapLabel.draw(p_gc);
}

void RaceUI::drawCarLabels(CL_GraphicContext &p_gc)
{
	const Race::Level &level = m_logic->getLevel();
	const int carCount = level.getCarCount();

	CL_Pointf pos;

	for (int i = 0; i < carCount; ++i) {

		const Race::Car &car = level.getCar(i);
		pos = m_viewport->onScreen(car.getPosition());

		pos.y += 20;

		m_carLabel.setPosition(pos);
		m_carLabel.setText(car.getOwner()->getName());

		m_carLabel.draw(p_gc);
		m_carLabel.width();
	}
}

void RaceUI::drawPlayerList(CL_GraphicContext &p_gc)
{
	m_playerList.draw(p_gc);
}

void RaceUI::load(CL_GraphicContext &p_gc)
{
	m_speedMeter.load(p_gc);
	m_playerList.load(p_gc);
	m_voteLabel.load(p_gc);
	m_messageBoardLabel.load(p_gc);
	m_lapLabel.load(p_gc);
	m_carLabel.load(p_gc);

}

} // namespace
