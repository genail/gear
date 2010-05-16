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

#include "PlayOnlineScene.h"

#include <ClanLib/gui.h>

#include "common.h"
#include "controllers/PlayOnlineSceneController.h"
#include "gfx/widgets/Header.h"

static const int MARGIN = 10;

const CL_String COL_NAME = "name";
const CL_String COL_GAMEMODE = "gamemode";
const CL_String COL_MAP = "map";
const CL_String COL_PLAYERS = "players";
const CL_String COL_PING = "ping";

class PlayOnlineSceneImpl
{
	public:

		PlayOnlineScene *m_parent;

		Gfx::Header m_sceneTitleHeader;
		CL_Label m_statusLabel;
		CL_ListView m_serverList;

		CL_PushButton m_mainMenuButton;
		CL_PushButton m_refreshButton;
		CL_PushButton m_connectButton;

		PlayOnlineSceneController m_controller;

		CL_Callback_v1<const PlayOnlineScene::Entry&> m_serverEntrySelected;


		PlayOnlineSceneImpl(PlayOnlineScene *p_parent);
		~PlayOnlineSceneImpl();

		void configureListViewWidget();
		void configureButtons();

		void addServerEntry(const PlayOnlineScene::Entry &p_entry);
		void clearServerEntries();

		void onServerListSelectionChanged(CL_ListViewSelection p_selection);
};

PlayOnlineScene::PlayOnlineScene(CL_GUIComponent *p_parent) :
		GuiScene(p_parent),
		m_impl(new PlayOnlineSceneImpl(this))
{
	m_impl->m_controller.initialize();
}

PlayOnlineSceneImpl::PlayOnlineSceneImpl(PlayOnlineScene *p_parent) :
		m_parent(p_parent),
		m_sceneTitleHeader(p_parent),
		m_statusLabel(p_parent),
		m_serverList(p_parent),
		m_mainMenuButton(p_parent),
		m_refreshButton(p_parent),
		m_connectButton(p_parent),
		m_controller(p_parent)
{
	const int stageWidth = m_parent->get_width();

	m_sceneTitleHeader.set_geometry(CL_Rect(10, 30, 300, 80));
	m_sceneTitleHeader.setText(_("Play Online"));
	m_sceneTitleHeader.setTextSize(58);

	m_statusLabel.set_geometry(CL_Rect(stageWidth - 300, 80, stageWidth, 100));
	m_statusLabel.set_text(_("Press the refresh button"));

	configureListViewWidget();
	configureButtons();

	m_serverList.func_selection_changed().set(
			this, &PlayOnlineSceneImpl::onServerListSelectionChanged);
}

void PlayOnlineSceneImpl::configureListViewWidget()
{
	static const float COL_NAME_SIZE = 0.4f;
	static const float COL_GAMEMODE_SIZE = 0.2f;
	static const float COL_MAP_SIZE = 0.2f;
	static const float COL_PLAYERS_SIZE = 0.1f;
	static const float COL_PING_SIZE = 0.1f;

	static const int BOTTOM_MARGIN = 50;

	const int stageWidth = m_parent->get_width();
	const int stageHeight = m_parent->get_height();
	const int listWidth = stageWidth - 2 * MARGIN;

	m_serverList.set_geometry(CL_Rect(10, 100, stageWidth - MARGIN, stageHeight - BOTTOM_MARGIN));
	m_serverList.set_display_mode(listview_mode_details);
	m_serverList.set_select_whole_row(true);

	// add headers
	CL_ListViewHeader *header = m_serverList.get_header();

	CL_ListViewColumnHeader colName = header->create_column(COL_NAME, _("Server name"));
	colName.set_width(listWidth * COL_NAME_SIZE);
	header->append(colName);

	CL_ListViewColumnHeader colGameMode = header->create_column(COL_GAMEMODE, _("Gamemode"));
	colGameMode.set_width(listWidth * COL_GAMEMODE_SIZE);
	header->append(colGameMode);

	CL_ListViewColumnHeader colMap = header->create_column(COL_MAP, _("Map"));
	colMap.set_width(listWidth * COL_MAP_SIZE);
	header->append(colMap);

	CL_ListViewColumnHeader colPlayers = header->create_column(COL_PLAYERS, _("Players"));
	colPlayers.set_width(listWidth * COL_PLAYERS_SIZE);
	header->append(colPlayers);

	CL_ListViewColumnHeader colPing = header->create_column(COL_PING, _("Ping"));
	colPing.set_width(listWidth * COL_PING_SIZE);
	header->append(colPing);
}

