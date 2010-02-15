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

#include "EditorPoint.h"

using namespace Race;
using namespace Gfx;

const int POINT_WIDTH = 5;
const int PAINT_LINE_WIDTH = 2;

const float DEFAULT_RADIUS = 60.0f;
const float DEFAULT_SHIFT = 0.0f;

namespace Editor
{
	class EditorPointImpl
	{
	public:
		EditorPointImpl() : 
			m_selectedIndex(-1),
			m_lightIndex(-1),
			m_selectedPointColor(CL_Colorf::yellow),
			m_pointColor(CL_Colorf::blue),
			m_radiusLineColor(CL_Colorf::white),
			m_shiftLineColor(CL_Colorf::green),
			m_selectedPointFrameColor(CL_Colorf::red),
			m_shiftPointColor(CL_Colorf::red),
			m_minAndMaxShiftRectColor(CL_Colorf::red),
			m_state(None)
		{

		}

		// help variables

		CL_Pointf m_minShiftPoint;

		CL_Pointf m_maxShiftPoint;

		int m_selectedIndex;

		int m_lightIndex;

		CL_Colorf m_selectedPointFrameColor;

		CL_Colorf m_selectedPointColor;

		CL_Colorf m_shiftPointColor;

		CL_Colorf m_pointColor;

		CL_Colorf m_radiusLineColor;

		CL_Colorf m_shiftLineColor;

		CL_Colorf m_minAndMaxShiftRectColor;

		// input

		enum State
		{
			None,
			Point
		};

		State m_state;
	};

	EditorPoint::EditorPoint(Race::Level& p_raceLevel, Gfx::Level& p_gfxLevel, Race::Track& p_track, Gfx::Viewport& p_viewport) : 
		m_impl(new EditorPointImpl()),
		EditorBase(p_raceLevel, p_gfxLevel, p_track, p_viewport)
	{
		setDefaultPoints();
	}

	EditorPoint::~EditorPoint()
	{

	}

	void EditorPoint::setDefaultPoints()
	{
		m_track.clear();

		m_track.addPoint(CL_Pointf(150.0f, 150.0f), DEFAULT_RADIUS, DEFAULT_SHIFT);
		m_track.addPoint(CL_Pointf(600.0f, 120.0f), DEFAULT_RADIUS, DEFAULT_SHIFT);
		m_track.addPoint(CL_Pointf(600.0f, 400.0f), DEFAULT_RADIUS, DEFAULT_SHIFT);
		m_track.addPoint(CL_Pointf(200.0f, 450.0f), DEFAULT_RADIUS, DEFAULT_SHIFT);

		m_gfxLevel.getTrackTriangulator().triangulate(m_track);
	}

	void EditorPoint::drawPoints(CL_GraphicContext &p_gc)
	{
		for (int i = 0; i < m_track.getPointCount(); ++i)
		{
			bool isSelected = (i == m_impl->m_selectedIndex) ? true : false;
			bool isLight = (i == m_impl->m_lightIndex) ? true : false;
			drawPoint(i, isSelected, isLight, p_gc);
		}
	}

	void EditorPoint::drawPoint(int p_index, bool &p_isSelected, bool &p_isLight, CL_GraphicContext &p_gc)
	{
		CL_Colorf color = (p_isSelected || p_isLight) ? m_impl->m_selectedPointColor : m_impl->m_pointColor;

		if (p_isSelected)
		{
			if (isFirstKey(CL_KEY_SHIFT) && m_impl->m_selectedIndex != -1)
			{
				CL_Pen pen;

				pen.set_line_width(PAINT_LINE_WIDTH);
				p_gc.set_pen(pen);

				int x1, y1, x2, y2;
				getShiftRect(p_index, &x1, &y1, &x2, &y2);

				CL_Draw::line(p_gc, x1, y1, x2, y2, m_impl->m_shiftLineColor);

				pen.set_line_width(1.0f);
				p_gc.set_pen(pen);

				CL_Draw::fill(p_gc, getPointRect(m_impl->m_minShiftPoint), m_impl->m_minAndMaxShiftRectColor);
				CL_Draw::fill(p_gc, getPointRect(m_impl->m_maxShiftPoint), m_impl->m_minAndMaxShiftRectColor);
			}
		}

		CL_Draw::fill(p_gc, getPointRect(p_index), color);

		if (p_isSelected)
		{
			CL_Draw::box(p_gc, getPointRect(p_index), m_impl->m_selectedPointFrameColor);
		}
	}

