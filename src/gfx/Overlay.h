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

#pragma once

#include "boost/utility.hpp"

#include <ClanLib/core.h>
#include <ClanLib/gui.h>

namespace Gfx
{

class DirectScene;
class OverlayImpl;

/**
 * Overlay is gui component on top of Gfx::DirectScene.
 */
class Overlay : public CL_GUIComponent
{
	public:

		Overlay(DirectScene &p_scene, const CL_Rect &p_geom);

		virtual ~Overlay();


		virtual void load(CL_GraphicContext &p_gc) = 0;

		virtual void draw(CL_GraphicContext &p_gc, const CL_Rect &p_clip) = 0;


		bool isVisible();

		const CL_Rect &getGeometry();


		void setVisible(bool p_visible);


	private:

		CL_SharedPtr<OverlayImpl> m_impl;


		bool is_visible();

		CL_Rect get_geometry();

		void render(CL_GraphicContext &p_gc, const CL_Rect &p_clip);

		void set_visible(bool p_visible);
};

}

