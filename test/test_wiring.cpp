/*******************************************************************************
 * wallaroo - A library for configurable creation and wiring of C++ classes
 * Copyright (C) 2012 Daniele Pallastrelli 
 *
 *
 * This file is part of wallaroo.
 * For more information, see http://wallaroo.googlecode.com/
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 ******************************************************************************/

//#define BOOST_TEST_MODULE WiringTest
//#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp>

#include "wallaroo/registered.h"
#include "wallaroo/catalog.h"

using namespace wallaroo;

// some classes:

REGISTERED_CLASS( AA, void, void ), public Device
{
public:
    virtual int F() { return 5; }
    virtual ~AA() {}
};

REGISTER( AA, void, void )

REGISTERED_CLASS( BB, void, void ), public Device
{
public:
    virtual void F() {}
    virtual ~BB() {}
};

REGISTER( BB, void, void )

// tests

BOOST_AUTO_TEST_SUITE( Wiring )

BOOST_AUTO_TEST_CASE( wiringOk )
{
    Catalog catalog;
    BOOST_REQUIRE_NO_THROW( catalog.Create( "a", "AA" ) );
    BOOST_REQUIRE_NO_THROW( catalog[ "a" ] );
}
BOOST_AUTO_TEST_SUITE_END()