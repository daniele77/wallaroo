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

#ifndef WALLAROO_CLASS_H_
#define WALLAROO_CLASS_H_

#include <string>
#include "detail/factory.h"
#include "cxx0x.h"
#include "device.h"

namespace wallaroo
{

// forward declarations
class Plugin;

/** This is the description of a class having a constructor that 
* takes two parameters of type @c P1 and @c P2 and implements
* the interface @c Device.
* The class provides a method to get an instance of the described class.
* It also provides a static registry of the instances of this class,
* and methods to register and retrieve them.
*/
template < class P1, class P2 >
class Class
{
    public :

        typedef cxx0x::shared_ptr< Device > Ptr;
        typedef cxx0x::function< Ptr( const P1& p1, const P2& p2 ) > FactoryMethod;

        /** Create an instance of the class described by this object.
        * @param p1 The first parameter to pass to the constructor
        * @param p2 The second parameter to pass to the constructor
        * @return a shared_ptr to the new instance (or the empty 
        * shared_ptr if the descriptor is not valid)
        */
        Ptr NewInstance( const P1& p1, const P2& p2 ) const
        {
            if( fm )
                return( fm( p1, p2 ) ) ;
            else
                return( Ptr() ) ;
        }

        /** Return the @c Class< P1, P2 > registered with the name @c name.
        */
        static Class ForName( const std::string& name )
        {
            typename Classes::const_iterator i = Registry().find( name );
            if ( i != Registry().end() )
                return i -> second;
            return Class(); // default value
        }
    private :
        FactoryMethod fm;
        typedef cxx0x::unordered_map< std::string, Class< P1, P2 > > Classes;
        template < class T, class T1, class T2 > friend class Registration;
        static void Register( const std::string& s, const FactoryMethod& m )
        {
            Registry().insert( std::make_pair( s, Class( m ) ) );
        }
        static Classes& Registry()
        {
            static Classes registry;
            return registry;
        }
        Class()
        {
        }
        Class( FactoryMethod m ) :
            fm( m )
        {
        }
};

/** This is the description of a class having a constructor that
* takes one parameter of type @c P and implements
* the interface @c Device.
* The class provides a method to get an instance of the described class.
* It also provides a static registry of the instances of this class,
* and methods to register and retrieve them.
*/
template < class P >
class Class< P, void >
{
    public :

        typedef cxx0x::shared_ptr< Device > Ptr;
        typedef cxx0x::function< Ptr( const P& p ) > FactoryMethod;

        /** Create an instance of the class described by this object.
        * @param p The parameter to pass to the constructor
        * @return a shared_ptr to the new instance (or the empty
        * shared_ptr if the descriptor is not valid)
        */
        Ptr NewInstance( const P& p ) const
        {
            if( fm )
                return( fm( p ) ) ;
            else
                return( Ptr() ) ;
        }

        /** Return the @c Class< P, void > registered with the name @c name.
        */
        static Class ForName( const std::string& name )
        {
            typename Classes::const_iterator i = Registry().find( name );
            if ( i != Registry().end() )
                return i -> second ;
            return Class(); // default value
        }
    private :
        FactoryMethod fm;
        typedef cxx0x::unordered_map< std::string, Class< P, void > > Classes;
        template < class T, class T1, class T2 > friend class Registration;
        static void Register( const std::string& s, const FactoryMethod& m )
        {
            Registry().insert( std::make_pair( s, Class( m ) ) );
        }
        static Classes& Registry()
        {
            static Classes registry;
            return registry;
        }
        Class()
        {
        }
        Class( FactoryMethod m ) :
            fm( m )
        {
        }
};


/** This is the description of a class having a constructor that
* takes no parameters and implements the interface @c Device.
* The class provides a method to get an instance of the described class.
* It also provides a static registry of the instances of this class,
* and methods to register and retrieve them.
*/
template <>
class Class< void, void >
{
    public :

        typedef cxx0x::shared_ptr< Device > Ptr;
        typedef cxx0x::function< Ptr() > FactoryMethod;

        /** Create an instance of the class described by this object.
        * @return a shared_ptr to the new instance (or the empty
        * shared_ptr if the descriptor is not valid)
        */
        Ptr NewInstance() const
        {
            if( fm )
            {
                Ptr p = fm();
                p -> Source( plugin ); // set the ref count to shared library
                return p;
            }
            else
                return( Ptr() );
        }

        /** Return the @c Class< void, void > registered with the name @c name.
        */
        static Class ForName( const std::string& name )
        {
            Classes::const_iterator i = Registry().find( name );
            if ( i != Registry().end() )
                return i -> second ;
            return Class(); // default value
        }
    private :
        FactoryMethod fm;
        cxx0x::shared_ptr< Plugin > plugin; // optional shared ptr to plugin, to release the shared library when is no more used
        typedef cxx0x::unordered_map< std::string, Class< void, void > > Classes;
        template < class T, class T1, class T2 > friend class Registration;
        friend class Plugin;
        static void Register( const std::string& s, const FactoryMethod& m )
        {
            Registry().insert( std::make_pair( s, Class( m ) ) );
        }
        static void Register( const std::string& s, const FactoryMethod& m, const cxx0x::shared_ptr< Plugin >& plugin )
        {
            Registry().insert( std::make_pair( s, Class( m, plugin ) ) );
        }
        static Classes& Registry()
        {
            static Classes registry;
            return registry;
        }
        Class()
        {
        }
        Class( FactoryMethod m ) :
            fm( m )
        {
        }
        Class( FactoryMethod m, const cxx0x::shared_ptr< Plugin >& p ) :
            fm( m ),
            plugin( p )
        {
        }
};


/** This class registers on its constructor a class @c T
*   with its factory method.
*/
template < class T, class P1 = void, class P2 = void >
class Registration
{
public:
    Registration( const std::string& name )
    {
        typename Class< P1, P2 >::FactoryMethod fm( &detail::Factory< T, P1, P2 >::Create );
        Class< P1, P2 >::Register( name, fm );
    }
};

} // wallaroo namespace


/** This macro must be used in your implementation file (.cpp or .cc)
 * to register a class. When a class is registered, you can create an instance
 * using Catalog::Create(const std::string&,const std::string&,const P1&,const P2&).
 * @param C The class name
 * @param ... The type of the other parameters of the class constructor
 * @hideinitializer
 */
#define WALLAROO_REGISTER( C, ... ) \
    static const ::wallaroo::Registration< C, ##__VA_ARGS__ > C##r( #C ) ;
// NOTE: the ## before __VA_ARGS__ removes the comma when no arguments are passed

#endif // WALLAROO_CLASS_H_
