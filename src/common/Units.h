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

const float SCREEN_WORLD_RATIO = 20.0f;

class Units
{
	public:

		virtual ~Units();


		/** Converts screen unit to world unit */
		static float toWorld(float p_scrVal);

		/** Converts screen unit coord to world unit coord */
		static CL_Pointf toWorld(const CL_Pointf &p_scrPt);

		/** Converts world unit to screen unit */
		static float toScreen(float p_worldVal);

		/** Converts world unit coord to screen unit coord */
		static CL_Pointf toScreen(const CL_Pointf &p_worldPt);

	private:

		Units();
};

inline
float Units::toWorld(float p_scrVal)
{
	return p_scrVal / SCREEN_WORLD_RATIO;
}

inline
CL_Pointf Units::toWorld(const CL_Pointf &p_scrPt)
{
	return CL_Pointf(toWorld(p_scrPt.x), toWorld(p_scrPt.y));
}

inline
float Units::toScreen(float p_worldVal)
{
	return p_worldVal * SCREEN_WORLD_RATIO;
}

inline
CL_Pointf Units::toScreen(const CL_Pointf &p_worldPt)
{
	return CL_Pointf(toScreen(p_worldPt.x), toScreen(p_worldPt.y));
}
