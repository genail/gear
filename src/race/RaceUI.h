/*
 * RaceUI.h
 *
 *  Created on: 2009-09-23
 *      Author: chudy
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
