/*
 * ServerConfigration.h
 *
 *  Created on: 2009-10-11
 *      Author: chudy
 */

#ifndef SERVERCONFIGURATION_H_
#define SERVERCONFIGURATION_H_

#include <ClanLib/core.h>

class ServerConfiguration {

	public:

		ServerConfiguration();

		virtual ~ServerConfiguration();

		int getPort() const;

	private:

		/** Server port */
		int m_port;

		void load(const CL_String &p_configFile);
};

#endif /* SERVERCONFIGURATION_H_ */
