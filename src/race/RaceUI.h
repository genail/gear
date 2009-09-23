/*
 * RaceUI.h
 *
 *  Created on: 2009-09-23
 *      Author: chudy
 */

#ifndef RACEUI_H_
#define RACEUI_H_

#include "graphics/Drawable.h"

class RaceUI: public Drawable {
	public:
		RaceUI();
		virtual ~RaceUI();

		/**
		 * Displays the countdown as "3, 2, 1, START". Total
		 * Time needed to display "START" label equals 3 seconds from
		 * the moment of calling this method.
		 */
		void displayCountdown();

		virtual void draw(CL_GraphicContext &p_gc);
		virtual void load(CL_GraphicContext &p_gc);

	private:
		/** Countdown start time. If 0 then countdown didn't start or its already finished */
		unsigned m_countDownStart;

		/** Countdown font */
		CL_Font_Freetype m_countdownFont;

		void drawCountdownLabel(CL_GraphicContext &p_gc, const CL_String &p_label);
};

#endif /* RACEUI_H_ */
