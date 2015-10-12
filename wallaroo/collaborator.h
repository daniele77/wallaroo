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

#ifndef WALLAROO_COLLABORATOR_H_
#define WALLAROO_COLLABORATOR_H_

#include <string>
#include <typeinfo>
#include <vector>
#include "cxx0x.h"
#include "dependency.h"
#include "part.h"
#include "exceptions.h"

namespace wallaroo
{

/// This type should be used as second template parameter in Collaborator class to specify 
/// that the Collaborator is optional (i.e.: you can omit to link a part to the collaborator)
struct optional
{
    template < typename T >
    static bool WiringOk( const cxx0x::weak_ptr< T >& ) { return true; }
};
/// This type should be used as second template parameter in Collaborator class to specify
/// that the Collaborator is mandatory (i.e.: you cannot omit to link a part to the collaborator)
struct mandatory
{
    template < typename T >
    static bool WiringOk( const cxx0x::weak_ptr< T >& t ) { return !t.expired(); }
};
/// This type should be used as second template parameter in Collaborator class to specify
/// that the Collaborator is a collection and you can wire the collaborator with a number 
/// of parts greater or equal to @c MIN and lesser or equal to @c MAX
template < std::size_t MIN = 0, std::size_t MAX = 0 >
struct bounded_collection
{
    template < typename T >
    static bool WiringOk( const T* t )
    {
        const std::size_t s = t -> size();
        return ( s >= MIN && ( MAX == 0 || s <= MAX ) );
    }
};
// template specializations to avoid warning about "unsigned is always >= 0":
template < std::size_t MAX >
struct bounded_collection< 0, MAX >
{
    template < typename T >
    static bool WiringOk( const T* t )
    {
        return ( t -> size() <= MAX );
    }
};
template < std::size_t MIN >
struct bounded_collection< MIN, 0 >
{
    template < typename T >
    static bool WiringOk( const T* t )
    {
        return ( t -> size() >= MIN );
    }
};
template <>
struct bounded_collection< 0, 0 >
{
    template < typename T >
    static bool WiringOk( const T* )
    {
        return true;
    }
};
/// This type should be used as second template parameter in Collaborator class
/// to specify that the Collaborator is a collection and you can wire as many 
/// parts to the collaborator as you want. Even zero.
typedef bounded_collection<> collection;

/**
 * This represents a "collaborator" of a "part" that
 * you can link with another "part".
 *
 * If the part1 has the collaborator1 linked to part2, part1 will
 * basically get a pointer to part2.
 *
 * @tparam T The type of the Part contained
 * @tparam P This represents the kind of Collaborator (@ref mandatory if you must link a part,
 *           @ref optional if you can leave this collaborator unlinked,
 *           @ref collection if you can link many parts to this collaborator)
 * @tparam Container If P = @ref collection, this represents the std container
 *           the Collaborator will derive from.
 */
template <
    typename T,
    typename P = mandatory,
    template < typename E, typename Allocator = std::allocator< E > > class Container = std::vector
>
class Collaborator  : public Dependency
{
public:

    typedef cxx0x::weak_ptr< T > WeakPtr;
    typedef cxx0x::shared_ptr< T > SharedPtr;

    /** Create a Collaborator and register it to its Part for later wiring.
    * @param name The name of this collaborator
    * @param token The registration token you can get by calling Part::RegistrationToken()
    */
    Collaborator( const std::string& name, const RegToken& token )
    {
        Part* owner = token.GetPart();
        owner -> Register( name, this );
    }

    /** Link this collaborator with a Part
    * @param dev The part you want link with this collaborator
    * @throw WrongType If @c dev is not a subclass of @c T
    */
    void Link( const cxx0x::shared_ptr< Part >& dev )
    {
        cxx0x::shared_ptr< T > _dev = cxx0x::dynamic_pointer_cast< T >( dev );
        if ( ! _dev ) // bad type!
            throw WrongType();
        else
            part = _dev;
    }

    /** Give access to the embedded part.
    * @throw DeletedPartError If the embedded part has been deleted.
    */
    SharedPtr operator -> ()
    {
        SharedPtr result = part.lock();
        if ( ! result ) 
            throw DeletedPartError();
        return result;
    }

    /** Give access to the embedded part as const.
    * @throw DeletedPartError If the embedded part has been deleted.
    */
    const SharedPtr operator -> () const
    {
        const SharedPtr result = part.lock();
        if ( ! result )
            throw DeletedPartError();
        return result;
    }

    /** Convert to a shared ptr.
    * @throw DeletedPartError If the embedded part has been deleted.
    */
    operator SharedPtr()
    {
        SharedPtr result = part.lock();
        if ( ! result )
            throw DeletedPartError();
        return result;
    }

    /** Convert to a const shared ptr.
    * @throw DeletedPartError If the embedded part has been deleted.
    */
    operator const SharedPtr() const
    {
        const SharedPtr result = part.lock();
        if ( ! result )
            throw DeletedPartError();
        return result;
    }
    
    /** Returns true if the collaborator has been wired and the embedded
    * part has not been deleted.
    * @return true If the embedded part exists.
    */
    operator bool() const
    {
        return !part.expired();
    }

   /** Check if this Collaborator is correctly wired according to the
    * P template parameter policy.
    * @return true If the check pass.
    */
    virtual bool WiringOk() const
    {
        return P::WiringOk( part );
    }

private:
    WeakPtr part;

    // copy ctor and assignment operator disabled
    Collaborator( const Collaborator& );
    Collaborator& operator = ( const Collaborator& );
};


// partial specialization for the collection case
template <
    typename T,
    template < typename E, typename Allocator = std::allocator< E > > class Container,
    std::size_t MIN,
    std::size_t MAX
>
class Collaborator< T, bounded_collection< MIN, MAX >, Container > : public Dependency, public Container< cxx0x::weak_ptr< T > >
{
private:
    typedef Container< cxx0x::weak_ptr< T > > C;

public:

    /** Create a Collaborator and register it to its Part for future wiring.
    * @param name The name of this collaborator
    * @param token The registration token you can get by calling Part::RegistrationToken()
    */
    Collaborator( const std::string& name, const RegToken& token )
    {
        Part* owner = token.GetPart();
        owner -> Register( name, this );
    }

    /** Add a Part into this (collection) collaborator
    * @param part The part to connect
    * @throw WrongType If @c part is not a subclass of @c T
    */
    void Link( const cxx0x::shared_ptr< Part >& part )
    {
        cxx0x::shared_ptr< T > obj = cxx0x::dynamic_pointer_cast< T >( part );
        if ( ! obj ) // bad type!
            throw WrongType();
        else
            C::push_back( obj );
    }

    /** Check if this Collaborator is correctly wired (i.e. the size of the collection
    * must be comprise in the interval [MIN, MAX])
    * @return true If the check pass.
    */
    virtual bool WiringOk() const
    {
        return bounded_collection< MIN, MAX >::WiringOk( this );
    }

private:
    // copy ctor and assignment operator disabled
    Collaborator( const Collaborator& );
    Collaborator& operator = ( const Collaborator& );
};

#ifndef WALLAROO_REMOVE_DEPRECATED
#define Plug Collaborator
#endif

}

#endif