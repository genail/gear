/*
 * Application.cpp
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#include <stdlib.h>

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/gl.h>
#include <ClanLib/application.h>

#include "graphics/DebugLayer.h"
#include "graphics/Stage.h"
#include "race/RaceScene.h"
#include "Player.h"
#include "Properties.h"
#include "race/RaceUI.h"
#include "race/Race.h"

#include "network/Client.h"

/* Default server port */
const int DEFAULT_PORT = 2500;


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

	std::cout << "Player name: ";
	std::cin >> stdinNickname;

	do {
		std::cout << "Do you want to connect to server? (y/n): ";
		std::cin >> stdinConnect;
	} while (stdinConnect != "y" && stdinConnect != "n");

	if (stdinConnect == "y") {
		std::cout << "Please specify the server to connect to. The acceptable format is HOST, HOST:PORT or write anything to ." << std::endl;
		std::cout << "Server addr: ";
		std::cin >> stdinServer;
	}

	const CL_String serverAddrPort = stdinServer;
	const CL_String nickname = stdinNickname;

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

	Stage::m_width = 800;
	Stage::m_height = 600;

	// Setup clanlib modules:
	CL_SetupCore setup_core;
	CL_SetupDisplay setup_display;
	CL_SetupGL setup_gl;
	CL_SetupNetwork setup_network;

	CL_ConsoleLogger logger;

	// Create a window:
	CL_DisplayWindow window("The Great Race Game", Stage::getWidth(), Stage::getHeight());


	CL_ResourceManager resources("resources/resources.xml");
	Stage::m_resourceManager = &resources;

	DebugLayer debugLayer;
	Stage::m_debugLayer = &debugLayer;

	Player player(nickname);

	try {

		Client client;

		if (serverAddrPort.size() > 0) {
			// separate server addr from port if possible
			std::vector<CL_TempString> parts = CL_StringHelp::split_text(serverAddrPort, ":");

			const CL_String serverAddr = parts[0];
			const int serverPort = (parts.size() == 2 ? CL_StringHelp::local8_to_int(parts[1]) : DEFAULT_PORT);

			client.connect(serverAddr, serverPort, &player);
		}

		Race race(&window, &player, &client);
		race.exec();
	} catch (CL_Exception e) {
		CL_Console::write_line(e.message);
	}

	CL_Console::write_line("Thanks for playing :-)");

	return 0;
}
