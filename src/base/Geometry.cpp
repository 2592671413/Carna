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

#include <Carna/base/Geometry.h>
#include <Carna/base/GeometryFeature.h>
#include <Carna/base/CarnaException.h>
#include <vector>
#include <map>

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// Geometry :: Details
// ----------------------------------------------------------------------------------

struct Geometry::Details
{
    std::map< unsigned int, GeometryFeature* > featureByRole;
    std::map< GeometryFeature*, unsigned int > roleByFeature;
};



// ----------------------------------------------------------------------------------
// Geometry
// ----------------------------------------------------------------------------------

Geometry::Geometry( unsigned int geometryType )
    : pimpl( new Details() )
    , geometryType( geometryType )
{
}


Geometry::~Geometry()
{
    clearFeatures();
}


void Geometry::clearFeatures()
{
    std::vector< GeometryFeature* > features( pimpl->featureByRole.size() );
    unsigned int featureIdx = 0;
    for( auto featureItr = pimpl->featureByRole.begin(); featureItr != pimpl->featureByRole.end(); ++featureItr )
    {
        features[ featureIdx++ ] = featureItr->second;
    }
    pimpl->featureByRole.clear();
    pimpl->roleByFeature.clear();
    for( auto featureItr = features.begin(); featureItr != features.end(); ++featureItr )
    {
        GeometryFeature* const ga = *featureItr;
        ga->removeFrom( *this );
    }
}


void Geometry::visitFeatures( const std::function< void( GeometryFeature& gf, unsigned int role ) >& visit ) const
{
    for( auto featureItr = pimpl->featureByRole.begin(); featureItr != pimpl->featureByRole.end(); ++featureItr )
    {
        visit( *featureItr->second, featureItr->first );
    }
}


void Geometry::putFeature( unsigned int role, GeometryFeature& gf )
{
    const auto featureByRoleItr = pimpl->featureByRole.find( role );
    if( featureByRoleItr != pimpl->featureByRole.end() && featureByRoleItr->second != &gf )
    {
        /* Given role is already occupied by another feature.
         */
        removeFeature( role );
    }
    const auto roleByFeatureItr = pimpl->roleByFeature.find( &gf );
    if( roleByFeatureItr != pimpl->roleByFeature.end() && roleByFeatureItr->second != role )
    {
        /* Given feature already occupies another role.
         */
        removeFeature( gf );
    }
    const std::size_t size0 = pimpl->featureByRole.size();
    pimpl->featureByRole[ role ] = &gf;
    pimpl->roleByFeature[ &gf ] = role;
    if( size0 != pimpl->featureByRole.size() )
    {
        gf.addTo( *this, role );
    }
}


void Geometry::removeFeature( GeometryFeature& gf )
{
    const auto featureItr = pimpl->roleByFeature.find( &gf );
    if( featureItr != pimpl->roleByFeature.end() )
    {
        const unsigned int role = featureItr->second;
        pimpl->roleByFeature.erase( featureItr );
        pimpl->featureByRole.erase( role );
        gf.removeFrom( *this );
    }
}


void Geometry::removeFeature( unsigned int role )
{
    const auto featureItr = pimpl->featureByRole.find( role );
    if( featureItr != pimpl->featureByRole.end() )
    {
        GeometryFeature* const gf = featureItr->second;
        pimpl->roleByFeature.erase( gf );
        pimpl->featureByRole.erase( featureItr );
        gf->removeFrom( *this );
    }
}


bool Geometry::hasFeature( const GeometryFeature& gf ) const
{
    return pimpl->roleByFeature.find( const_cast< GeometryFeature* >( &gf ) ) != pimpl->roleByFeature.end();
}


bool Geometry::hasFeature( unsigned int role ) const
{
    return pimpl->featureByRole.find( role ) != pimpl->featureByRole.end();
}


GeometryFeature& Geometry::feature( unsigned int role ) const
{
    const auto featureItr = pimpl->featureByRole.find( role );
    CARNA_ASSERT( featureItr != pimpl->featureByRole.end() );
    return *featureItr->second;
}


std::size_t Geometry::featuresCount() const
{
    return pimpl->featureByRole.size();
}



}  // namespace Carna :: base

}  // namespace Carna
