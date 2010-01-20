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

#include "EditorPoint.h"

using namespace Race;
using namespace Gfx;

const int POINT_WIDTH = 5;
const int PAINT_LINE_WIDTH = 2;
const int DISCOVER_LINE_WIDTH = 12;

namespace Editor
{
	class EditorPointImpl
	{
	public:
		EditorPointImpl(Track& p_track, Gfx::Level& p_gfxLevel) : 
			m_track(p_track),
			m_gfxLevel(p_gfxLevel),
			m_selectedIndex(-1),
			m_lightIndex(-1),
			m_selectedPointColor(CL_Colorf::yellow),
			m_pointColor(CL_Colorf::blue),
			m_radiusLineColor(CL_Colorf::white),
			m_shiftLineColor(CL_Colorf::green),
			m_selectedPointFrameColor(CL_Colorf::red),
			m_shiftPointColor(CL_Colorf::red),
			m_pressedId(CL_NONE_PRESSED),
			m_state(None)
		{
			setDefaultPoints();
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

		// input

		enum State
		{
			None,
			Point
		};

		int m_pressedId;

		State m_state;

		// references

		Track& m_track;

		Gfx::Level& m_gfxLevel;

		// methods

		void handleInput(bool p_pressed, const CL_InputEvent &p_event);

		void mouseMoved(const CL_Pointf &p_mousePos, const CL_Pointf &p_lastMousePos, const CL_Pointf &p_deltaPos);

		void draw(CL_GraphicContext &p_gc);

		void load(CL_GraphicContext &p_gc);

		void update(unsigned int p_timeElapsed);

		void setDefaultPoints();

		void drawPoints(CL_GraphicContext &p_gc);

		void drawPoint(int p_index, bool &p_isSelected, bool &p_isLight, CL_GraphicContext &p_gc);

		void findPointAt(const CL_Pointf &p_pos, int &p_index);

		void getShiftRect(int p_index, int* x1, int* y1, int* x2, int* y2);

		CL_Rect getPointRect(int p_index);

		CL_Rect getPointRect(const CL_Pointf &p_point);

		void setToPerpendicular(CL_Vec2f &p_vector2, bool p_isInvert);

		void setMinAndMaxShiftPoint(int p_index);

		void setRadius(int p_index, int p_set);

		void triangulate(int p_index);

		bool getHandle() const;
	};

	void EditorPointImpl::setDefaultPoints()
	{
		m_track.clear();

		m_track.addPoint(CL_Pointf(150.0f, 150.0f), 50.0f, 0.0f);
		m_track.addPoint(CL_Pointf(600.0f, 120.0f), 50.0f, 0.0f);
		m_track.addPoint(CL_Pointf(600.0f, 400.0f), 60.0f, 0.0f);
		m_track.addPoint(CL_Pointf(200.0f, 450.0f), 40.0f, 0.0f);

		m_gfxLevel.getTrackTriangulator().triangulate(m_track);
	}

	bool EditorPointImpl::getHandle() const
	{
		return (!m_pressedId == CL_NONE_PRESSED);
	}

	void EditorPointImpl::draw(CL_GraphicContext &p_gc)
	{
		drawPoints(p_gc);
	}

	void EditorPointImpl::load(CL_GraphicContext &p_gc)
	{

	}

	void EditorPointImpl::update(unsigned int p_timeElapsed)
	{
		setMinAndMaxShiftPoint(m_selectedIndex);
	}

	void EditorPointImpl::drawPoints(CL_GraphicContext &p_gc)
	{
		for (int i = 0; i < m_track.getPointCount(); ++i)
		{
			bool isSelected = (i == m_selectedIndex) ? true : false;
			bool isLight = (i == m_lightIndex) ? true : false;
			drawPoint(i, isSelected, isLight, p_gc);
		}
	}

	void EditorPointImpl::drawPoint(int p_index, bool &p_isSelected, bool &p_isLight, CL_GraphicContext &p_gc)
	{
		CL_Colorf color = (p_isSelected || p_isLight) ? m_selectedPointColor : m_pointColor;

		if (p_isSelected)
		{
			if (m_pressedId == CL_KEY_CONTROL && m_selectedIndex != -1)
			{
				CL_Pen pen;

				pen.set_line_width(PAINT_LINE_WIDTH);
				p_gc.set_pen(pen);

				int x1, y1, x2, y2;
				getShiftRect(p_index, &x1, &y1, &x2, &y2);

				CL_Draw::line(p_gc, x1, y1, x2, y2, m_shiftLineColor);

				pen.set_line_width(1.0f);
				p_gc.set_pen(pen);

				CL_Draw::fill(p_gc, getPointRect(m_minShiftPoint), CL_Colorf::red);
				CL_Draw::fill(p_gc, getPointRect(m_maxShiftPoint), CL_Colorf::red);
			}
		}

		CL_Draw::fill(p_gc, getPointRect(p_index), color);

		if (p_isSelected)
		{
			CL_Draw::box(p_gc, getPointRect(p_index), m_selectedPointFrameColor);
		}
	}

	void EditorPointImpl::findPointAt(const CL_Pointf &p_pos, int &p_index)
	{
		if (m_pressedId == CL_NONE_PRESSED)
		{
			p_index = -1;
			m_state = None;

			CL_Rect rect = getPointRect(p_pos);

			for (int i = 0; i < m_track.getPointCount(); ++i)
			{
				if (rect.contains(m_track.getPoint(i).getPosition()))
				{
					m_state = Point;
					p_index = i;

					return;
				}
			}
		}
	}

