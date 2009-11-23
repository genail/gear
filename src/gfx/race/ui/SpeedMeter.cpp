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

#include "SpeedMeter.h"

#include "gfx/Stage.h"

namespace Gfx {

SpeedMeter::SpeedMeter() :
	m_speedKMS(0)
{
}

SpeedMeter::~SpeedMeter()
{
}

void SpeedMeter::load(CL_GraphicContext &p_gc)
{
	m_speedControlBg = CL_Sprite(p_gc, "race/speed_meter_bg", Gfx::Stage::getResourceManager());
	m_speedControlArrow = CL_Sprite(p_gc, "race/speed_meter_arrow", Gfx::Stage::getResourceManager());

	m_speedControlBg.set_linear_filter(true);
	m_speedControlArrow.set_linear_filter(true);
}

void SpeedMeter::draw(CL_GraphicContext &p_gc)
{
	/* bg and arrow sizes */
	static const unsigned width = 400, height = 400;

	/* Scale ration */
	static const float scaleRatio = 0.4f;

	/* Real width and height (after scaling) */
	static const unsigned rwidth = width * scaleRatio, rheight = height * scaleRatio;

	/* margin from left and bottom */
	static const unsigned margin = 10;

	/* Start angle */
	const CL_Angle startAngle(-140, cl_degrees);

	/** Angle step per 1 km /s */
	const float angleStep = 1.0f;


	// set the right modelview matrix
	p_gc.push_modelview();

	p_gc.mult_translate(rwidth / 2 + margin, Gfx::Stage::getHeight() - rheight / 2 - margin);
	p_gc.mult_scale(0.40f, 0.40f);


	// draw the speed meter background
	m_speedControlBg.draw(p_gc, 0, 0);


	// put arrow in the right angle
	CL_Angle arrowRotation(startAngle);
	arrowRotation += CL_Angle(angleStep * m_speedKMS, cl_degrees);

	p_gc.mult_rotate(arrowRotation);


	// draw the arrow
	m_speedControlArrow.draw(p_gc, 0, 0);


	// restore the modelview matrix
	p_gc.pop_modelview();
}

void SpeedMeter::setSpeed(unsigned p_speedKMS)
{
	m_speedKMS = p_speedKMS;
}

} // namespace
