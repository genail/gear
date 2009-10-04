/*
 * TyreStripes.cpp
 *
 *  Created on: 2009-10-02
 *      Author: chudy
 */

#include "TyreStripes.h"

#include "common.h"
#include "graphics/Stage.h"

TyreStripes::TyreStripes()
{
}

TyreStripes::~TyreStripes()
{
}

void TyreStripes::add(const CL_Pointf &p_from, const CL_Pointf &p_to)
{
	static const int STRIPE_LIMIT = 2000;

	Stripe stripe(p_from, p_to);
	m_stripes.push_back(stripe);

	// remove all stripes above the limit
	if (m_stripes.size() > STRIPE_LIMIT) {
		m_stripes.pop_front(); // there will be always one stripe to much
	}

}

void TyreStripes::draw(CL_GraphicContext &p_gc)
{
	CL_Pen pen;
	pen.set_line_width(3);
	p_gc.set_pen(pen);

	CL_Colorf color(0.0f, 0.0f, 0.0f, 0.5f);

	foreach (const Stripe& stripe,  m_stripes) {
		CL_Draw::line(p_gc, stripe.m_from, stripe.m_to, color);
	}

	Stage::getDebugLayer()->putMessage("stripes_num", CL_StringHelp::int_to_local8(m_stripes.size()));
}
