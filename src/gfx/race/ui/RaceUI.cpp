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

#include "common/Game.h"
#include "logic/race/Car.h"
#include "gfx/scenes/RaceScene.h"
#include "gfx/Stage.h"

namespace Gfx {

RaceUI::RaceUI() :
	m_label(CL_Pointf(100, 20), "", Label::F_BOLD, 20)
{
}

RaceUI::~RaceUI()
{
}

void RaceUI::draw(CL_GraphicContext &p_gc)
{

	// draw speed control
	m_speedMeter.draw(p_gc);

	m_label.setPosition(CL_Pointf(10, m_label.height()));
	m_label.setText("VOTE: Restart race? yes: 0 no: 0");
	m_label.draw(p_gc);

	m_label.setPosition(CL_Pointf(10, m_label.height() * 2));
	m_label.setText("To vote press F1 (YES) or F2 (NO)");
	m_label.draw(p_gc);
}

void RaceUI::load(CL_GraphicContext &p_gc)
{
	// load speed meter
	m_speedMeter.load(p_gc);

	m_label.load(p_gc);

}

} // namespace
