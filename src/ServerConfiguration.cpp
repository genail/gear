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

#include "ServerConfiguration.h"

#include "common.h"

/* Configuration file location */
const CL_String CONFIG_FILE = "config.xml";

class ServerConfigurationImpl
{
	public:

		/** Server port */
		int m_port;

		/** Level name */
		CL_String m_level;


		ServerConfigurationImpl() :
			m_port(DEFAULT_PORT)
		{
			// try to load server configuration
			load(CONFIG_FILE);
		}


		void load(const CL_String &p_configFile);
};

ServerConfiguration::ServerConfiguration() :
	m_impl(new ServerConfigurationImpl())
{
	// empty
}

ServerConfiguration::~ServerConfiguration()
{
	// empty
}

void ServerConfigurationImpl::load(const CL_String &p_configFile)
{
	try {
		cl_log_event(LOG_DEBUG, "loading configuration from %1", p_configFile);

		CL_File file(
				p_configFile,
				CL_File::open_existing,
				CL_File::access_read
		);


		CL_DomDocument document(file);
		CL_DomElement root = document.get_document_element();

		if (root.named_item("server").is_null()) {
			// no configuration at all
			return;
		}

		CL_DomElement server = root.named_item("server").to_element();

		m_level = server.select_string("level");
		m_port = server.select_int("port");

		if (m_level.length() == 0) {
			cl_log_event(LOG_ERROR, "%1: level not set", CONFIG_FILE);
			exit(1);
		}

		if (m_port <= 0 || m_port > 0xFFFF) {
			cl_log_event(LOG_ERROR, "%1: invalid port value", CONFIG_FILE);
			exit(1);
		}

//		// read all elements
//		CL_DomNode cur = server.get_first_child();
//		while (cur.is_element()) {
//
//			if (cur.get_node_name() == "port") {
//				m_port = CL_StringHelp::local8_to_int
//						(cur.to_element().get_text()
//				);
//				cl_log_event(LOG_DEBUG, "Port set to %1", m_port);
//			}
//
//			cur = cur.get_next_sibling();
//		}


	} catch (CL_Exception e) {
		cl_log_event(LOG_ERROR, e.message);
	}
}

const CL_String &ServerConfiguration::getLevel() const
{
	return m_impl->m_level;
}

int ServerConfiguration::getPort() const
{
	return m_impl->m_port;
}
