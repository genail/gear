/*
 * Debug.h
 *
 *  Created on: 2009-09-25
 *      Author: chudy
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <iostream>

class Debug {
	public:
		virtual ~Debug() {}

		static std::ostream &out() {
			m_out << "DEBUG: ";
			return m_out;
		}

		static std::ostream &err() {
			m_err << "DEBUG: ";
			return m_err;
		}

	private:
		Debug();

		static std::ostream &m_out;
		static std::ostream &m_err;
};

#endif /* DEBUG_H_ */
