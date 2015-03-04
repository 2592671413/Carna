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

#include <Carna/base/Vertex.h>
#include <vector>

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// VertexBase
// ----------------------------------------------------------------------------------

const VertexAttributes VertexBase::attributes = []()->VertexAttributes
{
    using Carna::base::VertexAttribute;
    std::vector< VertexAttribute > attributes;
    attributes.push_back( VertexAttribute( 0, 4 ) );
    return attributes;
}();


VertexBase::VertexBase()
    : x( 0 )
    , y( 0 )
    , z( 0 )
    , w( 1 )
{
}



}  // namespace Carna :: base

}  // namespace Carna
