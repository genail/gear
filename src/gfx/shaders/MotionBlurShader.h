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

#include "gfx/shaders/Shader.h"

class CL_Angle;
class CL_ProgramObject;
class CL_Rect;

namespace Gfx
{

class MotionBlurShaderImpl;
class MotionBlurShader : public Shader
{
	public:
		MotionBlurShader();
		virtual ~MotionBlurShader();

		void setRadius(int p_radius);
		void setAngle(const CL_Angle &p_angle);

	protected:

		virtual const CL_String &getFragmentShaderResourceName();
		virtual const CL_String &getVertexShaderResourceName();

		virtual const CL_Rect &getDrawRect(const CL_Rect &p_requested);

		virtual void setUniforms(CL_ProgramObject &p_program);

	private:
		CL_SharedPtr<MotionBlurShaderImpl> m_impl;
		friend class MotionBlurShaderImpl;
};


}
