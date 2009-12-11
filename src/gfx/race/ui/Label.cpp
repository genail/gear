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

#include "Label.h"

#include <assert.h>

namespace Gfx {

Label::Label(
		const CL_Pointf &p_pos,
		const CL_String &p_text,
		Font p_font,
		int p_size,
		const CL_Colorf &p_color
		) :
		m_pos(p_pos),
		m_text(p_text),
		m_font(p_font),
		m_size(p_size),
		m_color(p_color)
{

}

Label::~Label()
{
}

void Label::draw(CL_GraphicContext &p_gc)
{
	assert(isLoaded());
	m_clFont.draw_text(p_gc, m_pos.x, m_pos.y, m_text, m_color);
}

void Label::load(CL_GraphicContext &p_gc)
{
	Drawable::load(p_gc);

	CL_FontDescription desc;
	desc.set_typeface_name("tahoma");
	desc.set_height(m_size);

	if (m_font == F_BOLD) {
		desc.set_weight(100000);
	}

	m_clFont = CL_Font_System(p_gc, desc);

	// remember metrics
	m_fontMetrics = m_clFont.get_font_metrics(p_gc);
}

void Label::setColor(const CL_Colorf &p_color)
{
	m_color = p_color;
}

void Label::setPosition(const CL_Pointf &p_pos)
{
	m_pos = p_pos;
}

void Label::setText(const CL_String &p_text)
{
	m_text = p_text;
}

float Label::height()
{
	assert(isLoaded());
	return m_fontMetrics.get_height();
}

float Label::width()
{
	assert(isLoaded());
	return m_fontMetrics.get_average_character_width() * m_text.length();
}

} // namespace
