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

#ifdef GL1
#include <ClanLib/gl1.h>
#elif GL2
#include <ClanLib/gl.h>
#endif // GL1/2

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
//	if (args.size() < 3) {
//		CL_Console::write_line("usage: ./game server_addr nickname");
//		return 1;
//	}
//
//	const CL_String serverAddrPort = args[1];
//	const CL_String nickname = args[2];

	std::string stdinNickname, stdinServer, stdinConnect;

//	std::cout << "Player name: ";
//	std::cin >> stdinNickname;
//
//	do {
//		std::cout << "Do you want to connect to server? (y/n): ";
//		std::cin >> stdinConnect;
//	} while (stdinConnect != "y" && stdinConnect != "n");
//
//	if (stdinConnect == "y") {
//		std::cout << "Please specify the server to connect to. The acceptable format is HOST, HOST:PORT or write anything to ." << std::endl;
//		std::cout << "Server addr: ";
//		std::cin >> stdinServer;
//	}
//
//	const CL_String serverAddrPort = stdinServer;
//	const CL_String nickname = stdinNickname;
	const CL_String serverAddrPort = "localhost";
	const CL_String nickname = "n";

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

	Gfx::Stage::m_width = 800;
	Gfx::Stage::m_height = 600;

	// Setup clanlib modules:
	CL_SetupCore 	setup_core;
	CL_SetupDisplay setup_display;
	CL_SetupGUI 	setup_gui;

#ifdef GL1
	CL_SetupGL1     setup_gl;
#elif GL2
	CL_SetupGL     setup_gl;
#endif // GL1/2
	CL_SetupNetwork setup_network;

	CL_ConsoleLogger logger;

	CL_DisplayWindow display_window("Gear", Gfx::Stage::m_width, Gfx::Stage::m_height);

	// load resources
	CL_ResourceManager resources("resources/resources.xml");
	Gfx::Stage::m_resourceManager = &resources;

	CL_GUIWindowManagerTexture wm(display_window);

	// load GUI
	CL_GUIManager gui;
	CL_GUIThemeDefault theme;
//	CL_GUIWindowManagerSystem window_manager;

	gui.set_window_manager(wm);

#ifdef GL1
	//  Note - If you are using the GL1 target, you will get a perfomance increase by enabling these 2 lines
	//   It reduces the number of internal CL_FrameBuffer swaps. The GL1 target (OpenGL 1.3), performs this slowly
	//   Setting the texture group here, lets the GUI Texture Window Manager know the optimum texture size of all root components
	CL_TextureGroup texture_group(display_window.get_gc(), CL_Size(512, 512));
	wm.set_texture_group(texture_group);
#endif //GL1

	CL_ResourceManager res("resources/GUIThemeAeroPacked/resources.xml");
	CL_CSSDocument css_document;
	css_document.load("resources/GUIThemeAeroPacked/theme.css");

	gui.set_css_document(css_document);
	theme.set_resources(res);
	gui.set_theme(theme);

	CL_DisplayWindowDescription desc("Gear");
	desc.set_position(CL_Rect(0, 0, Gfx::Stage::getWidth(), Gfx::Stage::getHeight()), true);

	GameWindow gameWindow(&gui, desc);

	// load debug layer
	DebugLayer debugLayer;
	Gfx::Stage::m_debugLayer = &debugLayer;

	// build race game

	try {

		SceneContainer sceneContainer(&gameWindow);
		Game::getInstance().setSceneContainer(&sceneContainer);

		Gfx::Stage::pushScene(&sceneContainer.getMainMenuScene());

		// run the gui
		gui.exec(true);
//		while (!gui.exec(false)) {
//
//		}

	} catch (CL_Exception e) {
		CL_Console::write_line(e.message);
	}

	CL_Console::write_line("Thanks for playing :-)");

	return 0;
}
