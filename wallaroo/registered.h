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

#ifndef WALLAROO_REGISTEREDCLASS_H_
#define WALLAROO_REGISTEREDCLASS_H_

// facilities to define a wallaroo class
#include "class.h"
#include "collaborator.h"
#include "attribute.h"
#include "part.h"

/// @cond DOC_WALLAROO_TOKENPASTE
#define WALLAROO_TOKENPASTE_HELPER(x, y) x ## y
#define WALLAROO_TOKENPASTE(x, y) WALLAROO_TOKENPASTE_HELPER(x, y)
/// @endcond

/** This macro must be used in your implementation file (.cpp or .cc)
* to register a class. When a class is registered, you can create an instance
* using Catalog::Create(const std::string&,const std::string&,const P1&,const P2&).
* @param C The class name
* @param ... The type of the other parameters of the class constructor
* @hideinitializer
*/
#define WALLAROO_REGISTER( C, ... ) \
    static const ::wallaroo::Registration< C, ##__VA_ARGS__ > WALLAROO_TOKENPASTE(__reg__,__LINE__)( #C ) ;
// NOTE: the ## before __VA_ARGS__ removes the comma when no arguments are passed

#endif
