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

#pragma once

#include <ClanLib/core.h>
#include <list>

#include "common.h"

namespace RaceResistance {

class Primitive;

class Geometry : public boost::noncopyable {

	public:

		Geometry();

		virtual ~Geometry();


		const CL_Rectf &getBounds() const;


		void addCircle(const CL_Circlef &p_circle);

		void addRectangle(const CL_Rectf &p_rectangle);

		void andCircle(const CL_Circlef &p_circle);

		void andRect(const CL_Rectf &p_rectangle);

		bool contains(const CL_Pointf &p_point) const;

		void subtractCircle(const CL_Circlef &p_circle);

		void subtractRect(const CL_Rectf &p_rectangle);

	private:

		CL_Rectf m_bounds;

		/** False from the beggining. Set to true if bounds are first time set to real value. */
		bool m_boundsSet;

		typedef std::list<const Primitive*> TPrimitivesList;
		TPrimitivesList m_primitives;


		void updateBounds(const CL_Circlef &p_circle);

		void updateBounds(const CL_Rectf &p_rect);

		void updateBounds(float p_l, float p_t, float p_r, float p_b);

};

} // namespace

