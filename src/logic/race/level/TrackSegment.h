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

#pragma once

#include <vector>

#include <ClanLib/core.h>

namespace Race
{

class TrackSegmentImpl;

class TrackSegment
{

	public:

		class PointPair
		{
			public:
				PointPair(const CL_Pointf &p_left, CL_Pointf& p_right) :
					m_left(p_left),
					m_right(p_right)
				{ /* empty */ }

				CL_Pointf m_left, m_right;

				bool operator=(const PointPair &p_other) {
					m_left = p_other.m_left;
					m_right = p_other.m_right;

					return true;
				}
		};


		TrackSegment(
				const std::vector<CL_Pointf> &p_triPoints,
				const std::vector<CL_Pointf> &p_midPoints,
				const std::vector<PointPair> &p_pairs
		);

		virtual ~TrackSegment();


		/** @return bounding rectangle (reverse y) */
		const CL_Rectf &getBounds() const;

		/** @return Middle track points from what track was constructed. */
		const std::vector<CL_Pointf> &getMidPoints() const;

		const std::vector<CL_Pointf> &getTrianglePoints() const;

		const std::vector<PointPair> &getPointPairs() const;


	private:

		CL_SharedPtr<TrackSegmentImpl> m_impl;
};

}

