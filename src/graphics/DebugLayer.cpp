/*
 * DebugLayer.cpp
 *
 *  Created on: 2009-09-11
 *      Author: chudy
 */

#include "DebugLayer.h"

DebugLayer::DebugLayer()
{

}

DebugLayer::~DebugLayer() {
}

void DebugLayer::draw(CL_GraphicContext &p_gc) {

	if (m_font.is_null()) {
		m_font = CL_Font(p_gc, "Tahoma", 14);
		m_fontMetrics = m_font.get_font_metrics(p_gc);
	}

	int x = 5;
	int y = 15;
	const int margin = 0;

	for (std::map<CL_String, CL_String>::iterator itor = m_messages.begin(); itor != m_messages.end(); ++itor) {
		m_font.draw_text(p_gc, x, y, itor->first + ": " + itor->second, CL_Colorf::white);
		y += m_fontMetrics.get_height() + margin;
	}
}
