/*
 * Application.cpp
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/gl.h>
#include <ClanLib/application.h>

#include "graphics/DebugLayer.h"
#include "graphics/Stage.h"
#include "race/RaceScene.h"

#include "network/Client.h"


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
	Stage::m_width = 800;
	Stage::m_height = 600;

	// Setup clanlib modules:
	CL_SetupCore setup_core;
	CL_SetupDisplay setup_display;
	CL_SetupGL setup_gl;

	// Create a window:
	CL_DisplayWindow window("Hello World", Stage::getWidth(), Stage::getHeight());

	// Retrieve some commonly used objects:
	CL_GraphicContext gc = window.get_gc();
	CL_InputDevice keyboard = window.get_ic().get_keyboard();
	CL_Font font(gc, "Tahoma", 30);

	CL_ResourceManager resources("resources.xml");
	Stage::m_resourceManager = &resources;

	DebugLayer debugLayer;
	Stage::m_debugLayer = &debugLayer;

	RaceScene raceScene;
	Car *car = new Car(50, 50);
	raceScene.getLevel().addCar(car);
	raceScene.getViewport().attachTo(&car->getPosition());
	raceScene.getViewport().setScale(2.0f);

	Client client("localhost", 1234, car);

	unsigned int lastTime = CL_System::get_time();

	// Loop until user hits escape:
	while (!keyboard.get_keycode(CL_KEY_ESCAPE))
	{
		unsigned int delta = CL_System::get_time() - lastTime;
		lastTime += delta;

		if (keyboard.get_keycode(CL_KEY_LEFT) && !keyboard.get_keycode(CL_KEY_RIGHT)) {
			car->setTurn(-1.0f);
		} else if (keyboard.get_keycode(CL_KEY_RIGHT) && !keyboard.get_keycode(CL_KEY_LEFT)) {
			car->setTurn(1.0f);
		} else {
			car->setTurn(0.0f);
		}

		if (keyboard.get_keycode(CL_KEY_UP)) {
			car->setAcceleration(true);
		} else {
			car->setAcceleration(false);
		}

		if (keyboard.get_keycode(CL_KEY_DOWN)) {
			car->setBrake(true);
		} else {
			car->setBrake(false);
		}

		car->update(delta);

		// Draw some text and lines:
		gc.clear(CL_Colorf::cadetblue);

		raceScene.draw(gc);

		// draw debug layer
		debugLayer.draw(gc);

		// Make the stuff visible:
		window.flip();

		// Read messages from the windowing system message queue,
		// if any are available:
		CL_KeepAlive::process();

		// Avoid using 100% CPU in the loop:
		CL_System::sleep(10);
	}

	return 0;
}
