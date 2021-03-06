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

#ifndef WALLAROO_DETAIL_TOKENSOURCE_H_
#define WALLAROO_DETAIL_TOKENSOURCE_H_

#include <string>
#include <sstream>
#include "wallaroo/exceptions.h"

namespace wallaroo
{
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////

// Represent a token when scanning a .wal configuration file
struct Token
{
    // terminals
    enum Type { 
        load,       // @load
        create,     // new
        open,       // (
        close,      // )
        stmtsep,    // ;
        attrsep,    // ,
        assign,     // =
        collsep,    // .
        id,         // <id>     [a-zA-Z_] [a-zA-Z0-9_:<>]*
        value,      // true | false | \'{char}\' | \"{char}*\" | [0-9+-]?[0-9]+\.[0-9]+
        done        // EOF
    };
    Type type;
    std::string lexem;
    Token( Type t, const std::string& l = std::string() ) : type( t ), lexem( l ) {}
    // return a string explaining the type of a token
    static std::string Description( Type t )
    {
        switch ( t )
        {
            case load: return "@load"; break;
            case create: return "new"; break;
            case open: return "("; break;
            case close: return ")"; break;
            case stmtsep: return ";"; break;
            case attrsep: return ","; break;
            case assign: return "="; break;
            case collsep: return "."; break;
            case id: return "<id>"; break;
            case value: return "<value>"; break;
            case done: return "<EOF>"; break;
        }
        return "???"; // can't reach this point
    }
};

// Split an input stream into a sequence of token.
// Each call at Split::Next method returns the next token
// (or throws a LexicalError if the next token is unknown).
class TokenSource
{
public:
    explicit TokenSource( std::istream& in ) : input( in ), lineno( 1 ), column( 1 ) {}
    // throw LexicalError
    Token Next()
    {
        while ( true )
        {
            char c = input.peek();
            switch ( c )
            {
                case ' ': case '\t': Consume(); break;
                case '\n': NewLine(); Consume(); break;
                case '(': Consume(); return Token( Token::open ); break;
                case ')': Consume(); return Token( Token::close ); break;
                case ';': Consume(); return Token( Token::stmtsep ); break;
                case ',': Consume(); return Token( Token::attrsep ); break;
                case '=': Consume(); return Token( Token::assign ); break;
                case '.': Consume(); return Token( Token::collsep ); break;
                case '\'': 
                    {
                        Consume(); // '
                        c = input.peek();
                        // TODO manage quoted chars
                        Consume(); // char
                        if ( input.peek() == '\'' ) Consume(); // '
                        else throw LexicalError( "Missing terminating char closing", lineno, column );
                        return Token( Token::value, std::string( 1, c ) );
                    }
                    break;
                case '"':
                    {
                        std::string value;
                        Consume();
                        c = input.peek();
                        while ( c != '"' && !input.eof() && c != '\n' )
                        {
                            value += c;
                            Consume();
                            c = input.peek();
                        }
                        if ( c == '"' ) Consume();
                        else throw LexicalError( "Missing terminating string closing", lineno, column );
                        return Token( Token::value, value );
                    }
                    break;
                case '#':
                    {
                        while ( c != '\n' && !input.eof() )
                        {
                            Consume();
                            c = input.peek();
                        }
                        if ( input.eof() ) return Token( Token::done );
                        else 
                        {
                            NewLine();
                            Consume();
                        }
                    }
                    break;
                default:
                {
                    if ( isalpha( c ) || c == '_' )
                    {
                        std::string id;
                        while ( isalnum( c ) || c == '_' || c == ':' || c == '<' || c == '>' )
                        {
                            id += c;
                            Consume();
                            c = input.peek(); // next char...
                        }
                        if ( id == "new" ) return Token( Token::create );
                        else if ( id == "true" || id == "false" ) return Token( Token::value, id );
                        else return Token( Token::id, id );
                    }
                    else if (c == '@')
                    {
                        std::string id;
                        while (isalnum(c) || c == '@')
                        {
                            id += c;
                            Consume();
                            c = input.peek(); // next char...
                        }
                        if (id == "@load") return Token(Token::load);
                        throw LexicalError("Invalid keyword:" + id, lineno, column);
                    }
                    else if ( input.eof() )
                        return Token( Token::done );
                    else if ( isdigit( c ) || c == '-' || c == '+' )
                    {
                        std::string num;
                        std::size_t sepcount = 0;

                        num += c;
                        Consume();
                        c = input.peek(); // next char...

                        while ( isdigit( c ) || c == '.' )
                        {
                            if ( c == '.' ) 
                                if ( ++sepcount > 1 )
                                    throw LexicalError( "Floating point invalid (too many '.')", lineno, column );
                            num += c;
                            Consume();
                            c = input.peek(); // next char...
                        }
                        return Token( Token::value, num );
                    }
                    else
                        throw LexicalError( "Unrecognized character", lineno, column );
                }
            }
        }
    }

    std::size_t Line() const { return lineno; }
    std::size_t Col() const { return column; }

private:
    void NewLine()
    {
        ++lineno;
        column = 1;
    }
    void Consume()
    {
        input.get();
        ++column;
    }
    std::istream& input;
    std::size_t lineno;
    std::size_t column;
};

} // detail
} // namespace

#endif // WALLAROO_DETAIL_TOKENSOURCE_H_
