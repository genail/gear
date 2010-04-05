/*
 * Copyright (c) 2009-2010, Piotr Korzuszek
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
#include "math/Float.h"

namespace Gfx {

const int IMMUTABLE_STRIPE_LIMIT = 256;

const CL_Colorf STRIPE_COLOR(0.0f, 0.0f, 0.0f, 0.15f);
const CL_Vec4f STRIPE_COLOR_VEC(
		STRIPE_COLOR.r, STRIPE_COLOR.g, STRIPE_COLOR.b, STRIPE_COLOR.a
);

class Stripe {

	public:

		/** Stipe from -> to points */
		CL_Pointf m_from, m_to;

		/** Owner to continue last stripe */
		const Race::Car *m_owner;

		/** If this stripe is still mutable?  */
		bool m_mutable;

		Stripe(const CL_Pointf &p_from, const CL_Pointf &p_to, const Race::Car *p_owner) :
			m_from(p_from), m_to(p_to), m_owner(p_owner), m_mutable(true) {}


		float length() const { return m_from.distance(m_to); }

		const CL_Pointf &getFromPoint() const { return m_from; }

		const CL_Pointf &getToPoint() const { return m_to; }

};

// array of primitives for quicker drawing
class StripeArr
{
	private:

		/** Verticle array */
		CL_Vec2f *const m_verticles;

		/** Color array */
		CL_Vec4f *const m_colors;

		/** Compiled state */
		bool m_compiled;

		/** Primitives array */
		CL_PrimitivesArray *m_priArr;

		/** m_size * 2 */
		const int m_size2;

		void compile(CL_GraphicContext &p_gc) {
			G_ASSERT(!m_compiled);

			m_priArr = new CL_PrimitivesArray(p_gc);
			m_priArr->set_attributes(0, m_verticles);
			m_priArr->set_attributes(1, m_colors);

			m_compiled = true;
		}

	public:

		/** Array size */
		const int m_size;

		StripeArr(int p_size) :
			m_verticles(new CL_Vec2f[p_size * 2]),
			m_colors(new CL_Vec4f[p_size * 2]),
			m_size(p_size),
			m_size2(p_size * 2),
			m_compiled(false),
			m_priArr(NULL)
		{ /* empty */ }

		~StripeArr() {
			if (m_priArr) {
				delete m_priArr;
			}
			delete[] m_colors;
			delete[] m_verticles;
		}

		void add(int p_idx, const Stripe &p_stripe) {
			G_ASSERT(!m_compiled);
			G_ASSERT(p_idx >= 0 && p_idx < m_size);

			const int idx2 = p_idx * 2;

			m_verticles[idx2] = p_stripe.m_from;
			m_verticles[idx2 + 1] = p_stripe.m_to;

			m_colors[idx2] = STRIPE_COLOR_VEC;
			m_colors[idx2 + 1] = STRIPE_COLOR_VEC;
		}

		void draw(CL_GraphicContext &p_gc) {
			if (!m_compiled) {
				compile(p_gc);
			}

			p_gc.set_program_object(cl_program_color_only);
			p_gc.draw_primitives(cl_lines, m_size2, *m_priArr);
		}
};


class TyreStripesImpl
{
	public:

		typedef std::list<Stripe> TStripeList;
		typedef std::deque<StripeArr*> TStripeArrList;
		typedef std::map<const Race::Car*, CL_Pointf> TDriftPointMap;

		/** Level at what stripes are drawn */
		const Race::Level *const m_level;

		/** Stripes container. First stripe is the youngest one. */
		TStripeList m_stripes;

		/** Stripe arrays containter */
		TStripeArrList m_stripeArrays;

		/** Immutable stripes count (for optimalization) */
		int m_immutableStripeCnt;

		/** Last drift point map */
		TDriftPointMap m_lastDriftMap;


		TyreStripesImpl(const Race::Level *p_level) :
			m_level(p_level),
			m_immutableStripeCnt(0)
		{ /* empty */ }

		~TyreStripesImpl() {
			clear();
		}

		void add(
				const CL_Pointf &p_from,
				const CL_Pointf &p_to,
				const Race::Car *p_owner
		);

		void add4WheelStripe(const Race::Car &p_car, const CL_Pointf &p_from);


		void clear();


		bool equals(
				const CL_Pointf &p_a, const CL_Pointf &p_b,
				float p_precission
		);


		/** Splits immutable sprites from mutable */
		void splitImmutable();

};

TyreStripes::TyreStripes(const Race::Level *p_level) :
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
	static const unsigned STRIPE_LENGTH_LIMIT = 30;
	// how many last stripes that belongs to this car should I check?
	static const int BACK_SEARCH_LIMIT = 4;
	// point equals check precission
	static const float EQUAL_CHECK_PRECISSION = 3.0f;

	// search for four last stripes of this car and check if I can merge
	// this stripe to the last one
	bool merged = false;
	int foundCount = 0;

	// last found stripe
	Stripe *lastFound = NULL;

	for (
			TStripeList::iterator itor = m_stripes.begin();
			itor != m_stripes.end();
			++itor
	) {
		Stripe &s = *itor;

		if (s.m_owner == p_owner) {

			++foundCount;

			// must end on the same point and length must be below limit
			if (
					equals(s.m_to, p_from, EQUAL_CHECK_PRECISSION)
					&& s.length() < STRIPE_LENGTH_LIMIT
				) {
				G_ASSERT(s.m_mutable);

				// remove old stripe
				Stripe copy = s;
				m_stripes.erase(itor);

				// and construct new one
				m_stripes.push_front(Stripe(copy.m_from, p_to, p_owner));
				merged = true;

				break;
			}

			if (foundCount == BACK_SEARCH_LIMIT) {
				lastFound = &s;
				break;
			}
		}
	}

	if (!merged) {
		// when not merged, then create a new stripe
		m_stripes.push_front(Stripe(p_from, p_to, p_owner));

		// set the last found as immutable
		if (lastFound) {
			lastFound->m_mutable = false;
			m_immutableStripeCnt++;
		}

	}

}

