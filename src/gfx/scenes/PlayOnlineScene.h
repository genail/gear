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

#pragma once

#include <ClanLib/core.h>

#include "gfx/GuiScene.h"

class PlayOnlineSceneImpl;
class PlayOnlineScene : public Gfx::GuiScene
{
	public:

		struct Entry {
			CL_String addr;
			int port;
			CL_String serverName;
			CL_String gamemode;
			CL_String mapName;
			int playerCountCurrent;
			int playerCountLimit;
			int ping;
		};

		PlayOnlineScene(CL_GUIComponent *p_parent);
		virtual ~PlayOnlineScene();

		virtual void draw(CL_GraphicContext &p_gc);

		void addServerEntry(const Entry &p_entry);
		void clearServerEntries();

		CL_Callback_v0 &refreshButtonClicked();
		CL_Callback_v0 &mainMenuButtonClicked();
		CL_Callback_v0 &connectButtonClicked();
		CL_Callback_v1<const PlayOnlineScene::Entry&> &serverEntrySelected();

	private:

		CL_SharedPtr<PlayOnlineSceneImpl> m_impl;

		friend class PlayOnlineSceneImpl;

};

