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

#include <stdlib.h>

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/application.h>

#if defined(GL1)
#include <ClanLib/gl1.h>
#elif defined(GL2)
#include <ClanLib/gl.h>
#else
#error must define GL1 or GL2
#endif // GL1 || GL2

#include "Game.h"
#include "graphics/GameWindow.h"
#include "graphics/DebugLayer.h"
#include "graphics/Stage.h"
#include "race/RaceScene.h"
#include "Player.h"
#include "Properties.h"
#include "graphics/RaceUI.h"
#include "race/Race.h"
#include "network/Client.h"
#include "gui/MainMenuScene.h"
#include "gui/SceneContainer.h"


class Application
{
	public:
		static int main(const std::vector<CL_String> &args);
};

// Create global application object:
// You MUST include this line or the application start-up will fail to
// locate your application object.
CL_ClanApplication app(&Application::main);

int Application::main(const std::vector<CL_String> &args)
{

	try {

		// set default properties
#ifndef NDEBUG
		Properties::setProperty("debug.draw_checkpoints", false);
		Properties::setProperty("debug.draw_vectors", false);
#endif //NDEBUG

		// read args properties
		for (std::vector<CL_String>::const_iterator itor = args.begin(); itor != args.end(); ++itor) {
			if (itor->substr(0, 2) == "-P") {
				const std::vector<CL_TempString> parts = CL_StringHelp::split_text(itor->substr(2), "=");

				if (parts.size() != 2) {
					CL_Console::write_line(CL_String8("cannot parse ") + *itor);
					continue;
				}

				Properties::setProperty(parts[0], parts[1]);
			}
		}

		// modules setup
		CL_Console::write_line("initializing");

		CL_SetupCore 	setup_core;
		CL_ConsoleLogger logger;

		cl_log_event("init", "initializing display");
		CL_SetupDisplay setup_display;

		Gfx::Stage::m_width = 800;
		Gfx::Stage::m_height = 600;

		cl_log_event("init", "display will be %1 x %2", Gfx::Stage::m_width, Gfx::Stage::m_height);

		cl_log_event("init", "initializing gui");
		CL_SetupGUI 	setup_gui;

		cl_log_event("init", "initializing network");
		CL_SetupNetwork setup_network;

#if defined(GL1)
		cl_log_event("init", "initializing OpenGL 1.x");
		CL_SetupGL1     setup_gl;

		CL_DisplayWindow displayWindow("Gear", Gfx::Stage::m_width, Gfx::Stage::m_height);
#else defined(GL2)
		cl_log_event("init", "initializing OpenGL 2.x");
		CL_SetupGL     setup_gl;

		CL_OpenGLWindowDescription displayWindowDescription;

		displayWindowDescription.set_title("Gear");
		displayWindowDescription.set_size(CL_Size(Gfx::Stage::m_width, Gfx::Stage::m_height), false);
		displayWindowDescription.set_multisampling(4);

		CL_DisplayWindow displayWindow(displayWindowDescription);
#endif // GL1 || GL2

		cl_log_event("init", "loading gui components");

		CL_GUIWindowManagerTexture windowManager(displayWindow);

		CL_GUIManager guiManager;
		CL_GUIThemeDefault guiTheme;

		guiManager.set_window_manager(windowManager);

#ifdef GL1
		//  Note - If you are using the GL1 target, you will get a perfomance increase by enabling these 2 lines
		//   It reduces the number of internal CL_FrameBuffer swaps. The GL1 target (OpenGL 1.3), performs this slowly
		//   Setting the texture group here, lets the GUI Texture Window Manager know the optimum texture size of all root components
		CL_TextureGroup texture_group(displayWindow.get_gc(), CL_Size(512, 512));
		windowManager.set_texture_group(texture_group);
#endif // GL1

		CL_ResourceManager guiResources("resources/GUIThemeAeroPacked/resources.xml");
		CL_CSSDocument cssDocument;
		cssDocument.load("resources/GUIThemeAeroPacked/theme.css");

		guiManager.set_css_document(cssDocument);
		guiTheme.set_resources(guiResources);
		guiManager.set_theme(guiTheme);

		// load resources
		cl_log_event("init", "loading general resources");

		CL_ResourceManager generalResources("resources/resources.xml");
		Gfx::Stage::m_resourceManager = &generalResources;

		CL_DisplayWindowDescription guiDesc("Gear");
		guiDesc.set_position(CL_Rect(0, 0, Gfx::Stage::getWidth(), Gfx::Stage::getHeight()), true);

		GameWindow gameWindow(&guiManager, guiDesc);

		// load debug layer
		DebugLayer debugLayer;
		Gfx::Stage::m_debugLayer = &debugLayer;

		cl_log_event("init", "setting scene container");

		SceneContainer sceneContainer(&gameWindow);
		Game::getInstance().setSceneContainer(&sceneContainer);


		cl_log_event("init", "launching main scene");

		Gfx::Stage::pushScene(&sceneContainer.getMainMenuScene());
		guiManager.exec(true);

	} catch (CL_Exception e) {
		CL_Console::write_line(e.message);
	}

	CL_Console::write_line("Thanks for playing :-)");

	return 0;
}
