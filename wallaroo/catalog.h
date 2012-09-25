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

#ifndef WALLAROO_CATALOG_H_
#define WALLAROO_CATALOG_H_

#include <string>
#include <map>
#include <typeinfo>
#include <cassert>
#include <stdexcept>
#include "cxx0x.h"
#include "device.h"
#include "class.h"

namespace wallaroo
{
namespace detail
{

class DeviceShell; // forward declaration

// This is a service class that provides the "Into" part in the syntax:
//   catalog[ "source" ].Plug( "plug" ).Into( catalog[ "destination" ] );
// In particular, the method DeviceShell::Plug returns a PlugShell
// that contains the references to the "source" device and its "plug".
class PlugShell
{
public:
    PlugShell( cxx0x::shared_ptr< Device > dev, const std::string& plug ) :
        device( dev ),
        plugName( plug )
    {}
    void Into( DeviceShell destination );
private:
    cxx0x::shared_ptr< Device > device;
    const std::string plugName;
};

// This is a service class that provides the "Plug" part in the syntax:
//   catalog[ "source" ].Plug( "plug" ).Into( catalog[ "destination" ] );
// In particular, the Catalog::operator[] returns a DeviceShell
// that contains the reference to the "source" device.
class DeviceShell
{
public:

    DeviceShell( cxx0x::shared_ptr< Device > dev ) : 
        device( dev ) 
    {
        assert( device );
    }

    PlugShell Plug( const std::string& plugName )
    {
        return PlugShell( device, plugName );
    }

    /** Convert the contained device to the type T
    * @return the converted device.
    * @throw std::bad_cast if the contained device is not a subclass of T
    */
    template < class T >
    operator cxx0x::shared_ptr< T >()
    {
        cxx0x::shared_ptr< T > result = cxx0x::dynamic_pointer_cast< T >( device );
        if ( ! result ) throw std::bad_cast();
        return result;
    }

private:
    friend class PlugShell;
    cxx0x::shared_ptr< Device > device;
};


inline void PlugShell::Into( DeviceShell destination )
{
    device -> Wire( plugName, destination.device );
}


// ###
class Context; // forward declaration

} // namespace detail


/**
 * Catalog of the devices available for the application.
 *
 * It can create the instances from the name of a registered class
 * or add the instances.
 *
 * Each item in the catalog is identified by a @c id, with which
 * you can do a lookup.
 */
class Catalog
{
public:
    /** Look for the element @c itemId in the catalog.
    * @param itemId the name of the element
    * @return the element.
    * @throw std::range_error if the element does not exist in the catalog.
    */
    detail::DeviceShell operator [] ( const std::string& itemId )
    {
        Devices::iterator i = devices.find( itemId );
        if ( i == devices.end() ) throw std::range_error( itemId + " not found in the catalog" );
        return detail::DeviceShell( i -> second );
    }

    /** Add a device to the catalog
    * @param id the name of the device to add
    * @param device the device to add
    * @throw std::range_error if a device with the name @c id is already in the catalog
    */
    void Add( const std::string& id, cxx0x::shared_ptr< Device > dev )
    {
        cxx0x::shared_ptr< Device > toInsert( dev );
        std::pair< Devices::iterator, bool > result = 
            devices.insert( std::make_pair( id, toInsert ) );
        if ( ! result.second )
            throw std::range_error( id + " already in the catalog" );
    }

    /** Instantiate a class with a 2 parameters constructor and add it to the catalog
    * @param id the name of the element to create and add
    * @param className the name of the class to instantiate
    * @param p1 The first parameter of the class constructor
    * @param p2 The second parameter of the class constructor
    * @throw std::range_error if an element with the name @c id is already in the catalog or the @c className class has not been registered
    */
    template < class P1, class P2 >
    void Create( const std::string& id, const std::string& className, const P1& p1, const P2& p2 )
    {
        typedef Class< Device, P1, P2 > C;
        C c = C::ForName( className );
        cxx0x::shared_ptr< Device > obj = c.NewInstance( p1, p2 );
        if ( obj.get() == NULL ) throw std::range_error( className + " not registered" );
        Add( id, obj );
    }

