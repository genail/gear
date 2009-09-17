/*
 * Bound.h
 *
 *  Created on: 2009-09-17
 *      Author: chudy
 */

#ifndef BOUND_H_
#define BOUND_H_

#include <ClanLib/core.h>

#include "graphics/Drawable.h"

class Bound : public Drawable {
	public:
		Bound(const CL_LineSegment2f &p_segment);
		virtual ~Bound();

		const CL_LineSegment2f& getSegment() { return m_segment; }

		virtual void draw(CL_GraphicContext &p_gc);
		virtual void load(CL_GraphicContext &p_gc) {}

	private:
		/** Segment of this bound */
		CL_LineSegment2f m_segment;
};

#endif /* BOUND_H_ */