	void EditorPointImpl::setToPerpendicular(CL_Vec2f& p_vector2, bool p_isInvert)
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

	void EditorPointImpl::setMinAndMaxShiftPoint(int p_index)
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

			m_minShiftPoint = pos + minShiftGuide;
			m_maxShiftPoint = pos + maxShiftGuide;
		}
	}

	void EditorPointImpl::getShiftRect(int p_index, int* x1, int* y1, int* x2, int* y2)
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

	CL_Rect EditorPointImpl::getPointRect(int p_index)
	{
		TrackPoint p_trackPoint = m_track.getPoint(p_index);

		CL_Rect rect;

		rect.left = p_trackPoint.getPosition().x - POINT_WIDTH;
		rect.top = p_trackPoint.getPosition().y - POINT_WIDTH;
		rect.right = p_trackPoint.getPosition().x + POINT_WIDTH;
		rect.bottom = p_trackPoint.getPosition().y + POINT_WIDTH;

		return rect;
	}

	CL_Rect EditorPointImpl::getPointRect(const CL_Pointf &p_point)
	{
		CL_Rect rect;

		rect.left = p_point.x - POINT_WIDTH;
		rect.top = p_point.y - POINT_WIDTH;
		rect.right = p_point.x + POINT_WIDTH;
		rect.bottom = p_point.y + POINT_WIDTH;

		return rect;
	}

	void EditorPointImpl::setRadius(int p_index, int p_set)
	{
		if (p_index >= 0 && p_index < m_track.getPointCount())
		{
			TrackPoint& trackPoint = m_track.getPoint(p_index);

			float newRadius = trackPoint.getRadius() + p_set;

			if (newRadius > 40.0f)
				trackPoint.setRadius(newRadius);
		}
	}

	void EditorPointImpl::triangulate(int p_index)
	{
		int segment = p_index - 2;
		if (p_index < 0)
			segment = m_track.getPointCount() - segment;

		for (int i = 0; i < 4; ++i)
		{
			if (segment < 0)
				segment = m_track.getPointCount() - 1;
			else if (segment > m_track.getPointCount() - 1)
				segment = 0;

			m_gfxLevel.getTrackTriangulator().triangulate(m_track, segment);

			++segment;
		}
	}

	void EditorPointImpl::handleInput(bool p_pressed, const CL_InputEvent& p_event)
	{
		if (p_pressed && m_pressedId == CL_NONE_PRESSED)
		{
			m_pressedId = p_event.id;
		}
		else if (!p_pressed && p_event.id == m_pressedId)
		{
			m_pressedId = CL_NONE_PRESSED;
		}

		// bercik: radius set (to change and delete)
		if (p_pressed)
		{
			if (p_event.id == CL_KEY_UP)
			{
				setRadius(m_selectedIndex, 1);
			}
			else if (p_event.id == CL_KEY_DOWN)
			{
				setRadius(m_selectedIndex, -1);
			}

			triangulate(m_selectedIndex);
		}

		if (m_pressedId == CL_MOUSE_LEFT)
		{
			if (m_state == Point)
			{
				m_selectedIndex = m_lightIndex;
			}
			else
			{
				m_selectedIndex = m_lightIndex = -1;
			}
		}

	}

	void EditorPointImpl::mouseMoved(const CL_Pointf &p_mousePos, const CL_Pointf &p_lastMousePos, const CL_Pointf &p_deltaPos)
	{
		findPointAt(p_mousePos, m_lightIndex);
		
		if (m_selectedIndex != -1)
		{
			TrackPoint trackPoint = m_track.getPoint(m_selectedIndex);

			if (m_pressedId == CL_KEY_CONTROL)
			{
				float lastMaxShiftDeltaLength = m_maxShiftPoint.distance(p_lastMousePos);
				float lastMinShiftDeltaLength = m_minShiftPoint.distance(p_lastMousePos);

				float nowMaxShiftDeltaLength = m_maxShiftPoint.distance(p_mousePos);
				float nowMinShiftDeltaLength = m_minShiftPoint.distance(p_mousePos);

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
			else if (m_pressedId == CL_MOUSE_LEFT)
			{
				trackPoint.setPosition(trackPoint.getPosition() + p_deltaPos);
			}

			triangulate(m_selectedIndex);
		}
	}

	EditorPoint::EditorPoint(Track& p_track, Gfx::Level& p_gfxLevel) : 
		m_impl(new EditorPointImpl(p_track, p_gfxLevel))
	{

	}

	EditorPoint::~EditorPoint()
	{

	}

	void EditorPoint::draw(CL_GraphicContext &p_gc)
	{
		m_impl->draw(p_gc);
	}

	void EditorPoint::load(CL_GraphicContext &p_gc)
	{
		m_impl->load(p_gc);
	}

	void EditorPoint::mouseMoved(const CL_Pointf &p_mousePos, const CL_Pointf &p_lastMousePos, const CL_Pointf &p_deltaPos)
	{
		m_impl->mouseMoved(p_mousePos, p_lastMousePos, p_deltaPos);
	}

	void EditorPoint::update(unsigned int p_timeElapsed)
	{
		m_impl->update(p_timeElapsed);
	}

	void EditorPoint::handleInput(bool p_pressed, const CL_InputEvent& p_event)
	{
		m_impl->handleInput(p_pressed, p_event);
	}

	bool EditorPoint::getHandle() const
	{
		return m_impl->getHandle();
	}
}
