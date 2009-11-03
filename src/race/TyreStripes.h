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

#ifndef TYRESTRIPES_H_
#define TYRESTRIPES_H_

#include <list>
#include <ClanLib/core.h>

#include "race/Car.h"
#include "graphics/Drawable.h"

class TyreStripes : public Gfx::Drawable {

		struct Stripe {
				CL_Pointf m_from, m_to;
				const Car *m_owner;

				Stripe(const CL_Pointf &p_from, const CL_Pointf &p_to, const Car *p_owner) :
					m_from(p_from), m_to(p_to), m_owner(p_owner) {}

				float length() const
				{ return m_from.distance(m_to); }
		};

	public:
		TyreStripes();
		virtual ~TyreStripes();

		void add(const CL_Pointf &p_from, const CL_Pointf &p_to, const Car *p_owner);

		virtual void draw(CL_GraphicContext &p_gc);

	private:
		std::list<Stripe> m_stripes;
};

#endif /* TYRESTRIPES_H_ */
