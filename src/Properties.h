/*
 * Properties.h
 *
 *  Created on: 2009-09-20
 *      Author: chudy
 */

#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include <map>
#include <ClanLib/core.h>

class Properties {

	public:

		static void setProperty(const CL_String8 &p_key, bool p_value) { setProperty(p_key, CL_StringHelp::bool_to_local8(p_value)); }
		static void setProperty(const CL_String8 &p_key, int p_value) { setProperty(p_key, CL_StringHelp::int_to_local8(p_value)); }
		static void setProperty(const CL_String8 &p_key, const CL_String8 &p_value) { m_keyValueMap[p_key] = p_value; }


		static bool getPropertyAsBool(const CL_String8 &p_key) { return CL_StringHelp::local8_to_bool(getPropertyAsString(p_key)); }
		static int getPropertyAsInt(const CL_String8 &p_key) { return CL_StringHelp::local8_to_int(getPropertyAsString(p_key)); }
		static CL_String8 getPropertyAsString(const CL_String8 &p_key) { return m_keyValueMap[p_key]; }

	private:
		static std::map<CL_String8, CL_String8> m_keyValueMap;

		Properties();
		virtual ~Properties();
};

#endif /* PROPERTIES_H_ */
