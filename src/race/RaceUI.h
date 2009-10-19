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

#ifndef RACEUI_H_
#define RACEUI_H_

#include "graphics/Drawable.h"
#include "graphics/SpeedMeter.h"

class Race;

class RaceUI: public Drawable {
	public:
		RaceUI(Race *p_race);
		virtual ~RaceUI();

		/**
		 * Displays the countdown as "3, 2, 1, START". Total
		 * Time needed to display "START" label equals 3 seconds from
		 * the moment of calling this method.
		 */
		void displayCountdown();

		virtual void draw(CL_GraphicContext &p_gc);
		virtual void load(CL_GraphicContext &p_gc);

		void update(unsigned p_timeElapsed);

	private:

		/** The Race pointer */
		Race *m_race;

		/** Countdown start time. If 0 then countdown didn't start or its already finished */
		unsigned m_countDownStart;

		/** Countdown font */
		CL_Font_Freetype m_countdownFont;

		/** Lap display font */
		CL_Font_Freetype m_lapDisplayFont;

		/** Speed control widget */
		SpeedMeter m_speedMeter;

		void drawCountdownLabel(CL_GraphicContext &p_gc, const CL_String &p_label);
};

#endif /* RACEUI_H_ */
