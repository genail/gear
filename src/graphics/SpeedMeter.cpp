/*
 * SpeedMeter.cpp
 *
 *  Created on: 2009-10-10
 *      Author: chudy
 */

#include "SpeedMeter.h"

#include "graphics/Stage.h"

SpeedMeter::SpeedMeter() :
	m_speedKMS(0)
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
	/* bg and arrow sizes */
	static const unsigned width = 400, height = 400;

	/* Scale ration */
	static const float scaleRatio = 0.4f;

	/* Real width and height (after scaling) */
	static const unsigned rwidth = width * scaleRatio, rheight = height * scaleRatio;

	/* margin from left and bottom */
	static const unsigned margin = 10;

	/* Start angle */
	const CL_Angle startAngle(-140, cl_degrees);

	/** Angle step per 1 km /s */
	const float angleStep = 1.0f;

	const CL_PolygonRasterizer oldRasteriser = p_gc.get_polygon_rasterizer();

	CL_PolygonRasterizer rasterizer;
	rasterizer.set_antialiased(true);


	p_gc.set_polygon_rasterizer(rasterizer);

	p_gc.push_modelview();
	p_gc.mult_translate(rwidth / 2 + margin, Stage::getHeight() - rheight / 2 - margin);
	p_gc.mult_scale(0.40f, 0.40f);


	// draw the speed meter background
	m_speedControlBg.draw(p_gc, 0, 0);

	// put arrow in the right angle
	CL_Angle arrowRotation(startAngle);
	arrowRotation += CL_Angle(angleStep * m_speedKMS, cl_degrees);

	p_gc.mult_rotate(arrowRotation);

	// draw the arrow
	m_speedControlArrow.draw(p_gc, 0, 0);

	p_gc.pop_modelview();

	p_gc.set_polygon_rasterizer(oldRasteriser);
}

void SpeedMeter::setSpeed(unsigned p_speedKMS)
{
	m_speedKMS = p_speedKMS;
}
