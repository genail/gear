/*
 * Drawable.h
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#include <ClanLib/display.h>

#ifndef DRAWABLE_H_
#define DRAWABLE_H_

class Drawable {
	public:
		virtual void draw(CL_GraphicContext &p_gc) = 0;
		virtual void load(CL_GraphicContext &p_gc) {}

};

#endif /* DRAWABLE_H_ */
