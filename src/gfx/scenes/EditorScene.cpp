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

#include "EditorScene.h"

#include "common.h"
#include "gfx/Stage.h"
#include "gfx/race/level/Level.h"
#include "logic/race/level/Level.h"
#include "logic/race/level/Track.h"
#include "logic/race/level/TrackPoint.h"

using namespace Race;
using namespace Gfx;

const int LABEL_WIDTH = 280;

const int BUTTON_WIDTH = 100;
const int BUTTON_HEIGHT = 20;

const int V_MARGIN = 40;

const int MIN_LINE_LENGTH = 40;
const int PAINT_LINE_WIDTH = 2;
const int DISCOVER_LINE_WIDTH = 12;

class EditorSceneImpl
{
public:
	EditorSceneImpl() : 
			m_controller(NULL),
			m_raceLevel(),
			m_track(),
			m_selectedIndex(-1),
			m_lightIndex(-1),
			m_selectedPointColor(CL_Colorf::yellow),
			m_pointColor(CL_Colorf::blue),
			m_lineColor(CL_Colorf::white),
			m_selectedPointFrameColor(CL_Colorf::red),
			m_lastMousePos(0.0f, 0.0f),
			m_pressedState(PressedState::None),
			m_isPressed(false)
	{
		m_gfxLevel = new Gfx::Level(m_raceLevel);

		setDefaultPoints();

		m_raceLevel.setTrack(m_track);
	}

	~EditorSceneImpl()
	{
		m_raceLevel.destroy();
	}

	int m_selectedIndex;

	int m_lightIndex;

	CL_Colorf m_selectedPointFrameColor;

	CL_Colorf m_selectedPointColor;

	CL_Colorf m_pointColor;

	CL_Colorf m_lineColor;

	// track

	Race::Level m_raceLevel;

	Gfx::Level* m_gfxLevel;

	Track m_track;

	// input

	enum PressedState
	{
		None,
		Point,
		ShiftLeft,
		ShiftRight,
		RadiusUp,
		RadiusDown
	};

	enum InputState 
	{
		Pressed,
		Released
	};

	CL_Point m_lastMousePos;

	PressedState m_pressedState;

	bool m_isPressed;

	// scene controller

	EditorController m_controller;

	// gui components


	// Methods

	void setDefaultPoints();

	void handleInput(InputState p_state, const CL_InputEvent& p_event);

	void mouseMoved(const CL_Point &p_pos);

	void draw(CL_GraphicContext &p_gc);

	void load(CL_GraphicContext &p_gc);

	void drawPoints(CL_GraphicContext &p_gc);

	void drawPoint(const TrackPoint &p_trackPoint, bool &p_isSelected, bool &p_isLight, CL_GraphicContext &p_gc);

	void findPointAt(const CL_Point &p_pos, int &p_index, PressedState &p_pressedState);

	CL_Rect& getRadiusUpRect(const TrackPoint &p_trackPoint, int p_lineWidth);

	CL_Rect& getRadiusDownRect(const TrackPoint &p_trackPoint, int p_lineWidth);

	CL_Rect& getRadiusRect(const TrackPoint &p_trackPoint, int p_lineWidth);

	CL_Rect& getShiftLeftRect(const TrackPoint &p_trackPoint, int p_lineWidth);

	CL_Rect& getShiftRightRect(const TrackPoint &p_trackPoint, int p_lineWidth);

	CL_Rect& getShiftRect(const TrackPoint &p_trackPoint, int p_lineWidth);

	CL_Rect& getPointRect(const TrackPoint &p_trackPoint);

	CL_Rect& getPointRect(const CL_Point &p_point);

	// action slots
};

void EditorSceneImpl::setDefaultPoints()
{
	m_track.clear();

	m_track.addPoint(CL_Pointf(200.0f, 150.0f), 50.0f, 0.0f);
	m_track.addPoint(CL_Pointf(600.0f, 150.0f), 50.0f, 0.0f);
	m_track.addPoint(CL_Pointf(400.0f, 450.0f), 60.0f, 0.0f);
}

void EditorSceneImpl::draw(CL_GraphicContext &p_gc)
{
	(*m_gfxLevel).draw(p_gc);

	drawPoints(p_gc);
}

void EditorSceneImpl::load(CL_GraphicContext &p_gc)
{
	(*m_gfxLevel).load(p_gc);
}

void EditorSceneImpl::drawPoints(CL_GraphicContext &p_gc)
{
	for (int i = 0; i < m_track.getPointCount(); ++i)
	{
		bool isSelected = (i == m_selectedIndex) ? true : false;
		bool isLight = (i == m_lightIndex) ? true : false;
		drawPoint(m_track.getPoint(i), isSelected, isLight, p_gc);
	}
}

