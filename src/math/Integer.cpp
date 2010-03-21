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

#include "Integer.h"

#include <iostream>

//#include "common.h"
#include "common/gassert.h"

namespace Math
{

static const int INT_SEG_CNT = sizeof(int) * 2;

Integer::Integer()
{
	// empty
}

Integer::~Integer()
{
	// empty
}

int Integer::clamp(int p_val, int p_min, int p_max)
{
	G_ASSERT(p_max >= p_min);

	if (p_val >= p_min && p_val <= p_max) {
		return p_val;
	}

	p_val = p_val % (p_max + 1);

	if (p_val < p_min) {
		p_val += (p_max + 1);
	}

	return p_val;
}

unsigned Integer::fromHex(const CL_String &p_str)
{
	G_ASSERT(
			static_cast<int>(p_str.length()) <= INT_SEG_CNT
			&& "int size too small"
	);

	const int cnt = p_str.length();
	char c;
	unsigned tmp;
	unsigned result = 0;

	for (int i = 0; i < cnt; ++i) {
		c = p_str[i];

		if (c <= '9') {
			tmp = c - '0';
		} else {
			tmp = c - 'A' + 10;
		}

		result |= (tmp << ((cnt - 1) - i) * 4);
	}

	return result;
}

CL_String Integer::toHex(unsigned p_val)
{
	CL_String result;
	unsigned tmp;
	bool print = false;

	for (int seg = INT_SEG_CNT - 1; seg >= 0; --seg) {
		tmp = (p_val >> (seg * 4)) & 0xF;

		if (!print && tmp > 0) {
			print = true;
		}

		if (print) {
			if (tmp <= 9) {
				result += static_cast<char>('0' + tmp);
			} else {
				result += static_cast<char>('A' + tmp - 10);
			}
		}
	}

	if (!print) {
		result = "0";
	}

	return result;
}

}
