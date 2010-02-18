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

#include "ScoreTable.h"


#include <list>
#include <math.h>

#include "common.h"
#include "common/Player.h"
#include "gfx/Stage.h"
#include "gfx/race/ui/Label.h"
#include "math/Float.h"
#include "math/Time.h"
#include "logic/race/Progress.h"
#include "logic/race/RaceLogic.h"

namespace Gfx
{

const int MAX_ROWS = 32;
const float ENTRY_HEIGHT = 46;
const float SPACING_0 = 40;
const float SPACING_1_3 = 30;
const float SPACING_4_MORE = 10;

// precalculated positions table
float startPos[MAX_ROWS];
float endPos[MAX_ROWS];

/** One table row */
class TableRow
{
	public:

		const int m_place;

		const CL_String m_name;

		const Math::Time m_best, m_total;


		TableRow(
				int p_place,
				const CL_String &p_name,
				const Math::Time &p_best,
				const Math::Time &p_total
		) :
			m_place(p_place),
			m_name(p_name),
			m_best(p_best),
			m_total(p_total)

		{ /* empty */ }


		void draw(
				CL_GraphicContext &p_gc,
				float p_animProgress,
				Gfx::Label &p_highPosLabel,
				Gfx::Label &p_lowPosLabel,
				Gfx::Label &p_nickLabel,
				Gfx::Label &p_timeLabel
		);


}; // class TableRow

class TimeEntry
{
	public:

		const Player &m_player;

		const int m_best, m_total;


		TimeEntry(const Player &p_player, int p_best, int p_total) :
			m_player(p_player),
			m_best(p_best),
			m_total(p_total)
		{ /* empty */ }

		bool operator<(const TimeEntry &p_te)
		{
			return m_total < p_te.m_total;
		}
}; // class TimeEnty

class ScoreTableImpl
{
	public:

		const Race::RaceLogic *m_logic;

		std::vector<TableRow*> m_rows;

		// labels

		Gfx::Label m_highPosLabel;

		Gfx::Label m_lowPosLabel;

		Gfx::Label m_nickLabel;

		Gfx::Label m_timeLabel;

		// animation progress
		Math::Float m_animProgress;



