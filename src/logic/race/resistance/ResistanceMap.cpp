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

#include "ResistanceMap.h"

#include "common.h"
#include "Geometry.h"

namespace RaceResistance {

ResistanceMap::ResistanceMap()
{
}

ResistanceMap::~ResistanceMap()
{
}

void ResistanceMap::addGeometry(const CL_SharedPtr<Geometry> &p_geometry, float p_resistanceValue)
{
	Resistance resistance;

	resistance.m_geometry = p_geometry;
	resistance.m_value = p_resistanceValue;

	m_resistances.push_back(resistance);
}

float ResistanceMap::resistance(const CL_Pointf &p_point)
{
	float result = 0.0f;

	foreach(const Resistance &resistance, m_resistances) {
		const CL_SharedPtr<Geometry> &geom = resistance.m_geometry;
		const CL_Rectf &bounds = geom->getBounds();

		if (bounds.contains(p_point) && geom->contains(p_point)) {
			result = resistance.m_value;
		}
	}

	return result;
}

} // namespace
