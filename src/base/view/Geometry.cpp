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

#include <Carna/base/view/Geometry.h>

namespace Carna
{

namespace base
{

namespace view
{



// ----------------------------------------------------------------------------------
// Geometry
// ----------------------------------------------------------------------------------

Geometry::Geometry( int geometryType )
    : myDefinition( nullptr )
    , geometryType( geometryType )
{
}


void Geometry::setGeometryDefinition( const model::GeometryDefinition& gm )
{
    this->myDefinition = &gm;
}


bool Geometry::hasDefinition() const
{
    return myDefinition != nullptr;
}


const model::GeometryDefinition& Geometry::definition() const
{
    return *myDefinition;
}



}  // namespace Carna :: base :: view

}  // namespace Carna :: base

}  // namespace Carna