void EditorSceneImpl::drawPoint(const TrackPoint &p_trackPoint, bool &p_isSelected, bool &p_isLight, CL_GraphicContext &p_gc)
{
	CL_Colorf color = (p_isSelected || p_isLight) ? m_selectedPointColor : m_pointColor;

	if (p_isSelected)
	{
		CL_Draw::fill(p_gc, getRadiusRect(p_trackPoint, PAINT_LINE_WIDTH), m_lineColor);
		CL_Draw::fill(p_gc, getShiftRect(p_trackPoint, PAINT_LINE_WIDTH), m_lineColor);
	}

	CL_Draw::fill(p_gc, getPointRect(p_trackPoint), color);

	if (p_isSelected)
	{
		CL_Draw::box(p_gc, getPointRect(p_trackPoint), m_selectedPointFrameColor);
	}
}

void EditorSceneImpl::findPointAt(const CL_Point &p_pos, int &p_index, PressedState &p_pressedState)
{
	if (!m_isPressed)
	{
		p_index = -1;
		p_pressedState = PressedState::None;

		CL_Rect rect = getPointRect(p_pos);

		for (int i = 0; i < m_track.getPointCount(); ++i)
		{
			if (rect.contains(m_track.getPoint(i).getPosition()))
			{
				p_pressedState = PressedState::Point;
				p_index = i;

				return;
			}
		}

		if (m_selectedIndex != -1)
		{
			rect = getRadiusUpRect(m_track.getPoint(m_selectedIndex), DISCOVER_LINE_WIDTH);
			if (rect.contains(p_pos))
			{
				p_pressedState = PressedState::RadiusUp;
				p_index = m_selectedIndex;

				return;
			}

			rect = getRadiusDownRect(m_track.getPoint(m_selectedIndex), DISCOVER_LINE_WIDTH);
			if (rect.contains(p_pos))
			{
				p_pressedState = PressedState::RadiusDown;
				p_index = m_selectedIndex;

				return;
			}

			rect = getShiftLeftRect(m_track.getPoint(m_selectedIndex), DISCOVER_LINE_WIDTH);
			if (rect.contains(p_pos))
			{
				p_pressedState = PressedState::ShiftLeft;
				p_index = m_selectedIndex;

				return;
			}

			rect = getShiftRightRect(m_track.getPoint(m_selectedIndex), DISCOVER_LINE_WIDTH);
			if (rect.contains(p_pos))
			{
				p_pressedState = PressedState::ShiftRight;
				p_index = m_selectedIndex;

				return;
			}
		}
	}
}

CL_Rect& EditorSceneImpl::getRadiusRect(const TrackPoint &p_trackPoint, int p_lineWidth)
{
	CL_Point pos = p_trackPoint.getPosition();
	int radiusLine = p_trackPoint.getRadius() + MIN_LINE_LENGTH;

	CL_Point point(pos.x - (p_lineWidth / 2), pos.y - (radiusLine / 2));
	CL_Size size(p_lineWidth, radiusLine);

	CL_Rect rect(point, size);

	return rect;
}

CL_Rect& EditorSceneImpl::getRadiusUpRect(const TrackPoint &p_trackPoint, int p_lineWidth)
{
	CL_Point pos = p_trackPoint.getPosition();
	int radiusLine = (p_trackPoint.getRadius() + MIN_LINE_LENGTH) / 2;

	CL_Point point(pos.x - (p_lineWidth / 2), pos.y - (radiusLine));
	CL_Size size(p_lineWidth, radiusLine);

	CL_Rect rect(point, size);

	return rect;
}

CL_Rect& EditorSceneImpl::getRadiusDownRect(const TrackPoint &p_trackPoint, int p_lineWidth)
{
	CL_Point pos = p_trackPoint.getPosition();
	int radiusLine = (p_trackPoint.getRadius() + MIN_LINE_LENGTH) / 2;

	CL_Point point(pos.x - (p_lineWidth / 2), pos.y);
	CL_Size size(p_lineWidth, radiusLine);

	CL_Rect rect(point, size);

	return rect;
}

CL_Rect& EditorSceneImpl::getShiftRect(const TrackPoint &p_trackPoint, int p_lineWidth)
{
	CL_Point pos = p_trackPoint.getPosition();
	int shiftLine = p_trackPoint.getShift() + MIN_LINE_LENGTH;

	CL_Point point(pos.x - (shiftLine / 2), pos.y - (p_lineWidth / 2));
	CL_Size size(shiftLine, p_lineWidth);

	CL_Rect rect(point, size);

	return rect;
}

CL_Rect& EditorSceneImpl::getShiftLeftRect(const TrackPoint &p_trackPoint, int p_lineWidth)
{
	CL_Point pos = p_trackPoint.getPosition();
	int shiftLine = (p_trackPoint.getShift() + MIN_LINE_LENGTH) / 2;

	CL_Point point(pos.x - shiftLine, pos.y - (p_lineWidth / 2));
	CL_Size size(shiftLine, p_lineWidth);

	CL_Rect rect(point, size);

	return rect;
}

