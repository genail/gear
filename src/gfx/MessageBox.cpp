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

#include "MessageBox.h"

#include "gfx/Stage.h"

namespace Gfx
{

const int MESSAGE_BOX_WIDTH = 400;
const int MESSAGE_BOX_HEIGHT = 60;
const int MESSAGE_BOX_X_OFFSET = -MESSAGE_BOX_WIDTH / 2;
const int MESSAGE_BOX_Y_OFFSET = -MESSAGE_BOX_HEIGHT / 2;

class MessageBoxImpl
{
	public:

		CL_GUIComponent *m_owner;

		CL_Window *m_window;

		CL_Label *m_label;

		CL_String m_message;


		MessageBoxImpl(CL_GUIComponent *p_owner);

		void createWindow();

		void createLabel();

		~MessageBoxImpl();
};


MessageBox::MessageBox(CL_GUIComponent *p_owner) :
		m_impl(new MessageBoxImpl(p_owner))
{
	// empty
}

MessageBox::~MessageBox()
{
	// empty
}

MessageBoxImpl::MessageBoxImpl(CL_GUIComponent *p_owner) :
		m_owner(p_owner)
{
	createWindow();
	createLabel();
}

void MessageBoxImpl::createWindow()
{
	CL_Rect windowGeometry(
			Stage::getWidth() / 2 + MESSAGE_BOX_X_OFFSET,
			Stage::getHeight() / 2 + MESSAGE_BOX_Y_OFFSET,
			CL_Size(
					MESSAGE_BOX_WIDTH,
					MESSAGE_BOX_HEIGHT
			)
	);

	CL_GUITopLevelDescription windowDescription("MessageBox", windowGeometry, false);

	m_window = new CL_Window(m_owner, windowDescription);
	m_window->set_visible(false);
}

void MessageBoxImpl::createLabel()
{
	static const int MARGIN = 5;
	CL_Rect clientArea = m_window->get_client_area();

	m_label = new CL_Label(m_window);
	m_label->set_geometry(
			CL_Rect(
					clientArea.left + MARGIN,
					clientArea.top + MARGIN,
					clientArea.right - MARGIN,
					clientArea.bottom - MARGIN
			)
	);

	m_label->set_alignment(CL_Label::align_center);
	m_label->set_text("no message set");
}

MessageBoxImpl::~MessageBoxImpl()
{
	delete m_label;
	delete m_window;
}

void MessageBox::setMessage(const CL_String &p_message)
{
	m_impl->m_label->set_text(p_message);
}

void MessageBox::show()
{
	m_impl->m_window->set_visible(true);
}

void MessageBox::hide()
{
	m_impl->m_window->set_visible(false);
}

}
