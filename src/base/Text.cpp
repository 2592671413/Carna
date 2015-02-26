/*
 *  Copyright (C) 2010 - 2014 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 *
 */

#include <Carna/base/Text.h>
#include <fstream>

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// Text
// ----------------------------------------------------------------------------------

std::string Text::cat( const std::string& fileName )
{
    std::ifstream in( fileName.c_str(), std::ios::in );
    return std::string( std::istreambuf_iterator< char >( in ), std::istreambuf_iterator< char >() );
}



}  // namespace Carna :: base

}  // namespace Carna
