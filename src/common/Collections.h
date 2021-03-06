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

#include "common.h"

class Collections
{
	public:

		virtual ~Collections();

		template <typename T1>
		static bool contains(const std::vector<T1> &p_v, const T1 &p_el);

		template <typename T1>
		static int index(
				const std::vector<T1> &p_v,
				const T1 &p_el,
				int p_offset = 0
		);

		// two typenames because you can want to remove const ptr
		// from std::vector<ptr> what should be possible
		template <typename T1, typename T2>
		static bool remove(
				std::vector<T1> &p_v,
				const T2 &p_el
		);

	private:

		Collections();
};

template <typename T>
bool Collections::contains(const std::vector<T> &p_v, const T &p_el)
{
	return index(p_v, p_el) != -1;
}

template <typename T1>
int Collections::index(
		const std::vector<T1> &p_v,
		const T1 &p_el,
		int p_offset
)
{
	const int c = static_cast<int> (p_v.size());
	for (int i = p_offset; i < c; ++i) {
		if (p_v[i] == p_el) {
			return i;
		}
	}

	return -1;
}

template <typename T1, typename T2>
bool Collections::remove(
		std::vector<T1> &p_v,
		const T2 &p_el
)
{
	typedef typename std::vector<T1>::iterator TIter;

	for (TIter itor = p_v.begin(); itor != p_v.end(); ++itor) {
		if (*itor == p_el) {
			p_v.erase(itor);
			return true;
		}
	}

	return false;
}
