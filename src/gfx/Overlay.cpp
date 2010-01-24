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

#include "Overlay.h"

#include "gfx/DirectScene.h"

namespace Gfx
{

class OverlayImpl
{
	public:

		const DirectScene &m_scene;

		CL_Rect m_geom;

		bool m_loaded;


		explicit OverlayImpl(DirectScene &p_scene, const CL_Rect &p_geom) :
			m_scene(p_scene),
			m_geom(p_geom),
			m_loaded(false)
		{ /* empty */ }


};

Overlay::Overlay(DirectScene &p_scene, const CL_Rect &p_geom) :
	CL_GUIComponent(&p_scene.getParentComponent()),
	m_impl(new OverlayImpl(p_scene, p_geom))
{
	set_geometry(p_geom);
	func_render().set(this, &Overlay::render);
}

Overlay::~Overlay()
{
	// empty
}

void Overlay::render(CL_GraphicContext &p_gc, const CL_Rect &p_clip)
{
	if (!m_impl->m_loaded) {
		load(p_gc);
		m_impl->m_loaded = true;
	}

	draw(p_gc, p_clip);
}

bool Overlay::isVisible()
{
	return is_visible();
}

const CL_Rect &Overlay::getGeometry()
{
	return m_impl->m_geom;
}

void Overlay::setVisible(bool p_visible)
{
	set_visible(p_visible);
}

bool Overlay::is_visible()
{
	return CL_GUIComponent::is_visible();
}

void Overlay::set_visible(bool p_visible)
{
	CL_GUIComponent::set_visible(p_visible);
}

}
