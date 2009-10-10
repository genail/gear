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
	static const unsigned STRIPE_LIMIT = 2000;
	static const unsigned STRIPE_LENGTH_LIMIT = 100;
	static const unsigned BACK_SEARCH_LIMIT = 100;

	// search for four last stripes of this car and check if I can merge
	// this stripe to the last one
	bool merged = false;
	unsigned foundCount = 0;

	for (
			std::list<Stripe>::reverse_iterator ritor = m_stripes.rbegin();
			ritor != m_stripes.rend();
			++ritor
	) {
		Stripe& s = *ritor;

		if (s.m_owner == p_owner) {

			++foundCount;

			// must end on the same point and length must be below limit
			if (s.m_to == p_from && s.length() < STRIPE_LENGTH_LIMIT) {
				Stripe copy = s;

				// remove old stripe
//				cl_log_event("debug", "before: %1", m_stripes.size());
				m_stripes.erase(ritor.base());
//				cl_log_event("debug", "after: %1", m_stripes.size());

				// and construct new one
				m_stripes.push_back(Stripe(copy.m_from, p_to, p_owner));
				merged = true;

				cl_log_event("debug", "merged" );

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
		cl_log_event("debug", "nothing merged" );
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

	Stage::getDebugLayer()->putMessage("stripes_num", CL_StringHelp::int_to_local8(m_stripes.size()));
}