CL_Rect& EditorSceneImpl::getShiftRightRect(const TrackPoint &p_trackPoint, int p_lineWidth)
{
	CL_Point pos = p_trackPoint.getPosition();
	int shiftLine = (p_trackPoint.getShift() + MIN_LINE_LENGTH) / 2;

	CL_Point point(pos.x, pos.y - (p_lineWidth / 2));
	CL_Size size(shiftLine, p_lineWidth);

	CL_Rect rect(point, size);

	return rect;
}

CL_Rect& EditorSceneImpl::getPointRect(const TrackPoint &p_trackPoint)
{
	CL_Rect rect;

	rect.left = p_trackPoint.getPosition().x - 5.0f;
	rect.top = p_trackPoint.getPosition().y - 5.0f;
	rect.right = p_trackPoint.getPosition().x + 5.0f;
	rect.bottom = p_trackPoint.getPosition().y + 5.0f;

	return rect;
}

CL_Rect& EditorSceneImpl::getPointRect(const CL_Point &p_point)
{
	CL_Rect rect;

	rect.left = p_point.x - 5.0f;
	rect.top = p_point.y - 5.0f;
	rect.right = p_point.x + 5.0f;
	rect.bottom = p_point.y + 5.0f;

	return rect;
}

void EditorSceneImpl::handleInput(InputState p_state, const CL_InputEvent& p_event)
{
	bool pressed;

	switch (p_state) {
		case Pressed:
			pressed = true;
			break;
		case Released:
			pressed = false;
			break;
		default:
			assert(0 && "unknown input state");
	}

	switch (p_event.id) {
		case CL_MOUSE_LEFT:
			m_isPressed = pressed;
			break;
		default:
			break;
	}

	if (m_isPressed)
	{
		if (m_pressedState == PressedState::Point)
			m_selectedIndex = m_lightIndex;
		else if (m_pressedState == PressedState::None)
			m_selectedIndex = m_lightIndex = -1;
	}


	if (pressed && p_event.id == CL_KEY_ESCAPE) 
	{
		Stage::popScene();
	}
}

void EditorSceneImpl::mouseMoved(const CL_Point &p_pos)
{
	findPointAt(p_pos, m_lightIndex, m_pressedState);

	
	if (m_selectedIndex != -1 && m_isPressed)
	{
		TrackPoint *trackPoint = const_cast<TrackPoint*>(&m_track.getPoint(m_selectedIndex));
		CL_Point deltaPos = p_pos - m_lastMousePos;

		switch (m_pressedState)
		{
		case PressedState::Point:
			trackPoint->setPosition(trackPoint->getPosition() + deltaPos);
			break;
		case PressedState::RadiusUp:
			if (trackPoint->getRadius() - (deltaPos.y * 2) > 0.0f)
				trackPoint->setRadius(trackPoint->getRadius() - (deltaPos.y * 2));
			break;
		case PressedState::RadiusDown:
			if (trackPoint->getRadius() + (deltaPos.y * 2) > 0.0f)
				trackPoint->setRadius(trackPoint->getRadius() + (deltaPos.y * 2));
			break;
		case PressedState::ShiftLeft:
			if (trackPoint->getShift() - deltaPos.x > 0.0f)
				trackPoint->setShift(trackPoint->getShift() - (deltaPos.x * 2));
			break;
		case PressedState::ShiftRight:
			if (trackPoint->getShift() + deltaPos.x > 0.0f)
				trackPoint->setShift(trackPoint->getShift() + (deltaPos.x * 2));
			break;
		}

		m_raceLevel.setTrack(m_track);
		//m_gfxLevel = new Gfx::Level(m_raceLevel);
	}

	m_lastMousePos = p_pos;
}

EditorScene::EditorScene() :
	m_impl(new EditorSceneImpl())
{
	m_impl->m_controller = EditorController(this);
}

EditorScene::~EditorScene()
{
}

void EditorScene::draw(CL_GraphicContext &p_gc)
{
	DirectScene::draw(p_gc);

	CL_Draw::fill(p_gc, 0.0f, 0.0f, Stage::getWidth(), Stage::getHeight(), CL_Colorf::black);

	m_impl->draw(p_gc);
}

void EditorScene::load(CL_GraphicContext &p_gc)
{
	DirectScene::load(p_gc);

	m_impl->load(p_gc);
}

void EditorScene::inputPressed(const CL_InputEvent &p_event)
{
	m_impl->handleInput(EditorSceneImpl::InputState::Pressed, p_event);
}

void EditorScene::inputReleased(const CL_InputEvent &p_event)
{
	m_impl->handleInput(EditorSceneImpl::InputState::Released, p_event);
}

void EditorScene::mouseMoved(const CL_Point &p_pos)
{
	m_impl->mouseMoved(p_pos);
}