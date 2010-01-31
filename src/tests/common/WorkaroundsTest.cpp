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

#include "common/workarounds.h"

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

BOOST_AUTO_TEST_SUITE(WorkaroundsTest)

BOOST_AUTO_TEST_CASE(clAngleNormalize)
{
	static const float ACC = 0.001f;

	CL_Angle a(0.0f, cl_degrees);
	BOOST_REQUIRE_CLOSE(a.to_degrees(), 0.0f, ACC);

	Workarounds::clAngleNormalize(&a);
	BOOST_CHECK_CLOSE(a.to_degrees(), 0.0f, ACC);


	a.set_degrees(180.0f);
	BOOST_REQUIRE_CLOSE(a.to_degrees(), 180.0f, ACC);

	Workarounds::clAngleNormalize(&a);
	BOOST_CHECK_CLOSE(a.to_degrees(), 180.0f, ACC);


	a.set_degrees(370.0f);
	BOOST_REQUIRE_CLOSE(a.to_degrees(), 370.0f, ACC);

	Workarounds::clAngleNormalize(&a);
	BOOST_CHECK_CLOSE(a.to_degrees(), 10.0f, ACC);


	a.set_degrees(360.0f + 180.0f);
	BOOST_REQUIRE_CLOSE(a.to_degrees(), 360.0f + 180.0f, ACC);

	Workarounds::clAngleNormalize(&a);
	BOOST_CHECK_CLOSE(a.to_degrees(), 180.0f, ACC);


	a.set_degrees(730.0f);
	BOOST_REQUIRE_CLOSE(a.to_degrees(), 730.0f, ACC);

	Workarounds::clAngleNormalize(&a);
	BOOST_CHECK_CLOSE(a.to_degrees(), 10.0f, ACC);


	a.set_degrees(-10.0f);
	BOOST_REQUIRE_CLOSE(a.to_degrees(), -10.0f, ACC);

	Workarounds::clAngleNormalize(&a);
	BOOST_CHECK_CLOSE(a.to_degrees(), 350.0f, ACC);


	a.set_degrees(-370.0f);
	BOOST_REQUIRE_CLOSE(a.to_degrees(), -370.0f, ACC);

	Workarounds::clAngleNormalize(&a);
	BOOST_CHECK_CLOSE(a.to_degrees(), 350.0f, ACC);
}

BOOST_AUTO_TEST_CASE(clAngleNormalize180)
{
	static const float ACC = 0.001f;

	CL_Angle a(0.0f, cl_degrees);
	BOOST_REQUIRE_CLOSE(a.to_degrees(), 0.0f, ACC);

	Workarounds::clAngleNormalize180(&a);
	BOOST_CHECK_CLOSE(a.to_degrees(), -180.0f, ACC);


	a.set_degrees(180.0f);
	BOOST_REQUIRE_CLOSE(a.to_degrees(), 180.0f, ACC);

	Workarounds::clAngleNormalize180(&a);
	BOOST_CHECK_CLOSE(a.to_degrees(), 0.0f, ACC);


	a.set_degrees(360.0f + 180.0f);
	BOOST_REQUIRE_CLOSE(a.to_degrees(), 360.0f + 180.0f, ACC);

	Workarounds::clAngleNormalize180(&a);
	BOOST_CHECK_CLOSE(a.to_degrees(), 0.0f, ACC);


	a.set_degrees(-10.0f);
	BOOST_REQUIRE_CLOSE(a.to_degrees(), -10.0f, ACC);

	Workarounds::clAngleNormalize180(&a);
	BOOST_CHECK_CLOSE(a.to_degrees(), 170.0f, ACC);
}

BOOST_AUTO_TEST_SUITE_END()
