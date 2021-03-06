/*******************************************************************************
 * wallaroo - A library for configurable creation and wiring of C++ classes.
 * Copyright (C) 2012 Daniele Pallastrelli
 *
 * This file is part of wallaroo.
 * For more information, see http://wallaroolib.sourceforge.net/
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#include <sstream>
#include "wallaroo/dynamic_lib.h"
#include "wallaroo/dyn_registered.h"
#include "straightbetconsolefactory.h"
#include "straightbet.h"

WALLAROO_DYNLIB_REGISTER( StraightBetConsoleFactory );

StraightBetConsoleFactory::~StraightBetConsoleFactory()
{
}

std::string StraightBetConsoleFactory::Help() const
{
    return "straight <bin> <amount>";
}

cxx0x::shared_ptr< Bet > StraightBetConsoleFactory::Create( const std::string& cmdLine ) const
{
    using namespace std;

    string cmdName;
    string inputBin;
    Currency amount;

    stringstream ss( cmdLine );

    if ( ss.eof() ) return cxx0x::shared_ptr< Bet >();
    ss >> cmdName;
    if ( cmdName != "straight" ) return cxx0x::shared_ptr< Bet >();

    if ( ss.eof() ) return cxx0x::shared_ptr< Bet >();
    ss >> inputBin;
    
    if ( ss.eof() ) return cxx0x::shared_ptr< Bet >();
    ss >> amount;
    if ( ss.fail() ) return cxx0x::shared_ptr< Bet >();
    if ( !ss.eof() ) return cxx0x::shared_ptr< Bet >();
    if ( amount == 0 ) return cxx0x::shared_ptr< Bet >();

    Bin bin( StringToBin( inputBin ) );
    return cxx0x::shared_ptr< Bet >( new StraightBet( bin, amount ) );
}

