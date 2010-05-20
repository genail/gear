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

#include <boost/utility.hpp>
#include "clanlib/core/system.h"

class CL_GraphicContext;
class CL_ProgramObject;
class CL_Rect;

namespace Gfx
{

class ShaderImpl;
class Shader : public boost::noncopyable
{
	public:
		Shader();
		virtual ~Shader();

		void initialize(CL_GraphicContext &p_gc);
		void destroy();

		void begin(CL_GraphicContext &p_gc);
		void end(CL_GraphicContext &p_gc);

		void setBoundRect(const CL_Rect &p_boundRect);

	protected:

		virtual const CL_String &getFragmentShaderResourceName() = 0;
		virtual const CL_String &getVertexShaderResourceName() = 0;

		virtual const CL_Rect &getDrawRect(const CL_Rect &p_requested);

		virtual void setUniforms(CL_ProgramObject &p_program);

	private:
		CL_SharedPtr<ShaderImpl> m_impl;
		friend class ShaderImpl;
};

}

