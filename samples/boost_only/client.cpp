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

#include <iostream>
#include "client.h"

WALLAROO_REGISTER( Client ) // register this class

Client::Client() :
    // assign a name to each dependency, for the wiring phase
    relation( "relation", RegistrationToken() ),
    optionalRelation( "optionalRelation", RegistrationToken() ),
    relationVector( "relationVector", RegistrationToken() ),
    relationList( "relationList", RegistrationToken() ),
    relationBoundedVector( "relationBoundedVector", RegistrationToken() )
{
    std::cout << this << " Client::Client()" << std::endl;
}

void Client::G()
{
    std::cout << this << " Client::G()" << std::endl;

    relation -> F();

    typedef Collaborator< Interface, collection< std::list > >::iterator Iterator;
    for (  Iterator i = relationList.begin(); i != relationList.end(); ++i )
    {
        cxx0x::shared_ptr< Interface > s = i -> lock();
        if ( s ) 
            s -> F();
        else
            std::cerr << "an element in relationList has been deleted!" << std::endl;
    }

    for ( std::size_t i = 0; i < relationVector.size(); ++i )
    {
        cxx0x::shared_ptr< Interface > s = relationVector[ i ].lock();
        if ( s ) 
            s -> F();
        else
            std::cerr << "an element in relationVector has been deleted!" << std::endl;
    }
}

Client::~Client()
{
}

