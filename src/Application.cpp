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

#include <stdlib.h>

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/application.h>
#include <ClanLib/gl.h>
#include <ClanLib/gl1.h>

#include "common/Collections.h"
#include "common/Game.h"
#include "common/Player.h"
#include "common/Properties.h"
#include "gfx/DebugLayer.h"
#include "gfx/GameWindow.h"
#include "gfx/Stage.h"
#include "gfx/race/ui/RaceUI.h"
#include "gfx/scenes/MainMenuScene.h"
#include "gfx/scenes/RaceScene.h"
#include "logic/race/level/Level.h"
#include "network/client/Client.h"


// vsync configuration (should be set by cmake)

#if defined(VSYNC)
const int SYNC_PARAM = 1;
#elif defined(NO_VSYNC)
const int SYNC_PARAM = 0;
#else
const int SYNC_PARAM = 1;
#endif

// default screen size
const int DEF_SCREEN_W = 800;
const int DEF_SCREEN_H = 600;
const bool DEF_FULLSCREEN = false;
const int DEF_OPENGL_VER = 2;


class Application
{
	public:
		static int main(const std::vector<CL_String> &args);

		static void onWindowClose();

		/** This method must be called to do a proper drawing */
		static void wmRepaint() {}

};

// Create global application object:
// You MUST include this line or the application start-up will fail to
// locate your application object.
CL_ClanApplication app(&Application::main);

int Application::main(const std::vector<CL_String> &args)
{

	CL_SetupGL1 *setup_gl1 = NULL;
	CL_SetupGL *setup_gl2 = NULL;

	try {

		// modules setup
		CL_Console::write_line("initializing");

		CL_SetupCore 	setup_core;

		CL_ConsoleLogger logger;
		CL_SlotContainer slots;

		CL_Console::write_line("loading properties");
		Properties::load(CONFIG_FILE_CLIENT);

		// app switches

		// use gl1 instead of gl2
		static const CL_String SWITCH_GL1 = "-gl1";

		// parameter prefix
		static const CL_String PREFIX_PARAM = "-P";


		// read args properties
		static const int PREFIX_PARAM_LEN = PREFIX_PARAM.length();

		foreach (const CL_String &arg, args) {
			if (arg.substr(0, PREFIX_PARAM_LEN) == PREFIX_PARAM) {
				const std::vector<CL_TempString> parts =
						CL_StringHelp::split_text(
								arg.substr(PREFIX_PARAM_LEN),
								"="
						);

				if (parts.size() == 2) {
					Properties::set(parts[0], parts[1]);
				} else {
					CL_Console::write_line(cl_format("cannot parse %1", arg));
				}
			}
		}

		// set opengl version
		enum GLVer {
			GL1,
			GL2
		} glver = GL2;

		const bool useGL1Opt =
				Properties::getInt(CG_OPENGL_VER, DEF_OPENGL_VER) == 1;

		if (Collections::contains(args, SWITCH_GL1) || useGL1Opt) {
			glver = GL1;
		}


		cl_log_event("init", "initializing display");
		CL_SetupDisplay setup_display;

		Gfx::Stage::m_width =
				Properties::getInt(CG_SCREEN_WIDTH, DEF_SCREEN_W);
		Gfx::Stage::m_height =
				Properties::getInt(CG_SCREEN_HEIGHT, DEF_SCREEN_H);

		const bool fullscreenOpt =
				Properties::getBool(CG_FULLSCREEN, DEF_FULLSCREEN);

		cl_log_event("init", "display will be %1 x %2", Gfx::Stage::m_width, Gfx::Stage::m_height);

		cl_log_event("init", "initializing gui");
		CL_SetupGUI 	setup_gui;

		cl_log_event("init", "initializing network");
		CL_SetupNetwork setup_network;


		// initialize opengl
		switch (glver) {
			case GL1:
				cl_log_event("init", "initializing OpenGL 1.x");
				setup_gl1 = new CL_SetupGL1();
				break;

			case GL2:
				cl_log_event("init", "initializing OpenGL 2.x");
				setup_gl2 = new CL_SetupGL();
				break;

			default:
				G_ASSERT(0 && "unknown GLVer option");
		}


		CL_OpenGLWindowDescription winDesc;

		winDesc.set_title("Gear");
		winDesc.set_size(
				CL_Size(Gfx::Stage::m_width, Gfx::Stage::m_height),
				true
		);
		winDesc.set_fullscreen(fullscreenOpt);

		// triple buffering
		winDesc.set_flipping_buffers(3);

		CL_DisplayWindow displayWindow(winDesc);

		// window close action
		slots.connect_functor(displayWindow.sig_window_close(), &Application::onWindowClose);

		cl_log_event("init", "loading gui components");

		CL_GUIWindowManagerTexture windowManager(displayWindow);

		CL_GUIManager guiManager;
		CL_GUIThemeDefault guiTheme;

		guiManager.set_window_manager(windowManager);

		if (glver == GL1) {
			//  Note - If you are using the GL1 target, you will get a perfomance increase by enabling these 2 lines
			//   It reduces the number of internal CL_FrameBuffer swaps. The GL1 target (OpenGL 1.3), performs this slowly
			//   Setting the texture group here, lets the GUI Texture Window Manager know the optimum texture size of all root components
			CL_TextureGroup texture_group(displayWindow.get_gc(), CL_Size(512, 512));
			windowManager.set_texture_group(texture_group);
		}

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

		CL_GraphicContext &gc = displayWindow.get_gc();
		CL_InputContext &ic = displayWindow.get_ic();

		Gfx::GameWindow gameWindow(&guiManager, &windowManager, &ic);

		// load debug layer
		DebugLayer debugLayer;
		Gfx::Stage::m_debugLayer = &debugLayer;

		cl_log_event("init", "launching main scene");

		windowManager.func_repaint().set(&Application::wmRepaint);

		MainMenuScene mainMenuScene(&gameWindow);
		Gfx::Stage::pushScene(&mainMenuScene);

		while(gameWindow.update()) {
			gameWindow.draw(gc);
			displayWindow.flip(SYNC_PARAM);
		}

	} catch (CL_Exception &e) {
		CL_Console::write_line(e.message);
	} catch (std::exception &e) {
		CL_Console::write_line(e.what());
	}

	Properties::save(CONFIG_FILE_CLIENT);

	// free resources
	if (setup_gl1) {
		delete setup_gl1;
	}

	if (setup_gl2) {
		delete setup_gl2;
	}

	CL_Console::write_line("Thanks for playing :-)");

	return 0;
}

void Application::onWindowClose()
{
	exit(0);
}
