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

#include "Header.h"

#include <ClanLib/display.h>


namespace Gfx
{

class HeaderImpl
{
	public:
		Header *m_parent;

		CL_String m_text;
		int m_textSize;
		CL_Colorf m_textColor;

		bool m_dirty;
		CL_SharedPtr<CL_Font> m_font;


		HeaderImpl(Header *p_parent);
		~HeaderImpl();

		void onRender(CL_GraphicContext &gc, const CL_Rect &update_rect);
};

// --------------------------------------------------------

Header::Header(CL_GUIComponent *p_parent) :
		CL_GUIComponent(p_parent),
		m_impl(new HeaderImpl(this))
{
	// empty
}

HeaderImpl::HeaderImpl(Header *p_parent) :
		m_parent(p_parent),
		m_textSize(16),
		m_textColor(CL_Colorf::black),
		m_dirty(true)
{
	m_parent->func_render().set(this, &HeaderImpl::onRender);
}

Header::~Header()
{
	// empty
}

HeaderImpl::~HeaderImpl()
{
	// empty
}

// --------------------------------------------------------

void HeaderImpl::onRender(CL_GraphicContext &p_gc, const CL_Rect &p_updateRect)
{
	if (m_dirty) {
		CL_FontDescription fontDesc;
		fontDesc.set_height(m_textSize);
		fontDesc.set_typeface_name("tahoma");

		m_font = CL_SharedPtr<CL_Font>(new CL_Font_System(p_gc, fontDesc));
		m_dirty = false;
	}

	const CL_Rect geometry = m_parent->get_geometry();
	m_font->draw_text(p_gc, geometry.left, geometry.top, m_text, m_textColor);
}

void Header::setText(const CL_String &p_text)
{
	m_impl->m_text = p_text;
	m_impl->m_dirty = true;

	request_repaint();
}

void Header::setTextSize(int p_textSize)
{
	m_impl->m_textSize = p_textSize;
	m_impl->m_dirty = true;

	request_repaint();
}

void Header::setTextColor(const CL_Colorf &p_color)
{
	m_impl->m_textColor = p_color;
	m_impl->m_dirty = true;

	request_repaint();
}

} // namespace
