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

#pragma once

#include <ClanLib/core.h>

#include "common.h"
#include "gfx/Drawable.h"

namespace Gfx {

class Label: public Gfx::Drawable {

	public:

		static const int AP_LEFT;
		static const int AP_TOP;
		static const int AP_RIGHT;
		static const int AP_BOTTOM;
		static const int AP_CENTER;

		enum Font {
			F_REGULAR,
			F_BOLD
		};

		Label(
				const CL_Pointf &p_pos,
				const CL_String &p_text,
				Font p_font = F_REGULAR,
				int p_size = 14,
				const CL_Colorf &p_color = CL_Colorf::white
				);

		virtual ~Label();


		virtual void draw(CL_GraphicContext &p_gc);

		virtual void load(CL_GraphicContext &p_gc);


		/** Calculates height of this label in pixels */
		DEPRECATED(float height());

		/** Calculates width of this label in pixels */
		float width();

		CL_Size size(CL_GraphicContext &p_gc);


		void setAttachPoint(int p_attachPoint);

		void setColor(const CL_Colorf &p_color);

		void setPosition(const CL_Pointf &p_pos);

		void setText(const CL_String &p_text);

	private:

		CL_Pointf m_pos;

		int m_attachPoint;

		CL_String m_text;

		const Font m_font;

		const int m_size;

		CL_Colorf m_color;

		CL_Font_System m_clFont;

		CL_FontMetrics m_fontMetrics;

		void calculateAttachPoint(float p_w, float p_h, float &p_x, float &p_y);
};

}

