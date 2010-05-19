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

#include "ConnectScene.h"

#include "clanlib/display/2d.h"

#include "controllers/ConnectController.h"
#include "gfx/widgets/Header.h"

namespace Gfx
{

class ConnectSceneImpl
{
	public:

		ConnectScene *m_parent;
		ConnectController m_controller;

		Gfx::Header m_statusHeader;

		ConnectSceneImpl(ConnectScene *p_parent);
		~ConnectSceneImpl();
};

// --------------------------------------------------------

ConnectScene::ConnectScene(CL_GUIComponent *p_parent) :
		GuiScene(p_parent),
		m_impl(new ConnectSceneImpl(this))
{
	// empty
}

ConnectSceneImpl::ConnectSceneImpl(ConnectScene *p_parent) :
		m_parent(p_parent),
		m_controller(p_parent),
		m_statusHeader(p_parent)
{
	m_statusHeader.set_geometry(CL_Rect(50, 50, 300, 100));
	m_statusHeader.setTextSize(40);
	m_statusHeader.setTextColor(CL_Colorf::white);
}

ConnectScene::~ConnectScene()
{
	// empty
}

ConnectSceneImpl::~ConnectSceneImpl()
{
	// empty
}

// --------------------------------------------------------

void ConnectScene::draw(CL_GraphicContext &p_gc)
{
	CL_Draw::fill(p_gc, 0, 0, get_width(), get_height(), CL_Colorf::black);
}

ConnectController &ConnectScene::getController()
{
	return m_impl->m_controller;
}

void ConnectScene::setStatusText(const CL_String &p_statusText)
{
	m_impl->m_statusHeader.setText(p_statusText);
}

} // namespace