	void EditorPoint::findPointAt(const CL_Pointf &p_pos, int &p_index)
	{
		if (getKeys().size() == 0)
		{
			p_index = -1;
			m_impl->m_state = EditorPointImpl::None;

			CL_Rect rect = getPointRect(p_pos);

			for (int i = 0; i < m_track.getPointCount(); ++i)
			{
				if (rect.contains(m_track.getPoint(i).getPosition()))
				{
					m_impl->m_state = EditorPointImpl::Point;
					p_index = i;

					return;
				}
			}
		}
	}

	void EditorPoint::setToPerpendicular(CL_Vec2f& p_vector2, bool p_isInvert)
	{
		if ((p_vector2.x > 0 && p_vector2.y > 0) || (p_vector2.x < 0 && p_vector2.y < 0))
			p_vector2.x = -p_vector2.x;
		else 
			p_vector2.y = -p_vector2.y;

		if (p_isInvert)
		{
			p_vector2.x = -p_vector2.x;
			p_vector2.y = -p_vector2.y;
		}
	}

	void EditorPoint::setMinAndMaxShiftPoint(int p_index)
	{
		if (p_index >= 0 && p_index < m_track.getPointCount())
		{
			const TrackPoint& trackPoint = m_track.getPoint(p_index);

			float minShift = -1.0f - trackPoint.getShift();
			float maxShift = 1.0f - trackPoint.getShift();
			float radius = trackPoint.getRadius();

			CL_Vec2f guide = m_gfxLevel.getTrackTriangulator().getGuide(p_index);

			guide /= guide.length();

			CL_Vec2f minShiftGuide = guide;
			setToPerpendicular(minShiftGuide, true);
			minShiftGuide *= radius * minShift;

			CL_Vec2f maxShiftGuide = guide;
			setToPerpendicular(maxShiftGuide, true);
			maxShiftGuide *= radius * maxShift;

			const CL_Pointf& pos = trackPoint.getPosition();

			m_impl->m_minShiftPoint = pos + minShiftGuide;
			m_impl->m_maxShiftPoint = pos + maxShiftGuide;
		}
	}

	void EditorPoint::getShiftRect(int p_index, int* x1, int* y1, int* x2, int* y2)
	{
		const TrackPoint& p_trackPoint = m_track.getPoint(p_index);

		CL_Vec2f guide = m_gfxLevel.getTrackTriangulator().getGuide(p_index);

		float guideLength = guide.length();
		guide *= p_trackPoint.getShift() * p_trackPoint.getRadius();
		guide /= guideLength;

		setToPerpendicular(guide, false);

		const CL_Pointf& pos = p_trackPoint.getPosition();

		*x1 = pos.x;
		*y1 = pos.y;
		*x2 = pos.x + guide.x;
		*y2 = pos.y + guide.y;
	}

	CL_Rect EditorPoint::getPointRect(int p_index)
	{
		TrackPoint p_trackPoint = m_track.getPoint(p_index);

		CL_Rect rect;

		float pointWidth = POINT_WIDTH / m_viewport.getScale();

		rect.left = p_trackPoint.getPosition().x - pointWidth;
		rect.top = p_trackPoint.getPosition().y - pointWidth;
		rect.right = p_trackPoint.getPosition().x + pointWidth;
		rect.bottom = p_trackPoint.getPosition().y + pointWidth;

		return rect;
	}

	CL_Rect EditorPoint::getPointRect(const CL_Pointf &p_point)
	{
		CL_Rect rect;

		float pointWidth = POINT_WIDTH / m_viewport.getScale();

		rect.left = p_point.x - pointWidth;
		rect.top = p_point.y - pointWidth;
		rect.right = p_point.x + pointWidth;
		rect.bottom = p_point.y + pointWidth;

		return rect;
	}

	void EditorPoint::setRadius(int p_index, int p_set)
	{
		if (p_index >= 0 && p_index < m_track.getPointCount())
		{
			TrackPoint& trackPoint = m_track.getPoint(p_index);

			float newRadius = trackPoint.getRadius() + p_set;

			if (newRadius > 40.0f)
				trackPoint.setRadius(newRadius);
		}
	}

	void EditorPoint::triangulate(int p_index)
	{
		if (p_index >= 0 && p_index < m_track.getPointCount())
		{
			int segment = p_index - 2;
			if (segment < 0)
				segment = m_track.getPointCount() - abs(segment);

			for (int i = 0; i < 4; ++i)
			{
				if (segment > m_track.getPointCount() - 1)
					segment = 0;

				m_gfxLevel.getTrackTriangulator().triangulate(m_track, segment);

				++segment;
			}
		}
	}

	void EditorPoint::onDraw(CL_GraphicContext &p_gc)
	{
		drawPoints(p_gc);
	}

