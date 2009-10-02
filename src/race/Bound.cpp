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

}

Bound::~Bound() {
}

void Bound::draw(CL_GraphicContext &p_gc) {
	CL_Vec4f white_color(1.0f, 1.0f, 1.0f, 1.0f);
//
//	CL_Vec2f positions[] = { CL_Vec2f(m_segment.p.x,m_segment.p.y), CL_Vec2f(m_segment.q.x,m_segment.q.y) };
//	CL_Vec4f colors[] = { white_color, white_color };
//
//	CL_PrimitivesArray vertices(p_gc);
//	vertices.set_attributes(0, positions);
//	vertices.set_attributes(1, colors);

	CL_Pen pen;
	pen.set_line_width(3.0);

	p_gc.set_pen(pen);


	CL_Draw::line(p_gc, m_segment.p, m_segment.q, CL_Colorf::white);

//	p_gc.set_program(cl_program_color_only);
//	p_gc.set_program_object(cl_program_color_only);
//	p_gc.draw_primitives(cl_lines, 2, vertices);
}
