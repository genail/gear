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

#include "Shader.h"

#include "clanlib/core/math.h"
#include "clanlib/core/text.h"
#include "clanlib/display/2d.h"
#include "clanlib/display/render.h"

#include "common.h"
#include "common/gassert.h"
#include "gfx/Stage.h"

namespace Gfx
{

const CL_Vec4f COLOR_WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const CL_Vec4f QUAD_COLORS[] = {
	COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE
	};

class ShaderImpl
{
	public:
		Shader *m_parent;

		bool m_initialized;
		bool m_began;

		CL_Rect m_boundRect;
		CL_Rect m_drawRect;

		CL_ShaderObject m_vertShader;
		CL_ShaderObject m_fragShader;
		CL_ProgramObject m_program;

		CL_FrameBuffer m_frameBuffer;
		CL_Texture m_texture;

		CL_Vec2f *m_quadVerts;
		CL_PrimitivesArray m_quad;


		ShaderImpl(Shader *p_parent);
		~ShaderImpl();

		void initialize(CL_GraphicContext &p_gc);
		void destroy(CL_GraphicContext &p_gc);

		void begin(CL_GraphicContext &p_gc);
		void end(CL_GraphicContext &p_gc);
};

// --------------------------------------------------------

Shader::Shader() :
		m_impl(new ShaderImpl(this))
{
	// empty
}

ShaderImpl::ShaderImpl(Shader *p_parent) :
		m_parent(p_parent),
		m_initialized(false),
		m_began(false)
{
	// empty
}

Shader::~Shader()
{
	// empty
}

ShaderImpl::~ShaderImpl()
{
	// empty
}

// --------------------------------------------------------

void Shader::initialize(CL_GraphicContext &p_gc)
{
	m_impl->initialize(p_gc);
}

void ShaderImpl::initialize(CL_GraphicContext &p_gc)
{
	G_ASSERT(!m_initialized);

	CL_ResourceManager *resMgr = Gfx::Stage::getResourceManager();

	// load vertex shader
	const CL_String &vertShaderName = m_parent->getVertexShaderResourceName();
	m_vertShader = CL_ShaderObject::load(p_gc, vertShaderName, resMgr);
	m_program.attach(m_vertShader);

	// load fragment shader
	const CL_String &fragShaderName = m_parent->getFragmentShaderResourceName();
	m_fragShader = CL_ShaderObject::load(p_gc, fragShaderName, resMgr);
	m_program.attach(m_fragShader);

	// bind common attributes
	m_program.bind_attribute_location(CL_PRIARR_VERTS, "Position");
	m_program.bind_attribute_location(CL_PRIARR_COLORS, "Color0");
	m_program.bind_attribute_location(CL_PRIARR_TEXCOORDS, "TexCoord0");

	// link the program
	if (!m_program.link()) {
		m_program.detach(m_vertShader);
		m_program.detach(m_fragShader);

		throw CL_Exception(
				cl_format(
						"error linking shader %1, %2: %3",
						vertShaderName, fragShaderName,
						m_program.get_info_log()
				)
		);
	}

	// build frame buffer
	m_frameBuffer = CL_FrameBuffer(p_gc);

	m_initialized = true;
}

void Shader::destroy(CL_GraphicContext &p_gc)
{
	m_impl->destroy(p_gc);
}

void ShaderImpl::destroy(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_initialized);

	m_frameBuffer = CL_FrameBuffer();

	m_program.detach(m_vertShader);
	m_program.detach(m_fragShader);

	m_initialized = false;
}

void Shader::begin(CL_GraphicContext &p_gc)
{
	m_impl->begin(p_gc);
}

void ShaderImpl::begin(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_initialized);
	G_ASSERT(!m_began);

	// new texture
	m_drawRect = m_parent->getDrawRect(m_boundRect);
	m_texture = CL_Texture(p_gc, m_drawRect.get_width(), m_drawRect.get_height());

	// attach frame buffer
	m_frameBuffer.attach_color_buffer(0, m_texture);
	p_gc.set_frame_buffer(m_frameBuffer);

	// clear to transparent
	p_gc.clear(CL_Colorf::transparent);

	// set proper matrix
	p_gc.push_modelview();
	p_gc.mult_translate(-m_drawRect.left, -m_drawRect.top);

	m_began = true;
}

void Shader::end(CL_GraphicContext &p_gc)
{
	m_impl->end(p_gc);
}

void ShaderImpl::end(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_initialized);
	G_ASSERT(m_began);

	// detach frame buffer
	p_gc.reset_frame_buffer();
	m_frameBuffer.detach_color_buffer(0, m_texture);

	// prepare shader
	m_program.set_uniform1i("tex", 0);
	m_program.set_uniform1i("textureWidth", m_drawRect.get_width());
	m_program.set_uniform1i("textureHeight", m_drawRect.get_height());

	m_parent->setUniforms(m_program);

	// draw witch shader
	p_gc.set_program_object(m_program);

	m_began = false;
}

void Shader::setBoundRect(const CL_Rect &p_boundRect)
{
	m_impl->m_boundRect = p_boundRect;
}

const CL_Rect &Shader::getDrawRect(const CL_Rect &p_requested)
{
	return p_requested;
}

void Shader::setUniforms(CL_ProgramObject &p_program)
{
	// empty
}

}
