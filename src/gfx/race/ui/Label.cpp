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

#include "Label.h"

#include "common/gassert.h"

namespace Gfx {

const int Label::AP_LEFT   = 1;
const int Label::AP_TOP    = 2;
const int Label::AP_RIGHT  = 4;
const int Label::AP_BOTTOM = 8;
const int Label::AP_CENTER = 16;

const int AP_SIDES = Label::AP_LEFT | Label::AP_RIGHT;
const int AP_TOPS = Label::AP_TOP | Label::AP_BOTTOM;

class LabelImpl
{
	public:

		Label *m_parent;

		CL_Pointf m_pos;
		int m_attachPoint;
		CL_String m_text;
		const Label::Font m_font;
		const int m_size;
		CL_Colorf m_color;

		bool m_shadowVisible;
		CL_Colorf m_shadowColor;
		CL_Vec2f m_shadowOffset;

		CL_Font *m_clFont;
		CL_FontMetrics m_fontMetrics;


		LabelImpl(
				Label *p_parent,
				const CL_Pointf &p_pos,
				const CL_String &p_text,
				Label::Font p_font,
				int p_size,
				const CL_Colorf &p_color);
		~LabelImpl();

		void load(CL_GraphicContext &p_gc);
		void draw(CL_GraphicContext &p_gc);
		void drawShadow(CL_GraphicContext &p_gc, const CL_Pointf &p_pos);

		void calculateAttachPoint(float p_w, float p_h, float &p_x, float &p_y);
};

Label::Label(
	const CL_Pointf &p_pos,
	const CL_String &p_text,
	Font p_font,
	int p_size,
	const CL_Colorf &p_color) :
		m_impl(new LabelImpl(this, p_pos, p_text, p_font, p_size, p_color))
{
	// empty
}

LabelImpl::LabelImpl(
	Label *p_parent,
	const CL_Pointf &p_pos,
	const CL_String &p_text,
	Label::Font p_font,
	int p_size,
	const CL_Colorf &p_color) :
		m_parent(p_parent),
		m_pos(p_pos),
		m_attachPoint(Label::AP_LEFT | Label::AP_BOTTOM),
		m_text(p_text),
		m_font(p_font),
		m_size(p_size),
		m_color(p_color),
		m_shadowVisible(false),
		m_shadowColor(CL_Colorf::black),
		m_shadowOffset(1.5f, 1.5f),
		m_clFont(NULL)
{
	// empty
}

Label::~Label()
{
	// empty
}

LabelImpl::~LabelImpl()
{
	if (m_clFont) {
		delete m_clFont;
	}
}

void Label::load(CL_GraphicContext &p_gc)
{
	Drawable::load(p_gc);
	m_impl->load(p_gc);
}

void LabelImpl::load(CL_GraphicContext &p_gc)
{
	CL_FontDescription desc;
	desc.set_height(m_size);

	if (m_font == Label::F_REGULAR || m_font == Label::F_BOLD) {
		desc.set_typeface_name("tahoma");

		if (m_font == Label::F_BOLD) {
			desc.set_weight(100000);
		}

		m_clFont = new CL_Font_System(p_gc, desc);
	} else {
		desc.set_typeface_name("resources/pixel.ttf");
		m_clFont = new CL_Font_Freetype(p_gc, desc);
	}

	// remember metrics
	m_fontMetrics = m_clFont->get_font_metrics(p_gc);
}

void Label::draw(CL_GraphicContext &p_gc)
{
	m_impl->draw(p_gc);
}

void LabelImpl::draw(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_parent->isLoaded());

	float ax, ay;
	const CL_Size s = m_parent->size(p_gc);

	calculateAttachPoint(s.width, s.height, ax, ay);

	CL_Pointf position;
	position.x = m_pos.x - ax;
	position.y = m_pos.y - ay - m_fontMetrics.get_descent();

	if (m_shadowVisible) {
		drawShadow(p_gc, position);
	}

	m_clFont->draw_text(p_gc, position.x, position.y, m_text, m_color);

#if !defined(NDEBUG) && defined(DRAW_LABEL_BOUNDS)
	// draw label frame debug code
	CL_Pen newPen;
	newPen.set_line_width(1.0f);

	const CL_Pen oldPen = p_gc.get_pen();
	p_gc.set_pen(newPen);

	const float y2 = y + m_fontMetrics.get_descent();
	CL_Draw::box(p_gc, x, y2 - s.height, x + s.width, y2, CL_Colorf::red);

	p_gc.set_pen(oldPen);
#endif // !NDEBUG && DRAW_LABEL_BOUNDS
}

void LabelImpl::drawShadow(CL_GraphicContext &p_gc, const CL_Pointf &p_pos)
{
	CL_Pointf position;
	position.x = p_pos.x + m_shadowOffset.x;
	position.y = p_pos.y + m_shadowOffset.y;
	m_clFont->draw_text(p_gc, position.x, position.y, m_text, m_shadowColor);
}

void Label::setColor(const CL_Colorf &p_color)
{
	m_impl->m_color = p_color;
}

void Label::setPosition(const CL_Pointf &p_pos)
{
	m_impl->m_pos = p_pos;
}

void Label::setText(const CL_String &p_text)
{
	m_impl->m_text = p_text;
}

float Label::height()
{
	G_ASSERT(isLoaded());
	return m_impl->m_fontMetrics.get_height();
}

CL_Size Label::size(CL_GraphicContext &p_gc)
{
	G_ASSERT(isLoaded());
	return m_impl->m_clFont->get_text_size(p_gc, m_impl->m_text);
}

void Label::setAttachPoint(int p_attachPoint)
{
	G_ASSERT(!((p_attachPoint & AP_LEFT) && (p_attachPoint & AP_RIGHT)) && "bad value");
	G_ASSERT(!((p_attachPoint & AP_TOP) && (p_attachPoint & AP_BOTTOM)) && "bad value");

	m_impl->m_attachPoint = p_attachPoint;
}

void LabelImpl::calculateAttachPoint(float p_w, float p_h, float &p_x, float &p_y)
{
	if (m_attachPoint & AP_SIDES) {
		if (m_attachPoint & Label::AP_LEFT) {
			p_x = 0.0f;
		} else {
			p_x = p_w;
		}
	}

	if (m_attachPoint & AP_TOPS) {
		if (m_attachPoint & Label::AP_BOTTOM) {
			p_y = 0.0f;
		} else {
			p_y = -p_h;
		}
	}

	if (m_attachPoint & Label::AP_CENTER) {
		if (!(m_attachPoint & AP_TOPS)) {
			p_y = -p_h / 2.0f;
		}

		if (!(m_attachPoint & AP_SIDES)) {
			p_x = p_w / 2.0f;
		}
	}
}

void Label::setShadowVisible(bool p_visible)
{
	m_impl->m_shadowVisible = p_visible;
}

void Label::setShadowColor(const CL_Colorf &p_shadowColor)
{
	m_impl->m_shadowColor = p_shadowColor;
}

void Label::setShadowOffset(const CL_Vec2f &p_shadowOffset)
{
	m_impl->m_shadowOffset = p_shadowOffset;
}

} // namespace
