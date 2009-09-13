/*
 * Viewport.cpp
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#include "graphics/Stage.h"
#include "race/Viewport.h"

Viewport::Viewport() :
	m_x(0),
	m_y(0),
	m_width(Stage::getWidth()),
	m_height(Stage::getHeight()),
	m_attachPoint(NULL),
	m_scale(1.0f)
{
}

Viewport::~Viewport() {
}

void Viewport::prepareGC(CL_GraphicContext &p_gc) {
	p_gc.push_modelview();

	if (m_attachPoint != NULL) {
		const int stageWidth = Stage::getWidth();
		const int stageHeight = Stage::getHeight();

		m_x = m_attachPoint->x - stageWidth / 2 / m_scale;
		m_y = m_attachPoint->y - stageHeight / 2 / m_scale;
		m_width = stageWidth / m_scale;
		m_height = stageHeight / m_scale;
	}

	const float horizScale = p_gc.get_width() / m_width;
	const float vertScale = p_gc.get_height() / m_height;

	p_gc.mult_scale(horizScale, vertScale);

	p_gc.mult_translate(-m_x, -m_y);


}

void Viewport::finalizeGC(CL_GraphicContext &p_gc) {
	p_gc.pop_modelview();
}
