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

#ifndef BOUND_H_
#define BOUND_H_

#include <ClanLib/core.h>

#ifndef SERVER

#include "graphics/Drawable.h"

#define CLASS_BOUND class Bound : public Drawable

#else // !SERVER

#define CLASS_BOUND class Bound

#endif // !SERVER

CLASS_BOUND
{
	public:

		Bound(const CL_LineSegment2f &p_segment);

		virtual ~Bound();

		const CL_LineSegment2f& getSegment() const { return m_segment; }

	private:

		/** Segment of this bound */
		CL_LineSegment2f m_segment;

#ifndef SERVER
	public:

		virtual void draw(CL_GraphicContext &p_gc);

		const CL_CollisionOutline &getCollisionOutline() const { return m_collisionOutline; }

		virtual void load(CL_GraphicContext &p_gc) {}

	private:

		CL_CollisionOutline m_collisionOutline;


#endif // !SERVER

};

#endif /* BOUND_H_ */
