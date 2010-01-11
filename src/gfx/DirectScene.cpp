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

#include "DirectScene.h"

#include "common.h"

namespace Gfx {

DirectScene::DirectScene() :
		m_loaded(false)
{
}

DirectScene::~DirectScene()
{
}

bool DirectScene::isLoaded() const
{
	return m_loaded;
}

SceneType DirectScene::getType() const
{
	return ST_DIRECT;
}

void DirectScene::setLoaded(bool p_loaded)
{
	m_loaded = p_loaded;
}

void DirectScene::draw(CL_GraphicContext &p_gc)
{
	G_ASSERT(m_loaded);
}

void DirectScene::inputPressed(const CL_InputEvent &p_event)
{
	// empty
}

void DirectScene::inputReleased(const CL_InputEvent &p_event)
{
	// empty
}

void DirectScene::load(CL_GraphicContext &p_gc)
{
	G_ASSERT(!m_loaded);
	m_loaded = true;
}

void DirectScene::mouseMoved(const CL_Point &p_pos)
{
	// empty
}

void DirectScene::pushed()
{
	// empty
}

void DirectScene::poped()
{
	// empty
}

void DirectScene::setActive(bool p_active)
{
	// empty
}

void DirectScene::update(unsigned p_timeElapsed)
{
	G_ASSERT(!m_loaded);
}

}
