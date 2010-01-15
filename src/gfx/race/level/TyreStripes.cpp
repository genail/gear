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

#include <map>
#include <list>

#include "common.h"
#include "logic/race/Car.h"
#include "logic/race/level/Level.h"

namespace Gfx {

class Stripe {

	public:

		/** Stipe from -> to points */
		CL_Pointf m_from, m_to;

		/** Owner to continue last stripe */
		const Race::Car *m_owner;

		Stripe(const CL_Pointf &p_from, const CL_Pointf &p_to, const Race::Car *p_owner) :
			m_from(p_from), m_to(p_to), m_owner(p_owner) {}


		float length() const { return m_from.distance(m_to); }

		const CL_Pointf &getFromPoint() const { return m_from; }

		const CL_Pointf &getToPoint() const { return m_to; }

};


class TyreStripesImpl
{
	public:

		typedef std::list<Stripe> TStripeList;
		typedef std::map<const Race::Car*, CL_Pointf> TDriftPointMap;

		/** Level at what stripes are drawn */
		Race::Level m_level;

		/** Stripes container */
		TStripeList m_stripes;

		/** Last drift point map */
		TDriftPointMap m_lastDriftMap;


		TyreStripesImpl(const Race::Level &p_level) :
			m_level(p_level)
		{ /* empty */ }


		void add(
				const CL_Pointf &p_from,
				const CL_Pointf &p_to,
				const Race::Car *p_owner
		);

};

TyreStripes::TyreStripes(const Race::Level &p_level) :
	m_impl(new TyreStripesImpl(p_level))
{
	// empty
}

TyreStripes::~TyreStripes()
{
	// empty
}

void TyreStripesImpl::add(
		const CL_Pointf &p_from,
		const CL_Pointf &p_to,
		const Race::Car *p_owner
)
{
	static const unsigned STRIPE_LIMIT = 200;
	static const unsigned STRIPE_LENGTH_LIMIT = 15;
	static const unsigned BACK_SEARCH_LIMIT = 4;

	// search for four last stripes of this car and check if I can merge
	// this stripe to the last one
	bool merged = false;
	unsigned foundCount = 0;

	for (
			TStripeList::iterator itor = m_stripes.begin();
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

void TyreStripes::clear()
{
	m_impl->m_stripes.clear();
	m_impl->m_lastDriftMap.clear();
}

void TyreStripes::update()
{
	const int carCount = m_impl->m_level.getCarCount();
	TyreStripesImpl::TDriftPointMap::iterator itor;

	for (int i = 0; i < carCount; ++i) {
		const Race::Car &car = m_impl->m_level.getCar(i);
		itor = m_impl->m_lastDriftMap.find(&car);

		if (car.isDrifting()) {
			// add drift point if has last drift point
			if (itor != m_impl->m_lastDriftMap.end()) {
				m_impl->add(itor->second, car.getPosition(), &car);
			}

			// remember this point
			m_impl->m_lastDriftMap[&car] = car.getPosition();
		} else {
			// remove last drift point if not drifting
			if (itor != m_impl->m_lastDriftMap.end()) {
				m_impl->m_lastDriftMap.erase(itor);
			}
		}
	}

}

void TyreStripes::draw(CL_GraphicContext &p_gc)
{

}

} // namespace

