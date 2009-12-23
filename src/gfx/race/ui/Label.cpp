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

const int Label::AP_LEFT   = 1;
const int Label::AP_TOP    = 2;
const int Label::AP_RIGHT  = 4;
const int Label::AP_BOTTOM = 8;
const int Label::AP_CENTER = 16;

const int AP_SIDES = Label::AP_LEFT | Label::AP_RIGHT;
const int AP_TOPS = Label::AP_TOP | Label::AP_BOTTOM;

Label::Label(
		const CL_Pointf &p_pos,
		const CL_String &p_text,
		Font p_font,
		int p_size,
		const CL_Colorf &p_color
		) :
		m_pos(p_pos),
		m_attachPoint(AP_LEFT | AP_BOTTOM),
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

	float ax, ay;
	const CL_Size s = size(p_gc);

	calculateAttachPoint(s.width, s.height, ax, ay);

	m_clFont.draw_text(p_gc, m_pos.x - ax, m_pos.y - ay, m_text, m_color);
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

	//cl_log_event(LOG_DEBUG, "%1", m_fontMetrics.get_average_character_width());
	return m_fontMetrics.get_average_character_width() * m_text.length() / 3;
}

CL_Size Label::size(CL_GraphicContext &p_gc)
{
	assert(isLoaded());
	return m_clFont.get_text_size(p_gc, m_text);
}

void Label::setAttachPoint(int p_attachPoint)
{
	G_ASSERT(!((p_attachPoint & AP_LEFT) && (p_attachPoint & AP_RIGHT)) && "bad value");
	G_ASSERT(!((p_attachPoint & AP_TOP) && (p_attachPoint & AP_BOTTOM)) && "bad value");

	m_attachPoint = p_attachPoint;
}

void Label::calculateAttachPoint(float p_w, float p_h, float &p_x, float &p_y)
{
	if (m_attachPoint & AP_SIDES) {
		if (m_attachPoint & AP_LEFT) {
			p_x = 0.0f;
		} else {
			p_x = p_w;
		}
	}

	if (m_attachPoint & AP_TOPS) {
		if (m_attachPoint & AP_BOTTOM) {
			p_y = 0.0f;
		} else {
			p_y = -p_h;
		}
	}

	if (m_attachPoint & AP_CENTER) {
		if (!(m_attachPoint & AP_TOPS)) {
			p_y = -p_h / 2.0f;
		}

		if (!(m_attachPoint & AP_SIDES)) {
			p_x = p_w / 2.0f;
		}
	}
}

} // namespace
