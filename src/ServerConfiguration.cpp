/*
 * ServerConfigration.cpp
 *
 *  Created on: 2009-10-11
 *      Author: chudy
 */

#include "ServerConfiguration.h"

#include "common.h"

/* Configuration file location */
const CL_String CONFIG_FILE = "config.xml";
/* Default server port */
const int DEFAULT_PORT = 2500;

ServerConfiguration::ServerConfiguration() :
	m_port(DEFAULT_PORT)
{
	// try to load server configuration
	load(CONFIG_FILE);
}

ServerConfiguration::~ServerConfiguration()
{
}

void ServerConfiguration::load(const CL_String &p_configFile)
{
	try {

		cl_log_event("config", "Loading configuration from %1", p_configFile);

		CL_File file(p_configFile, CL_File::open_existing);


		CL_DomDocument document(file);
		CL_DomElement root = document.get_document_element();

		if (root.named_item("server").is_null()) {
			// no configuration at all
			return;
		}

		CL_DomElement server = root.named_item("server").to_element();


		// read all elements
		CL_DomNode cur = server.get_first_child();
		while (cur.is_element()) {

			if (cur.get_node_name() == "port") {
				m_port = CL_StringHelp::local8_to_int(cur.to_element().get_text());
				cl_log_event("config", "Port set to %1", m_port);
			}

			cur = cur.get_next_sibling();
		}


	} catch (CL_Exception e) {
		cl_log_event("exception", e.message);
	}
}

int ServerConfiguration::getPort() const
{
	return m_port;
}
