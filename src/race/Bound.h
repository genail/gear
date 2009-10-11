/*
 * Bound.h
 *
 *  Created on: 2009-09-17
 *      Author: chudy
 */

#ifndef BOUND_H_
#define BOUND_H_

#include <ClanLib/core.h>

#ifdef CLIENT

#include "graphics/Drawable.h"

#define CLASS_BOUND class Bound : public Drawable

#else // CLIENT

#define CLASS_BOUND class Bound

#endif // CLIENT

CLASS_BOUND
{
	public:
		Bound(const CL_LineSegment2f &p_segment);
		virtual ~Bound();

		const CL_LineSegment2f& getSegment() { return m_segment; }

#ifdef CLIENT
		virtual void draw(CL_GraphicContext &p_gc);
		virtual void load(CL_GraphicContext &p_gc) {}
#endif // CLIENT

	private:
		/** Segment of this bound */
		CL_LineSegment2f m_segment;
};

#endif /* BOUND_H_ */
