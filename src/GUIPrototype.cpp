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

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/gl.h>
#include <ClanLib/application.h>
#include <ClanLib/gui.h>
#include <stdlib.h>


class GUI
{
	public:
		static int main(const std::vector<CL_String> &args);

		void on_window_close() { exit(0); }
};

CL_ClanApplication app(&GUI::main);

int GUI::main(const std::vector<CL_String> &args)
{
	GUI gui_;

	// init
	CL_SetupCore setup_core;
	CL_SetupDisplay setup_display;
	CL_SetupGL setup_gl;
	CL_SetupGUI setup_gui;

	CL_DisplayWindowDescription win_desc;
	win_desc.set_allow_resize(true);
	win_desc.set_title("GUI Example Application");
//	win_desc.set_size(CL_Size( 1100, 900 ), false);
	win_desc.set_layered(false);


//	CL_DisplayWindow window = CL_DisplayWindow(win_desc);	// Don't destroy the window first, so the shared gc data is not lost
//	window.sig_window_close().connect(&gui_, &GUI::on_window_close);

	CL_GUIManager gui;
	CL_GUIThemeDefault theme;
	CL_GUIWindowManagerSystem window_manager;

	gui.set_window_manager(window_manager);

	CL_ResourceManager resources("/home/chudy/projekty/clanlib/ClanLib-2.0.4/Resources/GUIThemeAero/resources.xml");
	CL_CSSDocument css_document;
	css_document.load("/home/chudy/projekty/clanlib/ClanLib-2.0.4/Resources/GUIThemeAero/theme.css");

	gui.set_css_document(css_document);
	theme.set_resources(resources);
	gui.set_theme(theme);

	// Create components:
	CL_GUITopLevelDescription desc;
	desc.set_title("ClanLib GUI");
	CL_Window w(&gui, desc);

	CL_Label nameLabel(&w);
	nameLabel.set_geometry(CL_Rect(100, 100, 180, 120));
	nameLabel.set_text("Player's name");

	CL_LineEdit nameLineEdit(&w);
	nameLineEdit.set_geometry(CL_Rect(200, 100, 400, 120));

	CL_Label serverLabel(&w);
	serverLabel.set_geometry(CL_Rect(100, 140, 180, 160));
	serverLabel.set_text("Server addr");

	CL_LineEdit serverLineEdit(&w);
	serverLineEdit.set_geometry(CL_Rect(200, 140, 400, 160));

//	CL_PushButton button1(&w);
//	button1.set_geometry(CL_Rect(100, 100, 200, 120));
//	button1.set_text("Okay!");
//	button1.func_clicked().set(&on_button1_clicked, &button1);

	gui.exec(true);


//	while (true) {
//
////		CL_GUIMessage message = gui.get
////		gui_manager.dispatch_message(message);
////		if (!message.is_consumed()) {
////			accelerator_table.process_message(message);
////		}
//		CL_KeepAlive::process();
//
//
//	    CL_System::sleep(5);
//	}

	return 0;
}
