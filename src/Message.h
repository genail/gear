/*
 * Message.h
 *
 *  Created on: 2009-09-26
 *      Author: chudy
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <iostream>

#include "Debug.h"

class Message {
	public:
		virtual ~Message() {}

		static std::ostream &out() { return m_out; }
		static std::ostream &err() { return m_err; }

	private:
		Message();

		static std::ostream &m_out;
		static std::ostream &m_err;
};

#endif /* MESSAGE_H_ */
