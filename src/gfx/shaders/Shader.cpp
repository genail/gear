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
const CL_Vec4f QUAD_COLORS[] = { COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE };
const CL_Vec2f QUAD_TEX_COORDS[] = { CL_Vec2f(0,0), CL_Vec2f(0,1), CL_Vec2f(1,1), CL_Vec2f(1,0) };

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
		void destroy();

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
	m_boundRect.left = 0;
	m_boundRect.top = 0;
	m_boundRect.right = Stage::getWidth();
	m_boundRect.bottom = Stage::getHeight();

	// build quad verts
	const int w = Gfx::Stage::getWidth();
	const int h = Gfx::Stage::getHeight();

	m_quadVerts = new CL_Vec2f[4];
	m_quadVerts[0] = CL_Vec2f(0, 0);
	m_quadVerts[1] = CL_Vec2f(0, 1);
	m_quadVerts[2] = CL_Vec2f(1, 1);
	m_quadVerts[3] = CL_Vec2f(1, 0);
}

Shader::~Shader()
{
	// empty
}

ShaderImpl::~ShaderImpl()
{
	delete[] m_quadVerts;
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

	m_program = CL_ProgramObject(p_gc);

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

	// build quad
	m_quad = CL_PrimitivesArray(p_gc);
	m_quad.set_attributes(CL_PRIARR_VERTS, m_quadVerts);
	m_quad.set_attributes(CL_PRIARR_COLORS, QUAD_COLORS);
	m_quad.set_attributes(CL_PRIARR_TEXCOORDS, QUAD_TEX_COORDS);

	m_initialized = true;
}

void Shader::destroy()
{
	m_impl->destroy();
}

void ShaderImpl::destroy()
{
	G_ASSERT(m_initialized);

	m_quad = CL_PrimitivesArray();
	m_frameBuffer = CL_FrameBuffer();

	m_program.detach(m_vertShader);
	m_program.detach(m_fragShader);

	m_vertShader = CL_ShaderObject();
	m_fragShader = CL_ShaderObject();

	m_program = CL_ProgramObject();

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

	// get scaling in count
	const CL_Mat4f &matrix = p_gc.get_modelview();
	const float scaleX = matrix[0];
	const float scaleY = matrix[5];

	p_gc.mult_translate(-m_drawRect.left / scaleX, -m_drawRect.top / scaleY);

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

	// draw texture using shader
	p_gc.set_modelview(CL_Mat4f::identity());
	p_gc.mult_translate(m_drawRect.left, m_drawRect.top);
	p_gc.mult_scale(m_drawRect.get_width(), m_drawRect.get_height());

	p_gc.set_texture(0, m_texture);
	p_gc.set_program_object(m_program);

	p_gc.draw_primitives(cl_quads, 4, m_quad);

	p_gc.reset_program_object();
	p_gc.reset_texture(0);

#if defined(DRAW_WIREFRAME)
	CL_Draw::line(p_gc, 0, 0, 1, 0, CL_Colorf::red);
	CL_Draw::line(p_gc, 1, 0, 1, 1, CL_Colorf::red);
	CL_Draw::line(p_gc, 1, 1, 0, 1, CL_Colorf::red);
	CL_Draw::line(p_gc, 0, 1, 0, 0, CL_Colorf::red);
#endif // DRAW_WIREFRAME

	// reset modelview matrix
	p_gc.pop_modelview();

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
