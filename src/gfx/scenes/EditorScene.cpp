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

class EditorSceneImpl
{
public:
	EditorSceneImpl() : 
			m_controller(NULL),
			m_raceLevel(),
			m_gfxLevel(m_raceLevel),
			m_track(),
			m_selectedIndex(-1),
			m_selectedColor(CL_Colorf::white),
			m_pointColor(CL_Colorf::blue)
	{
		setDefaultPoints();

		m_raceLevel.setTrack(m_track);
	}

	~EditorSceneImpl()
	{
		m_raceLevel.destroy();
	}

	int m_selectedIndex;

	CL_Colorf m_selectedColor;

	CL_Colorf m_pointColor;

	// track

	Race::Level m_raceLevel;

	Gfx::Level m_gfxLevel;

	Track m_track;

	// input

	bool m_pressed;

	enum InputState {
			Pressed,
			Released
		};

	// scene controller

	EditorController m_controller;

	// gui components


	// Methods

	void setDefaultPoints();

	void handleInput(InputState p_state, const CL_InputEvent& p_event);

	void draw(CL_GraphicContext &p_gc);

	void load(CL_GraphicContext &p_gc);

	void drawPoints(CL_GraphicContext &p_gc);

	void drawPoint(const TrackPoint &p_trackPoint, bool &p_isSelected, CL_GraphicContext &p_gc);

	int findPointAt(CL_Rect &p_rect);

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
	m_gfxLevel.draw(p_gc);

	drawPoints(p_gc);
}

void EditorSceneImpl::load(CL_GraphicContext &p_gc)
{
	m_gfxLevel.load(p_gc);
}

void EditorSceneImpl::drawPoints(CL_GraphicContext &p_gc)
{
	for (int i = 0; i < m_track.getPointCount(); ++i)
	{
		bool isSelected = (i == m_selectedIndex) ? true : false;
		drawPoint(m_track.getPoint(i), isSelected, p_gc);
	}
}

void EditorSceneImpl::drawPoint(const TrackPoint &p_trackPoint, bool &p_isSelected, CL_GraphicContext &p_gc)
{
	CL_Colorf color = p_isSelected ? m_selectedColor : m_pointColor;

	CL_Draw::circle(p_gc, p_trackPoint.getPosition().x, p_trackPoint.getPosition().y, 5.0f, color);
}

int EditorSceneImpl::findPointAt(CL_Rect &p_rect)
{
	for (int i = 0; i < m_track.getPointCount(); ++i)
	{
		if (p_rect.contains(m_track.getPoint(i).getPosition()))
		{
			return i;
		}
	}

	return -1;
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
			m_pressed = pressed;
			break;
		default:
			break;
	}

	CL_Pointf mousePos = p_event.mouse_pos;
	m_selectedIndex = findPointAt(CL_Rect(mousePos.x - 5, mousePos.y - 5, mousePos.x + 5, mousePos.y + 5));


	if (pressed && p_event.id == CL_KEY_ESCAPE) 
	{
		Stage::popScene();
	}
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
