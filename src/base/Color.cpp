/*
 *  Copyright (C) 2010 - 2015 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 *
 */

#include <Carna/base/Color.h>
#include <Carna/base/math.h>

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// Color
// ----------------------------------------------------------------------------------

Color::Color( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
    : r( r )
    , g( g )
    , b( b )
    , a( a )
{
}


Color::operator math::Vector4f()
{
    return math::Vector4f( r / 255.f, g / 255.f, b / 255.f, a / 255.f );
}


bool Color::operator==( const Color& c2 ) const
{
    const Color& c1 = *this;
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}



}  // namespace Carna :: base

}  // namespace Carna