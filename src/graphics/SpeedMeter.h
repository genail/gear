/*
 * SpeedMeter.h
 *
 *  Created on: 2009-10-10
 *      Author: chudy
 */

#ifndef SPEEDMETER_H_
#define SPEEDMETER_H_

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include "graphics/Drawable.h"

class SpeedMeter : public Drawable {

	public:

		SpeedMeter();

		virtual ~SpeedMeter();

		virtual void draw(CL_GraphicContext &p_gc);

		virtual void load(CL_GraphicContext &p_gc);

	private:

		/** Background of speed control widget */
		CL_Sprite m_speedControlBg;

		/** Arrow od speed control widget. It's facing north. */
		CL_Sprite m_speedControlArrow;
};

#endif /* SPEEDMETER_H_ */
