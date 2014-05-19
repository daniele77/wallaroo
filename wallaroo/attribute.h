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

#ifndef WALLAROO_ATTRIBUTE_H_
#define WALLAROO_ATTRIBUTE_H_

#include <string>
//#include <typeinfo>
//#include <vector>
#include <sstream> // ### finch� non trovo un modo migliore per la conversione...
#include "cxx0x.h"
//#include "connector.h"
#include "device.h"
#include "typelessattribute.h"
#include "exceptions.h"


namespace wallaroo
{

namespace detail
{
    template < typename T >
    inline void String2Value( const std::string& v, T& value )
    {
        std::istringstream stream( v );
        stream >> value; // ### gestire eccezioni
    }

    template <>
    inline void String2Value< bool >( const std::string& v, bool& value )
    {
        if ( v == "true" ) value = true;
        else if ( v == "false" ) value = false;
        else throw "bad value"; // ### tirare eccezione corretta
    }

    template <>
    inline void String2Value< std::string >( const std::string& v, std::string& value )
    {
        value = v;
    }
}

/**
 * This represents a "attribute" of a "device" that
 * @@@@@@@@@@@@ TODO
 * you can "plug" into another "device".
 *
 * If the device1 has the plug1 plugged to device2, device1 will
 * basically get a pointer to device2.
 *
 * @tparam T The type of the Device contained
 * @tparam P This represents the kind of Plug (@ref mandatory if you must wire a device,
 *           @ref optional if you can leave this plug unwired, 
 *           @ref collection if you can wire many devices to this plug)
 * @tparam Container If P = @ref collection, this represents the std container the Plug will derive from.
 */
template < typename T >
class Attribute : public TypelessAttribute
{
public:

    /** @@@ TODO
    * Create a Plug and register it to its device for future wiring.
    * @param name the name of this plug
    * @param token the registration token got calling Device::RegistrationToken()
    */
    Attribute( const std::string& name, const RegToken& token )
    {
        Device* owner = token.GetDevice();
        owner -> Register( name, this );
    }
#if 0
    /** Plug this plug into a device
    * @param dev The device you want insert this plug into
    * @throw WrongType If @c dev is not a subclass of @c T
    */
    void PlugInto( const cxx0x::shared_ptr< Device >& dev )
    {
        cxx0x::shared_ptr< T > _dev = cxx0x::dynamic_pointer_cast< T >( dev );
        if ( ! _dev ) // bad type!
            throw WrongType();
        else
            device = _dev;
    }

    /** Give access to the embedded device.
    * @throw DeletedDeviceError If the embedded device has been deleted.
    */
    SharedPtr operator -> ()
    {
        SharedPtr result = device.lock();
        if ( ! result ) 
            throw DeletedDeviceError();
        return result;
    }

    /** Give access to the embedded device as const.
    * @throw DeletedDeviceError If the embedded device has been deleted.
    */
    const SharedPtr operator -> () const
    {
        const SharedPtr result = device.lock();
        if ( ! result )
            throw DeletedDeviceError();
        return result;
    }

    /** Convert to a shared ptr.
    * @throw DeletedDeviceError If the embedded device has been deleted.
    */
    operator SharedPtr()
    {
        SharedPtr result = device.lock();
        if ( ! result )
            throw DeletedDeviceError();
        return result;
    }

    /** Convert to a const shared ptr.
    * @throw DeletedDeviceError If the embedded device has been deleted.
    */
    operator const SharedPtr() const
    {
        const SharedPtr result = device.lock();
        if ( ! result )
            throw DeletedDeviceError();
        return result;
    }
#endif

    virtual void Set( const std::string& v )
    {
        detail::String2Value( v, value );
    }

    /* ### TODO commento doxygen */
    operator T () { return value; }

    /** ### TODO commento doxygen
    * Returns true if the plug has been wired and the embedded
    * device has not been deleted.
    * @return true If the embedded device exists.
    */
    operator bool() const
    {
        return !device.expired();
    }

#if 0
   /** Check if this Plug is correctly wired according to the
    * P template parameter policy.
    * @return true If the check pass.
    */
    virtual bool WiringOk() const
    {
        return P::WiringOk( device );
    }
#endif
private:
    typedef cxx0x::weak_ptr< T > WeakPtr;
    WeakPtr device;
    T value;

    // friend operators:
    template < typename T > friend bool operator == ( const Attribute< T >& lhs, const T& rhs );
    template < typename T > friend bool operator < ( const Attribute< T >& lhs, const T& rhs );
};


// binary operators with lhs Attribute<T> and rhs T:

template < typename T > bool operator == ( const Attribute< T >& lhs, const T& rhs ) { return lhs.value == rhs; }
template < typename T > bool operator != ( const Attribute< T >& a, const T& t ) { return !( lhs == rhs ); }

template < typename T > bool operator < ( const Attribute< T >& lhs, const T& rhs ){ return lhs.value < rhs; }
template < typename T > bool operator > ( const Attribute< T >& lhs, const T& rhs ){ return lhs > rhs; }
template < typename T > bool operator <= ( const Attribute< T >& lhs, const T& rhs ){ return !( lhs > rhs ); }
template < typename T > bool operator >= ( const Attribute< T >& lhs, const T& rhs ){ return !( lhs < rhs ); }


// binary operators with lhs T and rhs Attribute<T>:

template < typename T > bool operator == ( const T& lhs, const Attribute< T >& rhs ) { return rhs == lhs; }
template < typename T > bool operator != ( const T& a, const Attribute< T >& t ) { return rhs != lhs; }

template < typename T > bool operator < ( const T& lhs, const Attribute< T >& rhs ){ return rhs > lhs; }
template < typename T > bool operator > ( const T& lhs, const Attribute< T >& rhs ){ return rhs < lhs; }
template < typename T > bool operator <= ( const T& lhs, const Attribute< T >& rhs ){ return !( lhs > rhs ); }
template < typename T > bool operator >= ( const T& lhs, const Attribute< T >& rhs ){ return !( lhs < rhs ); }

}

#endif
