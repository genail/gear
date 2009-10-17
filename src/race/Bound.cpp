/*
 * Bound.cpp
 *
 *  Created on: 2009-09-17
 *      Author: chudy
 */

#include "Bound.h"

Bound::Bound(const CL_LineSegment2f &p_segment) :
	m_segment(p_segment)
{
#ifndef SERVER
	// build this bound contour
	CL_Contour contour;

	contour.get_points().push_back(p_segment.p);
	contour.get_points().push_back(p_segment.q);

	m_collisionOutline.get_contours().push_back(contour);

	m_collisionOutline.set_inside_test(true);

	m_collisionOutline.calculate_radius();
	m_collisionOutline.calculate_smallest_enclosing_discs();
#endif // !SERVER
}

Bound::~Bound() {
}

#ifndef SERVER

void Bound::draw(CL_GraphicContext &p_gc) {
	CL_Vec4f white_color(1.0f, 1.0f, 1.0f, 1.0f);

	CL_Pen pen;
	pen.set_line_width(3.0);

	p_gc.set_pen(pen);


	CL_Draw::line(p_gc, m_segment.p, m_segment.q, CL_Colorf::white);

#if !defined(SERVER) && defined(DRAW_COLLISION_OUTLINE)
	m_collisionOutline.draw(0, 0, CL_Colorf::red, p_gc);
#endif //!SERVER && DRAW_COLLISION_OUTLINE
}

#endif // !SERVER