bool TyreStripesImpl::equals(
		const CL_Pointf &p_a, const CL_Pointf &p_b,
		float p_precission
)
{
	return Math::Float::cmp(p_a.x, p_b.x, p_precission)
		&& Math::Float::cmp(p_a.y, p_b.y, p_precission);
}

void TyreStripes::clear()
{
	m_impl->clear();
}

void TyreStripesImpl::clear()
{
	m_stripes.clear();
	m_lastDriftMap.clear();


	foreach (StripeArr *sarr, m_stripeArrays) {
		delete sarr;
	}

	m_stripeArrays.clear();
}

void TyreStripes::update()
{
	const int carCount = m_impl->m_level->getCarCount();
	TyreStripesImpl::TDriftPointMap::iterator itor;

	for (int i = 0; i < carCount; ++i) {
		const Race::Car &car = m_impl->m_level->getCar(i);
		itor = m_impl->m_lastDriftMap.find(&car);

		if (car.isDrifting()) {
			// add drift point if has last drift point
			if (itor != m_impl->m_lastDriftMap.end()) {
				//m_impl->add(itor->second, car.getPosition(), &car);
				m_impl->add4WheelStripe(car, itor->second);
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

	// check immutable stripes count for optimalization
	if (m_impl->m_immutableStripeCnt >= IMMUTABLE_STRIPE_LIMIT) {
		cl_log_event(LOG_DEBUG, "split stripes: %1, %2", m_impl->m_stripes.size(), m_impl->m_stripeArrays.size());
		m_impl->splitImmutable();
	}
}

void TyreStripesImpl::add4WheelStripe(
		const Race::Car &p_car,
		const CL_Pointf &p_from
)
{
	static const int WHEEL_COUNT = 4;
	static const float TYRE_RADIUS = 20.0f; // tire distance from car center
	static const float DEG_90_RAD = CL_PI / 2;
	static const float DEG_45_RAD = DEG_90_RAD / 2;

	const CL_Pointf carPos = p_car.getPosition();
	const CL_Vec2f posDelta = carPos - p_from;

	CL_Angle angle = p_car.getCorpseAngle();

	CL_Vec2f v;
	float rad;

	CL_Pointf tyrePosA, tyrePosB;

	for (int i = 0; i < WHEEL_COUNT; ++i) {
		angle += CL_Angle(i == 0 ? DEG_45_RAD : DEG_90_RAD, cl_radians);

		rad = angle.to_radians();
		v.x = cos(rad);
		v.y = sin(rad);

		v.normalize();

		v *= TYRE_RADIUS;

		tyrePosB = carPos + v;
		tyrePosA = tyrePosB - posDelta;

		add(tyrePosA, tyrePosB, &p_car);
	}
}

void TyreStripesImpl::splitImmutable()
{
	static const unsigned ARRAYS_LIMIT = 128;

	// I need to skip first immutable stripes in order to take back
	// the oldest ones
	int skipCount = m_immutableStripeCnt - IMMUTABLE_STRIPE_LIMIT;
	m_immutableStripeCnt -= IMMUTABLE_STRIPE_LIMIT;

	StripeArr *arr = new StripeArr(IMMUTABLE_STRIPE_LIMIT);
	int idx = 0;

	for (
			TStripeList::iterator itor = m_stripes.begin();
			itor != m_stripes.end();
	) {
		Stripe &s = *itor;
		if (!s.m_mutable) {

			// skip first stripes if necessary
			if (skipCount == 0) {
				arr->add(idx++, s);
				m_stripes.erase(itor++);
			} else {
				skipCount--;
				itor++;
			}
		} else {
			itor++;
		}
	}

	m_stripeArrays.push_back(arr);

	if (m_stripeArrays.size() > ARRAYS_LIMIT) {
		delete m_stripeArrays.front();
		m_stripeArrays.pop_front();
	}
}

void TyreStripes::draw(CL_GraphicContext &p_gc)
{
	CL_Pen oldPen = p_gc.get_pen();

	CL_Pen newPen;
	newPen.set_line_width(3);
	p_gc.set_pen(newPen);

	// draw arrays
	foreach (StripeArr *sarr, m_impl->m_stripeArrays) {
		sarr->draw(p_gc);
	}

	foreach (const Stripe &stripe, m_impl->m_stripes) {
		const CL_Pointf &from = stripe.getFromPoint();
		const CL_Pointf &to = stripe.getToPoint();

		CL_Draw::line(p_gc, from, to, STRIPE_COLOR);
	}
}

} // namespace

