/*
 * SpeedMeter.cpp
 *
 *  Created on: 2009-10-10
 *      Author: chudy
 */

#include "SpeedMeter.h"

#include "graphics/Stage.h"

SpeedMeter::SpeedMeter()
{
}

SpeedMeter::~SpeedMeter()
{
}

void SpeedMeter::load(CL_GraphicContext &p_gc)
{
	m_speedControlBg = CL_Sprite(p_gc, "race/speed_meter_bg", Stage::getResourceManager());
	m_speedControlArrow = CL_Sprite(p_gc, "race/speed_meter_arrow", Stage::getResourceManager());

//	m_speedControlBg.set_linear_filter(true);
}

void SpeedMeter::draw(CL_GraphicContext &p_gc)
{

	const CL_PolygonRasterizer oldRasteriser = p_gc.get_polygon_rasterizer();

	CL_PolygonRasterizer rasterizer;
	rasterizer.set_antialiased(true);


	p_gc.set_polygon_rasterizer(rasterizer);

	p_gc.push_modelview();
	p_gc.mult_translate(100 - 20, 500 + 20);
	p_gc.mult_scale(0.20f, 0.20f);

	m_speedControlBg.draw(p_gc, 0, 0);

	p_gc.mult_rotate(CL_Angle(-100, cl_degrees));
	m_speedControlArrow.draw(p_gc, 0, 0);

	p_gc.pop_modelview();

	p_gc.set_polygon_rasterizer(oldRasteriser);
}
