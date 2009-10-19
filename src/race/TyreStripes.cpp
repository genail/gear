/*
 * Copyright (c) 2009, Piotr Korzuszek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
			std::list<Stripe>::iterator itor = m_stripes.begin();
			itor != m_stripes.end();
			++itor
	) {
		Stripe s = *itor;

		if (s.m_owner == p_owner) {

			++foundCount;

			// must end on the same point and length must be below limit
			if (s.m_to == p_from && s.length() < STRIPE_LENGTH_LIMIT) {
				Stripe copy = s;

				// remove old stripe
				m_stripes.erase(itor);

				// and construct new one
				m_stripes.push_front(Stripe(copy.m_from, p_to, p_owner));
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
		m_stripes.push_front(Stripe(p_from, p_to, p_owner));
	}

	// remove all stripes above the limit
	if (m_stripes.size() > STRIPE_LIMIT) {
		m_stripes.pop_back(); // there will be always one stripe to much
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