		ScoreTableImpl(const Race::RaceLogic *p_logic) :
			m_logic(p_logic),
			m_highPosLabel(CL_Pointf(), "", Gfx::Label::F_PIXEL, 108),
			m_lowPosLabel(CL_Pointf(), "", Gfx::Label::F_PIXEL, 72),
			m_nickLabel(CL_Pointf(), "", Gfx::Label::F_PIXEL, 36),
			m_timeLabel(CL_Pointf(), "", Gfx::Label::F_PIXEL, 24),
			m_animProgress(1.0f)
		{
			// calculate y positions
			int y;

			for (int i = 0; i < MAX_ROWS; ++i) {
				// at end
				y = SPACING_0 + (ENTRY_HEIGHT * i);

				if (i <= 3) {
					y += SPACING_1_3 * i;
				} else {
					y += SPACING_1_3 * 3;
					y += SPACING_4_MORE * (i - 3);
				}

				endPos[i] = y;

				// at start
				y += Stage::getHeight() * logf((i + 1) * 4);
				startPos[i] = y;
			}
		}

}; // class ScoreTableImpl

ScoreTable::ScoreTable(const Race::RaceLogic *p_logic) :
	m_impl(new ScoreTableImpl(p_logic))
{
	// empty
}

ScoreTable::~ScoreTable()
{
	// empty
}

void ScoreTable::load(CL_GraphicContext &p_gc)
{
	Drawable::load(p_gc);
	m_impl->m_highPosLabel.load(p_gc);
	m_impl->m_lowPosLabel.load(p_gc);
	m_impl->m_nickLabel.load(p_gc);
	m_impl->m_timeLabel.load(p_gc);
}

void ScoreTable::draw(CL_GraphicContext &p_gc)
{
	foreach (TableRow *row, m_impl->m_rows) {
		row->draw(
				p_gc,
				m_impl->m_animProgress.get(),
				m_impl->m_highPosLabel,
				m_impl->m_lowPosLabel,
				m_impl->m_nickLabel,
				m_impl->m_timeLabel
		);
	}
}

void TableRow::draw(
		CL_GraphicContext &p_gc,
		float p_animProgress,
		Gfx::Label &p_highPosLabel,
		Gfx::Label &p_lowPosLabel,
		Gfx::Label &p_nickLabel,
		Gfx::Label &p_timeLabel
)
{
	// place position offset (from entry center)
	static const CL_Vec2f POS_OFFSET(-300.0f, 0.0f);

	// nick position offset (from entry center)
	static const CL_Vec2f NICK_OFFSET(-200.0f, 0.0f);

	// time position offset (from entry center)
	static const CL_Vec2f TIME_OFFSET(300.0f, 0.0f);


	// background color
	static const CL_Colorf BG_COLOR(0.0f, 0.0f, 0.0f, 0.7f);


	// animation start and end y
	const float sy = startPos[m_place - 1];
	const float ey = endPos[m_place - 1];


	const float y = sy + (ey - sy) * p_animProgress;

	// draw bg
	CL_Draw::fill(
			p_gc,
			0, y,
			Gfx::Stage::getWidth(), y + ENTRY_HEIGHT,
			BG_COLOR
	);

	const CL_Pointf entryCenter(Stage::getWidth() / 2, y + ENTRY_HEIGHT / 2);

	Gfx::Label *posLabel;

	if (m_place <= 3) {
		posLabel = &p_highPosLabel;
	} else {
		posLabel = &p_lowPosLabel;
	}

	// draw position label
	posLabel->setPosition(entryCenter + POS_OFFSET);
	posLabel->setAttachPoint(Gfx::Label::AP_CENTER);
	posLabel->setText(CL_StringHelp::int_to_local8(m_place));

	posLabel->draw(p_gc);

	// draw nick label
	p_nickLabel.setPosition(entryCenter + NICK_OFFSET);
	p_nickLabel.setAttachPoint(Gfx::Label::AP_LEFT | Gfx::Label::AP_CENTER);
	p_nickLabel.setText(m_name);

	p_nickLabel.draw(p_gc);

	// draw times
	const float th2 = p_timeLabel.height() / 2.0f;

	p_timeLabel.setAttachPoint(Gfx::Label::AP_RIGHT | Gfx::Label::AP_CENTER);

	p_timeLabel.setPosition(entryCenter + TIME_OFFSET + CL_Vec2f(0.0f, -th2));
	p_timeLabel.setText(cl_format(_("TIME: %1"), m_total.raceFormat()));

	p_timeLabel.draw(p_gc);

	p_timeLabel.setPosition(entryCenter + TIME_OFFSET + CL_Vec2f(0.0f, +th2));
	p_timeLabel.setText(cl_format(_("BEST: %1"), m_best.raceFormat()));

	p_timeLabel.draw(p_gc);
}

void ScoreTable::rebuild()
{
	G_ASSERT(m_impl->m_logic->getRaceState() == Race::S_FINISHED_ALL);

	// clear last table
	foreach (TableRow *row, m_impl->m_rows) {
		delete row;
	}
	m_impl->m_rows.clear();

	const int lapCount = m_impl->m_logic->getRaceLapCount();
	const int pCount = m_impl->m_logic->getPlayerCount();

	G_ASSERT(pCount <= MAX_ROWS && "limit exhausted");

	const Race::Progress &progress = m_impl->m_logic->getProgress();

	// store time entries
	std::list<TimeEntry> timeEntries;
	int lap, best, total;

	for (int i = 0; i < pCount; ++i) {
		const Player &player = m_impl->m_logic->getPlayer(i);
		const Race::Car &car = player.getCar();

		// calculate best lap and total time
		total = 0;
		for (int lapIdx = 1; lapIdx <= lapCount; ++lapIdx) {
			lap = progress.getLapTime(car, lapIdx);
			total += lap;

			if ((lapIdx !=10 && lap < best) || lapIdx == 1) {
				best = lap;
			}
		}

		timeEntries.push_back(TimeEntry(player, best, total));
	}

	// sort and build table rows
	timeEntries.sort();

	int i = 1;
	foreach (const TimeEntry &te, timeEntries) {
		m_impl->m_rows.push_back(
				new TableRow(i++, te.m_player.getName(), te.m_total, te.m_best)
		);
	}

}

void ScoreTable::restartAnimation()
{
	m_impl->m_animProgress.animate(0.0f, 1.0f, 1000, Math::Easing::REGULAR_OUT);
}

void ScoreTable::update(unsigned p_timeElapsed)
{
	m_impl->m_animProgress.update(p_timeElapsed);
}

}
