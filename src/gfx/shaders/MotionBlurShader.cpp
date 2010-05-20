/*
 * Copyright (c) 2009-2010 The Gear Team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Gear nor the
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

#include "MotionBlurShader.h"

#include "clanlib/core/math.h"
#include "clanlib/display/render.h"

const CL_String SHADER_FRAG_NAME = "shaders/motionblur.frag";
const CL_String SHADER_VERT_NAME = "shaders/motionblur.vert";

namespace Gfx
{

class MotionBlurShaderImpl
{
	public:
		MotionBlurShader *m_api;

		int m_radius;
		CL_Angle m_angle;

		CL_Rect m_drawRect;


		MotionBlurShaderImpl(MotionBlurShader *p_api);
		~MotionBlurShaderImpl();

		const CL_Rect &getDrawRect(const CL_Rect &p_requested);
		void setUniforms(CL_ProgramObject &p_program);
};

// --------------------------------------------------------

MotionBlurShader::MotionBlurShader() :
	m_impl(new MotionBlurShaderImpl(this))
{
	// empty
}

MotionBlurShaderImpl::MotionBlurShaderImpl(MotionBlurShader *p_api) :
	m_api(p_api)
{
	// empty
}

MotionBlurShader::~MotionBlurShader()
{
	// empty
}

MotionBlurShaderImpl::~MotionBlurShaderImpl()
{
	// empty
}

// --------------------------------------------------------

const CL_String &MotionBlurShader::getFragmentShaderResourceName()
{
	return SHADER_FRAG_NAME;
}

const CL_String &MotionBlurShader::getVertexShaderResourceName()
{
	return SHADER_VERT_NAME;
}

const CL_Rect &MotionBlurShader::getDrawRect(const CL_Rect &p_requested)
{
	return m_impl->getDrawRect(p_requested);
}

const CL_Rect &MotionBlurShaderImpl::getDrawRect(const CL_Rect &p_requested)
{
	m_drawRect = p_requested;
	m_drawRect.expand(m_radius);

	return m_drawRect;
}

void MotionBlurShader::setUniforms(CL_ProgramObject &p_program)
{
	m_impl->setUniforms(p_program);
}

void MotionBlurShaderImpl::setUniforms(CL_ProgramObject &p_program)
{
	p_program.set_uniform1i("radius", m_radius);
	p_program.set_uniform1f("angle", m_angle.to_radians());
}

void MotionBlurShader::setRadius(int p_radius)
{
	m_impl->m_radius = p_radius;
}

void MotionBlurShader::setAngle(const CL_Angle &p_angle)
{
	m_impl->m_angle = p_angle;
}

} // namespace
