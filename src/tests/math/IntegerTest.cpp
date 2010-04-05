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

#include <unistd.h>
#include <boost/test/unit_test.hpp>

#include <ClanLib/core.h>

#include "math/Integer.h"

/*
 * Minimal testing facility:
 *
 * BOOST_CHECK( predicate )
 * BOOST_REQUIRE( predicate )
 * BOOST_ERROR( message )
 * BOOST_FAIL( message )
 *
 * Test tools:
 * http://www.boost.org/doc/libs/1_34_0/libs/test/doc/components/test_tools/index.html
 */

BOOST_AUTO_TEST_SUITE(IntegerTest)

BOOST_AUTO_TEST_CASE(clamp)
{
	BOOST_CHECK(Math::Integer::clamp(-2, 0, 0) == 0);
	BOOST_CHECK(Math::Integer::clamp(-1, 0, 0) == 0);
	BOOST_CHECK(Math::Integer::clamp(0, 0, 0) == 0);
	BOOST_CHECK(Math::Integer::clamp(1, 0, 0) == 0);
	BOOST_CHECK(Math::Integer::clamp(2, 0, 0) == 0);

	BOOST_CHECK(Math::Integer::clamp(-3, 0, 1) == 1);
	BOOST_CHECK(Math::Integer::clamp(-2, 0, 1) == 0);
	BOOST_CHECK(Math::Integer::clamp(-1, 0, 1) == 1);
	BOOST_CHECK(Math::Integer::clamp(0, 0, 1) == 0);
	BOOST_CHECK(Math::Integer::clamp(1, 0, 1) == 1);
	BOOST_CHECK(Math::Integer::clamp(2, 0, 1) == 0);
	BOOST_CHECK(Math::Integer::clamp(3, 0, 1) == 1);
	BOOST_CHECK(Math::Integer::clamp(4, 0, 1) == 0);

	BOOST_CHECK(Math::Integer::clamp(-4, 0, 2) == 2);
	BOOST_CHECK(Math::Integer::clamp(-3, 0, 0) == 0);
	BOOST_CHECK(Math::Integer::clamp(-2, 0, 2) == 1);
	BOOST_CHECK(Math::Integer::clamp(-1, 0, 2) == 2);
	BOOST_CHECK(Math::Integer::clamp(0, 0, 2) == 0);
	BOOST_CHECK(Math::Integer::clamp(1, 0, 2) == 1);
	BOOST_CHECK(Math::Integer::clamp(2, 0, 2) == 2);
	BOOST_CHECK(Math::Integer::clamp(3, 0, 2) == 0);
	BOOST_CHECK(Math::Integer::clamp(4, 0, 2) == 1);
}

BOOST_AUTO_TEST_CASE(toHex)
{
	BOOST_CHECK_EQUAL(Math::Integer::toHex(0).c_str(), "0");
	BOOST_CHECK_EQUAL(Math::Integer::toHex(1).c_str(), "1");
	BOOST_CHECK_EQUAL(Math::Integer::toHex(10).c_str(), "A");
	BOOST_CHECK_EQUAL(Math::Integer::toHex(15).c_str(), "F");
	BOOST_CHECK_EQUAL(Math::Integer::toHex(16).c_str(), "10");
	BOOST_CHECK_EQUAL(Math::Integer::toHex(255).c_str(), "FF");
	BOOST_CHECK_EQUAL(Math::Integer::toHex(256).c_str(), "100");
}

BOOST_AUTO_TEST_CASE(fromHex)
{
	BOOST_CHECK_EQUAL(Math::Integer::fromHex("0"), 0);
	BOOST_CHECK_EQUAL(Math::Integer::fromHex("1"), 1);
	BOOST_CHECK_EQUAL(Math::Integer::fromHex("A"), 10);
	BOOST_CHECK_EQUAL(Math::Integer::fromHex("F"), 15);
	BOOST_CHECK_EQUAL(Math::Integer::fromHex("10"), 16);
	BOOST_CHECK_EQUAL(Math::Integer::fromHex("FF"), 255);
	BOOST_CHECK_EQUAL(Math::Integer::fromHex("100"), 256);
}

BOOST_AUTO_TEST_SUITE_END()
