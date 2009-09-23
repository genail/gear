/*
 * RaceUI.cpp
 *
 *  Created on: 2009-09-23
 *      Author: chudy
 */

#include "RaceUI.h"

#include "graphics/Stage.h"

RaceUI::RaceUI() :
	m_countDownStart(0)
{
}

RaceUI::~RaceUI() {
}

void RaceUI::displayCountdown() {
	m_countDownStart = CL_System::get_time();
}

void RaceUI::draw(CL_GraphicContext &p_gc) {

	if (m_countDownStart != 0) {

		static const unsigned LABEL_3_END = 1000 - 1;
		static const unsigned LABEL_2_END = 2000 - 1;
		static const unsigned LABEL_1_END = 3000 - 1;
		static const unsigned LABEL_START_END = 4500 - 1;

		// display the countdown
		const unsigned delta = CL_System::get_time() - m_countDownStart;

		if (delta <= LABEL_3_END) {
			drawCountdownLabel(p_gc, "3");
		} else if (delta <= LABEL_2_END) {
			drawCountdownLabel(p_gc, "2");
		} else if (delta <= LABEL_1_END) {
			drawCountdownLabel(p_gc, "1");
		} else if (delta <= LABEL_START_END) {
			drawCountdownLabel(p_gc, "START");
		} else {
			m_countDownStart = 0;
		}
	}
}

void RaceUI::drawCountdownLabel(CL_GraphicContext &p_gc, const CL_String &p_label) {
	const CL_Size size = m_countdownFont.get_text_size(p_gc, p_label);
	m_countdownFont.draw_text(
			p_gc,
			Stage::getWidth() / 2 - size.width / 2,
			Stage::getHeight() / 2 - size.height / 2,
			p_label,
			CL_Colorf::red
	);
}

void RaceUI::load(CL_GraphicContext &p_gc) {

	// load countdown font
	CL_FontDescription fontDesc;

	fontDesc.set_typeface_name("resources/DejaVuSansCondensed-BoldOblique.ttf");
	fontDesc.set_height(92);

	m_countdownFont = CL_Font_Freetype(p_gc, fontDesc);

}
