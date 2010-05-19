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

#include "Properties.h"

#include "clanlib/core/io.h"

#include "common.h"

std::map<CL_String, CL_String> Properties::m_keyValueMap;

const bool Properties::NULL_BOOL = false;

const int Properties::NULL_INT = 0;

const CL_String Properties::NULL_CL_STRING = "";

/**
 * Checks if second argument is set to other than default.
 * If it's null object, then error about required p_key is thrown.
 */
template <typename T>
void checkNotNull(
		const T &p_actual,
		const T& p_nullObj,
		const CL_String &p_key
)
{
	if (&p_actual == &p_nullObj) {
		throw CL_Exception(
				cl_format(
						_("required config value '%1' not set"),
						p_key
				)
		);
	}
}


Properties::Properties()
{
	// empty
}

Properties::~Properties()
{
	// empty
}

void Properties::load(const CL_String &p_filename)
{
	try {
		CL_File file(p_filename, CL_File::open_existing, CL_File::access_read);

		CL_DataBuffer buffer(file.get_size());
		file.read(buffer.get_data(), buffer.get_size());

		file.close();

		if (buffer.get_size() != 0) {
			const std::vector<CL_TempString> lines =
					CL_StringHelp::split_text(
							buffer.get_data(), END_OF_LINE, true
					);

			const int linesCount = static_cast<signed>(lines.size());

			std::vector<CL_TempString> parts;

			for (int i = 0; i < linesCount; ++i) {
				parts = CL_StringHelp::split_text(lines[i], "=", false);

				if (parts.size() == 2) {
					const CL_TempString left = CL_StringHelp::trim(parts[0]);
					const CL_TempString right = CL_StringHelp::trim(parts[1]);

					Properties::set(left, right);

					cl_log_event(
							LOG_DEBUG, "Option '%1' set to '%2'", left, right
					);
				} else {
					cl_log_event(LOG_DEBUG, "Cannot parse line: %1", lines[i]);
				}
			}
		}


	} catch (CL_Exception &e) {
		cl_log_event(
				LOG_DEBUG, "File %1 cannot be read: %2", p_filename, e.message
		);
	}

}

void Properties::save(const CL_String &p_filename)
{
	try {
		CL_File file(p_filename, CL_File::create_always, CL_File::access_write);

		std::pair<CL_String, CL_String> pair;
		CL_String line;

		foreach(pair, m_keyValueMap) {
			line = cl_format("%1 = %2%3", pair.first, pair.second, END_OF_LINE);
			file.write(line.c_str(), line.length());

			cl_log_event(
					LOG_DEBUG,
					"Option '%1' saved with value '%2'",
					pair.first,
					pair.second
			);
		}

		file.close();

	} catch (CL_Exception &e) {
		cl_log_event(
				LOG_DEBUG,
				"File %1 cannot be written: %2",
				p_filename,
				e.message
		);
	}
}

void Properties::set(const CL_String &p_key, bool p_value)
{
	set(p_key, CL_StringHelp::bool_to_local8(p_value));
}

void Properties::set(const CL_String &p_key, int p_value)
{
	set(p_key, CL_StringHelp::int_to_local8(p_value));
}

void Properties::set(const CL_String &p_key, const CL_String &p_value)
{
	m_keyValueMap[p_key] = p_value;
}

bool Properties::getBool(const CL_String &p_key, const bool &p_defaultValue)
{
	if (m_keyValueMap.find(p_key) != m_keyValueMap.end()) {
		return CL_StringHelp::local8_to_bool(m_keyValueMap[p_key]);
	} else {
		checkNotNull(p_defaultValue, NULL_BOOL, p_key);
		return p_defaultValue;
	}
}

int Properties::getInt(const CL_String &p_key, const int &p_defaultValue)
{
	if (m_keyValueMap.find(p_key) != m_keyValueMap.end()) {
		return CL_StringHelp::local8_to_int(m_keyValueMap[p_key]);
	} else {
		checkNotNull(p_defaultValue, NULL_INT, p_key);
		return p_defaultValue;
	}
}

CL_String Properties::getString(const CL_String &p_key, const CL_String &p_defaultValue)
{
	if (m_keyValueMap.find(p_key) != m_keyValueMap.end()) {
		return m_keyValueMap[p_key];
	} else {
		checkNotNull(p_defaultValue, NULL_CL_STRING, p_key);
		return p_defaultValue;
	}
}
