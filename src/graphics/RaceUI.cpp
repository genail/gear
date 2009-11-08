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

#include "Game.h"
#include "race/Car.h"
#include "race/RaceScene.h"
#include "graphics/Stage.h"

RaceUI::RaceUI(RaceScene *p_raceScene) :
	m_raceScene(p_raceScene),
	m_countDownStart(0)
{
}

RaceUI::~RaceUI()
{
}

void RaceUI::displayCountdown()
{
	m_countDownStart = CL_System::get_time();
}

void RaceUI::draw(CL_GraphicContext &p_gc)
{

	const Race::Car &car = Game::getInstance().getPlayer().getCar();


	int currentLap = car.getLap();
	const int lapNum = m_raceScene->getLapsTotal();

	// stop lap display at last lap even if the value is higher
	if (currentLap > lapNum) {
		currentLap = lapNum;
	}

	const CL_String currentLapStr = CL_StringHelp::int_to_local8(currentLap);
	const CL_String lapsNumStr = CL_StringHelp::int_to_local8(lapNum);

	// draw speed control
	m_speedMeter.draw(p_gc);

	// draw lap

	m_lapDisplayFont.draw_text(
			p_gc,
			Gfx::Stage::getWidth() - 130,
			40,
			CL_String8("Lap ") + currentLapStr + " / " + lapsNumStr
	);

	if (m_countDownStart != 0) {

		static const unsigned LABEL_3_END = 1000 - 1;
		static const unsigned LABEL_2_END = 2000 - 1;
		static const unsigned LABEL_1_END = 3000 - 1;
		static const unsigned LABEL_START_END = 4500 - 1;

		// display the countdown
		const unsigned delta = CL_System::get_time() - m_countDownStart;

		if (delta <= LABEL_3_END) {
			drawCountdownLabel(p_gc, "3");
		} else if (delta <= LABEL_2_END) {
			drawCountdownLabel(p_gc, "2");
		} else if (delta <= LABEL_1_END) {
			drawCountdownLabel(p_gc, "1");
		} else if (delta <= LABEL_START_END) {
			drawCountdownLabel(p_gc, "START");
		} else {
			m_countDownStart = 0;
		}
	}
}

void RaceUI::drawCountdownLabel(CL_GraphicContext &p_gc, const CL_String &p_label)
{
	const CL_Size size = m_countdownFont.get_text_size(p_gc, p_label);
	m_countdownFont.draw_text(
			p_gc,
			Gfx::Stage::getWidth() / 2 - size.width / 2,
			Gfx::Stage::getHeight() / 2 - size.height / 2,
			p_label,
			CL_Colorf::red
	);
}

void RaceUI::load(CL_GraphicContext &p_gc)
{
	// load speed meter
	m_speedMeter.load(p_gc);


	// load countdown font
	CL_FontDescription fontDesc;

	fontDesc.set_typeface_name("resources/DejaVuSansCondensed-BoldOblique.ttf");
	fontDesc.set_height(92);

	m_countdownFont = CL_Font_Freetype(p_gc, fontDesc);

	// load lap display font
	fontDesc.set_height(28);

	m_lapDisplayFont = CL_Font_Freetype(p_gc, fontDesc);

}

void RaceUI::update(unsigned p_timeElapsed)
{
	// set the speed meter
	const float carSpeed = Game::getInstance().getPlayer().getCar().getSpeedKMS();
	m_speedMeter.setSpeed(carSpeed);
}
