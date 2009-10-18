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

void TyreStripes::add(const CL_Pointf &p_from, const CL_Pointf &p_to, const Car *p_owner)
{
	static const unsigned STRIPE_LIMIT = 400;
	static const unsigned STRIPE_LENGTH_LIMIT = 15;
	static const unsigned BACK_SEARCH_LIMIT = 4;

	// search for four last stripes of this car and check if I can merge
	// this stripe to the last one
	bool merged = false;
	unsigned foundCount = 0;

	for (
			std::list<Stripe>::iterator ritor = m_stripes.end();
			ritor != m_stripes.begin();
			--ritor
	) {
		Stripe s = *ritor;

		if (s.m_owner == p_owner) {

			++foundCount;

			// must end on the same point and length must be below limit
			if (s.m_to == p_from && s.length() < STRIPE_LENGTH_LIMIT) {
				Stripe copy = s;

				// remove old stripe
				m_stripes.erase(ritor);

				// and construct new one
				m_stripes.push_back(Stripe(copy.m_from, p_to, p_owner));
				merged = true;

				break;
			}

			if (foundCount == BACK_SEARCH_LIMIT) {
				break;
			}
		}
	}

	if (!merged) {
		// when not merged, then create a new stripe
		m_stripes.push_back(Stripe(p_from, p_to, p_owner));
	}

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

#ifndef NDEBUG
	Stage::getDebugLayer()->putMessage("stripes_num", CL_StringHelp::int_to_local8(m_stripes.size()));
#endif // !NDEBUG
}
