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

#include "RaceUITimeTrail.h"

#include "gfx/Stage.h"
#include "gfx/race/ui/Label.h"
#include "logic/race/GameLogicTimeTrailOnline.h"
#include "math/Time.h"
#include "ranking/RankingEntry.h"

namespace Gfx
{

class RaceUITimeTrailImpl
{
	public:

		RaceUITimeTrail *m_parent;
		const Race::GameLogicTimeTrailOnline &m_logic;

		Label m_bestOnlineTimeTitleLabel;
		Label m_bestOnlineTimeLabel;


		RaceUITimeTrailImpl(
				RaceUITimeTrail *p_parent, const Race::GameLogicTimeTrailOnline *p_logic);
		void positionLapTimeLabels();

		~RaceUITimeTrailImpl();

		void load(CL_GraphicContext &p_gc);
		void draw(CL_GraphicContext &p_gc);
};

RaceUITimeTrail::RaceUITimeTrail(const Race::GameLogicTimeTrailOnline *p_logic) :
		m_impl(new RaceUITimeTrailImpl(this, p_logic))
{
	// empty
}

RaceUITimeTrailImpl::RaceUITimeTrailImpl(
		RaceUITimeTrail *p_parent, const Race::GameLogicTimeTrailOnline *p_logic) :
		m_parent(p_parent),
		m_logic(*p_logic),
		m_bestOnlineTimeTitleLabel(CL_Pointf(), _("Best Online Lap"), Label::F_BOLD, 22),
		m_bestOnlineTimeLabel(CL_Pointf(), "", Label::F_REGULAR, 22)
{
	positionLapTimeLabels();

	m_bestOnlineTimeTitleLabel.setShadowVisible(true);
	m_bestOnlineTimeLabel.setShadowVisible(true);
}

void RaceUITimeTrailImpl::positionLapTimeLabels()
{
	const int BOTTOM_MARGIN = 20;
	const int RIGHT_MARGIN = 20;
	const int ENTRY_HEIGHT = 25;

	const int stageWidth = Stage::getWidth();
	const int stageHeight = Stage::getHeight();

	m_bestOnlineTimeLabel.setAttachPoint(Label::AP_RIGHT | Label::AP_BOTTOM);
	m_bestOnlineTimeLabel.setPosition(
			CL_Pointf(stageWidth - RIGHT_MARGIN, stageHeight - BOTTOM_MARGIN));

	m_bestOnlineTimeTitleLabel.setAttachPoint(Label::AP_RIGHT | Label::AP_BOTTOM);
	m_bestOnlineTimeTitleLabel.setPosition(
			CL_Pointf(stageWidth - RIGHT_MARGIN, stageHeight - BOTTOM_MARGIN - ENTRY_HEIGHT));
}

RaceUITimeTrail::~RaceUITimeTrail()
{
	// empty
}

RaceUITimeTrailImpl::~RaceUITimeTrailImpl()
{
	// empty
}

void RaceUITimeTrail::load(CL_GraphicContext &p_gc)
{
	m_impl->load(p_gc);
}

void RaceUITimeTrailImpl::load(CL_GraphicContext &p_gc)
{
	m_bestOnlineTimeLabel.load(p_gc);
	m_bestOnlineTimeTitleLabel.load(p_gc);
}

void RaceUITimeTrail::draw(CL_GraphicContext &p_gc)
{
	m_impl->draw(p_gc);
}

void RaceUITimeTrailImpl::draw(CL_GraphicContext &p_gc)
{
	if (m_logic.hasFirstPlaceRankingEntry()) {
		const RankingEntry &rankingEntry = m_logic.getFirstPlaceRankingEntry();
		Math::Time bestTime(rankingEntry.timeMs);

		m_bestOnlineTimeLabel.setText(
				cl_format("%1 by %2", bestTime.raceFormat(), rankingEntry.name));
	} else {
		m_bestOnlineTimeLabel.setText("--:--:---");
	}

	m_bestOnlineTimeTitleLabel.draw(p_gc);
	m_bestOnlineTimeLabel.draw(p_gc);
}

}
