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

#ifndef WALLAROO_DETAIL_DYNAMIC_LIBRARY_UNIX_H_
#define WALLAROO_DETAIL_DYNAMIC_LIBRARY_UNIX_H_

#include <string>
#include <dlfcn.h>


namespace wallaroo
{
namespace detail
{

// unix-like specific implementation of shared library
class PlatformSpecificDynamicLibrary
{
public:
    // throw WrongFile if the file does not exist or its format is wrong.
    explicit PlatformSpecificDynamicLibrary( const std::string& fileName )
    {
        libHandle = dlopen( fileName.c_str(), RTLD_LAZY | RTLD_GLOBAL );
        if ( ! libHandle ) throw WrongFile( fileName );
    }
    // Release the OS library
    ~PlatformSpecificDynamicLibrary()
    {
        dlclose( libHandle );
    }
    // Return a function pointer to the symbol funcName. NULL if the symbol was not found.
    template < typename F >
    F GetFunction( const std::string& funcName )
    {
        F f = (F)dlsym( libHandle, funcName.c_str() );
        if ( dlerror() != NULL ) return NULL;
        return f;
    }
    /* Returns the unix filename suffix
	   for shared libraries (including the period).
	   In debug mode, the suffix also includes a
	   "d" to specify the debug version of a library. */
    static std::string Suffix()
    {
        #if defined(__APPLE__)
	        #if defined(_DEBUG)
		        return "d.dylib";
	        #else
		        return ".dylib";
	        #endif
        #elif defined(hpux) || defined(_hpux)
	        #if defined(_DEBUG)
		        return "d.sl";
	        #else
		        return ".sl";
	        #endif
        #elif defined(__CYGWIN__)
	        #if defined(_DEBUG)
		        return "d.dll";
	        #else
		        return ".dll";
	        #endif
        #else
	        #if defined(_DEBUG)
		        return "d.so";
	        #else
		        return ".so";
	        #endif
        #endif
    }
private:
    void* libHandle;
};

} // namespace detail
} // namespace wallaroo

#endif // WALLAROO_DETAIL_DYNAMIC_LIBRARY_UNIX_H_
