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

// needed for CL_VERSION
#include <ClanLib/core.h>

// this file contains workarounds to known problems in used libraries

namespace Workarounds
{
	/** Clanlib <= 2.1.1 fix. Use this to normalize angles. */
	inline void clAngleNormalize(CL_Angle *p_angle)
	{
		#if CL_CURRENT_VERSION <= CL_VERSION(2,1,1)
		p_angle->normalize();
		if (p_angle->to_radians() < 0) {
			*p_angle += CL_Angle(2 * CL_PI, cl_radians);
		}
		#else
			p_angle->normalize();
		#endif
	}

	/** Clanlib <= 2.1.1 fix. Use this to normalize angles. */
	inline void clAngleNormalize180(CL_Angle *p_angle)
	{
		#if CL_CURRENT_VERSION <= CL_VERSION(2,1,1)

		clAngleNormalize(p_angle);
		p_angle->set_radians(p_angle->to_radians() - CL_PI);

		#else

		p_angle->normalize_180();

		#endif
	}
} // namespace