void PlayOnlineSceneImpl::configureButtons()
{
	const int stageWidth = m_parent->get_width();
	const int stageHeight = m_parent->get_height();

	static const int BUTTON_HEIGHT = 30;
	static const int BUTTON_WIDTH = 100;
	const int buttonTop = stageHeight - MARGIN - BUTTON_HEIGHT;
	const int buttonBottom = stageHeight - MARGIN;


	CL_Rect mainMenuButtonRect(MARGIN, buttonTop, MARGIN + BUTTON_WIDTH, buttonBottom);
	m_mainMenuButton.set_geometry(mainMenuButtonRect);
	m_mainMenuButton.set_text(_("Main menu"));

	CL_Rect refreshButtonRect(
			stageWidth - (MARGIN + BUTTON_WIDTH) * 2, buttonTop,
			stageWidth - (MARGIN * 2 + BUTTON_WIDTH), buttonBottom);
	m_refreshButton.set_geometry(refreshButtonRect);
	m_refreshButton.set_text(_("Refresh"));

	CL_Rect connectButtonRect(
			stageWidth - (MARGIN + BUTTON_WIDTH), buttonTop,
			stageWidth - MARGIN, buttonBottom);
	m_connectButton.set_geometry(connectButtonRect);
	m_connectButton.set_text(_("Connect"));
}

PlayOnlineScene::~PlayOnlineScene()
{
	// empty
}

PlayOnlineSceneImpl::~PlayOnlineSceneImpl()
{
	// empty
}

void PlayOnlineScene::draw(CL_GraphicContext &p_gc)
{
	CL_Draw::fill(p_gc, 0.0f, 0.0f, get_width(), get_height(), CL_Colorf::white);
}

void PlayOnlineScene::addServerEntry(const Entry &p_entry)
{
	m_impl->addServerEntry(p_entry);
}

void PlayOnlineSceneImpl::addServerEntry(const PlayOnlineScene::Entry &p_entry)
{
	CL_ListViewItem item = m_serverList.create_item();
	item.set_column_text(COL_NAME, p_entry.serverName);
	item.set_column_text(COL_GAMEMODE, p_entry.gamemode);
	item.set_column_text(COL_MAP, p_entry.mapName);
	item.set_column_text(
			COL_PLAYERS, cl_format("%1/%2", p_entry.playerCountCurrent, p_entry.playerCountLimit));
	item.set_column_text(COL_PING, CL_StringHelp::int_to_text(p_entry.ping));

	CL_ListViewItem docItem = m_serverList.get_document_item();
	docItem.append_child(item);

	item.set_userdata(CL_SharedPtr<PlayOnlineScene::Entry>(new PlayOnlineScene::Entry(p_entry)));
}

void PlayOnlineScene::clearServerEntries()
{
	m_impl->clearServerEntries();
}

void PlayOnlineSceneImpl::clearServerEntries()
{
	CL_ListViewItem docItem = m_serverList.get_document_item();
	docItem.remove_children();
}

void PlayOnlineScene::setStatusText(const CL_String &p_statusText)
{
	m_impl->m_statusLabel.set_text(p_statusText);
}

void PlayOnlineSceneImpl::onServerListSelectionChanged(CL_ListViewSelection p_selection)
{
	CL_ListViewSelectedItem selectedItem = p_selection.get_first();

	if (!selectedItem.is_null()) {
		CL_ListViewItem item = selectedItem.get_item();
		PlayOnlineScene::Entry *serverEntry =
				reinterpret_cast<PlayOnlineScene::Entry*>(item.get_userdata().get());

		m_serverEntrySelected.invoke(*serverEntry);
	}
}

CL_Callback_v0 &PlayOnlineScene::refreshButtonClicked()
{
	return m_impl->m_refreshButton.func_clicked();
}

CL_Callback_v0 &PlayOnlineScene::mainMenuButtonClicked()
{
	return m_impl->m_mainMenuButton.func_clicked();
}

CL_Callback_v0 &PlayOnlineScene::connectButtonClicked()
{
	return m_impl->m_connectButton.func_clicked();
}

CL_Callback_v1<const PlayOnlineScene::Entry&> &PlayOnlineScene::serverEntrySelected()
{
	return m_impl->m_serverEntrySelected;
}
