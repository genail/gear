/*
 * Bound.h
 *
 *  Created on: 2009-09-17
 *      Author: chudy
 */

#ifndef BOUND_H_
#define BOUND_H_

#include <ClanLib/core.h>

#ifndef SERVER

#include "graphics/Drawable.h"

#define CLASS_BOUND class Bound : public Drawable

#else // !SERVER

#define CLASS_BOUND class Bound

#endif // !SERVER

CLASS_BOUND
{
	public:

		Bound(const CL_LineSegment2f &p_segment);

		virtual ~Bound();

		const CL_LineSegment2f& getSegment() const { return m_segment; }

	private:

		/** Segment of this bound */
		CL_LineSegment2f m_segment;

#ifndef SERVER
	public:

		virtual void draw(CL_GraphicContext &p_gc);

		const CL_CollisionOutline &getCollisionOutline() const { return m_collisionOutline; }

		virtual void load(CL_GraphicContext &p_gc) {}

	private:

		CL_CollisionOutline m_collisionOutline;


#endif // !SERVER

};

#endif /* BOUND_H_ */