    /** Instantiate a class with a 1 parameters constructor and add it to the catalog
    * @param id the name of the element to create and add
    * @param className the name of the class to instantiate
    * @param p1 The parameter of the class constructor
    * @throw std::range_error if an element with the name @c id is already in the catalog or the @c className class has not been registered
    */
    template < class P >
    void Create( const std::string& id, const std::string& className, const P& p )
    {
        typedef Class< Device, P, void > C;
        C c = C::ForName( className );
        cxx0x::shared_ptr< Device > obj = c.NewInstance( p );
        if ( obj.get() == NULL ) throw std::range_error( className + " not registered" );
        Add( id, obj );
    }

    /** Instantiate a class with 0 parameters constructor and add it to the catalog
    * @param id the name of the element to create and add
    * @param className the name of the class to instantiate
    * @throw std::range_error if an element with the name @c id is already in the catalog or the @c className class has not been registered
    */
    void Create( const std::string& id, const std::string& className )
    {
        typedef Class< Device, void, void > C;
        C c = C::ForName( className );
        cxx0x::shared_ptr< Device > obj = c.NewInstance();
        if ( obj.get() == NULL ) throw std::range_error( className + " not registered" );
        Add( id, obj );
    }

    ////////// ###
    static Catalog*& GetCurrent()
    {
        static Catalog* current = NULL;
        return current;
    }
private:
    typedef std::map< std::string, cxx0x::shared_ptr< Device > > Devices;
    Devices devices;
    // ###
    friend class detail::Context;
    static void SetCurrent( Catalog* c )
    {
        GetCurrent() = c;
    }
};

////////////////////////////////////////////////////////////////////////
// new DSL

namespace detail
{

class UseAsExpression
{
public:
    UseAsExpression( detail::DeviceShell& _destClass, const std::string& _attribute ) :
      destClass( _destClass ),
      attribute( _attribute )
    {
    }
    void of( detail::DeviceShell& srcClass )
    {
        // perform the final assignment:
        srcClass.Plug( attribute ).Into( destClass );
    }
    /*
    * @throw std::range_error if the current catalog has not been selected with @c wallaroo_within
    */
    void of ( const std::string& srcClass )
    {
        // default container case
        Catalog* current = Catalog::GetCurrent();
        if ( ! current ) throw std::range_error( "current catalog not set" );
        of( ( *current )[ srcClass ] );
    }
private:
    detail::DeviceShell destClass;
    std::string attribute;
};

class UseExpression
{
public:
    explicit UseExpression( detail::DeviceShell& _destClass )
        : destClass( _destClass )
    {
    }
    UseAsExpression as( const std::string& attribute )
    {
        return UseAsExpression( destClass, attribute );
    }
private:
    detail::DeviceShell destClass;
};

} // namespace detail

inline detail::UseExpression use( detail::DeviceShell& destClass )
{
    return detail::UseExpression( destClass );
}

/*
* @throw std::range_error if the current catalog has not been selected with @c wallaroo_within
*/
inline detail::UseExpression use( const std::string& destClass )
{
    // default container case
    Catalog* current = Catalog::GetCurrent();
    if ( ! current ) throw std::range_error( "current catalog not set" );
    return use( ( *current )[ destClass ] );
}

////////////////////////////////////////////////////////////////////////

namespace detail
{

class Context
{
public:
    Context( Catalog& c ) :
      firstTime( true )
    {
        previous = Catalog::GetCurrent();
        Catalog::SetCurrent( &c );
    }
    ~Context()
    {
        Catalog::SetCurrent( previous );
    }
    bool FirstTime()
    {
        return firstTime;
    }
    void Terminate()
    {
        firstTime = false;
    }
private:
    bool firstTime;
    Catalog* previous;
};

} // namespace detail

#define wallaroo_within( C ) \
    for ( wallaroo::detail::Context context( C ); context.FirstTime(); context.Terminate() )

////////////////////////////////////////////////////////////////////////

} // namespace

#endif