	void EditorPoint::onUpdate(unsigned int p_timeElapsed)
	{
		setMinAndMaxShiftPoint(m_impl->m_selectedIndex);

		if (m_impl->m_selectedIndex >= 0 && (m_ctrl || m_alt))
		{
			int set = 0;
			bool isSet = false;

			if (isSecondKey(CL_KEY_ADD))
			{
				set = 1;
				isSet = true;
			}
			else if (isSecondKey(CL_KEY_SUBTRACT))
			{
				set = -1;
				isSet = true;
			}

			if (isSet)
			{
				if (m_alt)
					set *= 2;

				setRadius(m_impl->m_selectedIndex, set);
				triangulate(m_impl->m_selectedIndex);
			}
		}
	}

	void EditorPoint::onHandleInput()
	{	
		if (isFirstKey(CL_MOUSE_LEFT))
		{
			if (m_impl->m_state == EditorPointImpl::Point)
			{
				m_impl->m_selectedIndex = m_impl->m_lightIndex;
			}
			else
			{
				m_impl->m_selectedIndex = m_impl->m_lightIndex = -1;
			}
		}
		else if (isFirstKey(CL_KEY_DELETE) || isFirstKey(CL_KEY_D))
		{
			if (m_impl->m_selectedIndex >= 0 && m_track.getPointCount() > 3)
			{
				m_track.removePoint(m_impl->m_selectedIndex);

				m_impl->m_selectedIndex -= 1;
				if (m_impl->m_selectedIndex < 0)
					m_impl->m_selectedIndex = m_track.getPointCount() - 1;

				triangulate(m_impl->m_selectedIndex);
			}
		}
		else if (isFirstKey(CL_MOUSE_MIDDLE) || (isFirstKey(CL_KEY_CONTROL) && isSecondKey(CL_MOUSE_LEFT)))
		{
			int index = 0;

			if (m_impl->m_selectedIndex >= 0 && m_impl->m_selectedIndex < m_track.getPointCount())
				index = ++m_impl->m_selectedIndex;
			else
				index = m_impl->m_selectedIndex = m_track.getPointCount();

			m_track.addPoint(m_mousePos, DEFAULT_RADIUS, DEFAULT_SHIFT, index);

			triangulate(m_impl->m_selectedIndex);
		}
	}

	void EditorPoint::onMouseMoved()
	{
		findPointAt(m_mousePos, m_impl->m_lightIndex);
		
		if (m_impl->m_selectedIndex != -1)
		{
			TrackPoint trackPoint = m_track.getPoint(m_impl->m_selectedIndex);

			if (isFirstKey(CL_KEY_SHIFT))
			{
				float lastMaxShiftDeltaLength = m_impl->m_maxShiftPoint.distance(m_lastMousePos);
				float lastMinShiftDeltaLength = m_impl->m_minShiftPoint.distance(m_lastMousePos);

				float nowMaxShiftDeltaLength = m_impl->m_maxShiftPoint.distance(m_mousePos);
				float nowMinShiftDeltaLength = m_impl->m_minShiftPoint.distance(m_mousePos);

				float maxShiftDeltaLength = nowMaxShiftDeltaLength - lastMaxShiftDeltaLength;
				float minShiftDeltaLength = nowMinShiftDeltaLength - lastMinShiftDeltaLength;

				double newShift = trackPoint.getShift();

				if (maxShiftDeltaLength > 0.0f && minShiftDeltaLength < 0.0f)
					newShift += maxShiftDeltaLength / 100.0f;
				else if (maxShiftDeltaLength < 0.0f && minShiftDeltaLength > 0.0f)
					newShift -= minShiftDeltaLength / 100.0f;
				else if (nowMaxShiftDeltaLength > nowMinShiftDeltaLength)
					newShift += maxShiftDeltaLength / 100.0f;
				else
					newShift -= maxShiftDeltaLength / 100.0f;

				if (newShift >= -1.0f && newShift <= 1.0f)
					trackPoint.setShift(newShift);
			}
			else if (isFirstKey(CL_MOUSE_LEFT))
			{
				trackPoint.setPosition(trackPoint.getPosition() + m_deltaMousePos);
			}

			triangulate(m_impl->m_selectedIndex);
		}
	}

	void EditorPoint::onMouseScrolled(bool p_up)
	{
		if (m_impl->m_selectedIndex >= 0 && (m_ctrl || m_alt))
		{
			int set = p_up ? 1 : -1;

			if (m_alt)
				set *= 5;

			setRadius(m_impl->m_selectedIndex, set);
			triangulate(m_impl->m_selectedIndex);
		}
	}
}
