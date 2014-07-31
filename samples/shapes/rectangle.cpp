/*******************************************************************************
 * wallaroo - A library for configurable creation and wiring of C++ classes.
 * Copyright (C) 2012 Daniele Pallastrelli
 *
 * This file is part of wallaroo.
 * For more information, see http://wallaroo.googlecode.com/
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

#include "rectangle.h"

WALLAROO_REGISTER( Rectangle );

Rectangle::Rectangle() :
    width( "width", RegistrationToken() ),
    height( "height", RegistrationToken() ),
    xPos( 0 ),
    yPos( 0 )
{
}

Box Rectangle::BoundingBox() const
{
    return Box( xPos, yPos, xPos + width, yPos + height );
}

void Rectangle::Draw( cxx0x::shared_ptr< Canvas > canvas ) const
{
    for ( unsigned int x = xPos; x <= xPos + width; ++x )
    {
        canvas -> Point( x, yPos );
        canvas -> Point( x, yPos + height );
    }
    for ( unsigned int y = yPos; y <= yPos + height; ++y )
    {
        canvas -> Point( xPos, y );
        canvas -> Point( xPos + width, y );
    }
}

void Rectangle::MoveX( int delta )
{
    xPos += delta;
}

void Rectangle::MoveY( int delta )
{
    yPos += delta;
}
