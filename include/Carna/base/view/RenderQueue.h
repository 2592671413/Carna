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

#ifndef RENDERQUEUE_H_6014714286
#define RENDERQUEUE_H_6014714286

#include <Carna/Carna.h>
#include <Carna/base/view/Node.h>
#include <Carna/base/view/Geometry.h>
#include <Carna/base/view/Renderable.h>
#include <Carna/base/math.h>
#include <Carna/base/CarnaException.h>
#include <Carna/base/noncopyable.h>
#include <vector>
#include <algorithm>
#include <climits>

/** \file   RenderQueue.h
  * \brief  Defines \ref Carna::base::view::RenderQueue.
  */

namespace Carna
{

namespace base
{

namespace view
{



// ----------------------------------------------------------------------------------
// RenderQueue
// ----------------------------------------------------------------------------------

template< typename RenderableCompare >
class RenderQueue
{

    NON_COPYABLE

    std::vector< Renderable > renderables;
    std::size_t nextRenderableIndex;

public:

    const unsigned int geometryType;

    const unsigned int geometrTypeMask;

    const static unsigned int EXACT_MATCH_GEOMETRY_TYPE_MASK;

    RenderQueue( unsigned int geometryType, unsigned int geometryTypeMask = EXACT_MATCH_GEOMETRY_TYPE_MASK );
    
    void build( const Node& root, const math::Matrix4f& viewTransform );
    
    void rewind();
    
    bool isEmpty() const;
    
    const Renderable& poll();

    const Renderable& first() const;

    const Renderable& last() const;

}; // RenderQueue


template< typename RenderableCompare >
const unsigned int RenderQueue< RenderableCompare >::EXACT_MATCH_GEOMETRY_TYPE_MASK = std::numeric_limits< unsigned int >::max();


template< typename RenderableCompare >
RenderQueue< RenderableCompare >::RenderQueue( unsigned int geometryType, unsigned int geometrTypeMask )
    : geometryType( geometryType )
    , geometrTypeMask( geometrTypeMask )
{
}


template< typename RenderableCompare >
struct RenderableSort
{
    static void sort( std::vector< Renderable >& renderables )
    {
        if( renderables.size() > 2 )
        {
            std::sort( renderables.begin(), renderables.end(), RenderableCompare() );
        }
    }
};


template< >
struct RenderableSort< void >
{
    static void sort( std::vector< Renderable >& )
    {
    }
};


template< typename RenderableCompare >
void RenderQueue< RenderableCompare >::build( const Node& root, const math::Matrix4f& viewTransform )
{
    renderables.clear();
    nextRenderableIndex = 0;
    
    // collect all geometries
    root.visitChildren( true, [&]( const Spatial& spatial )
        {
            const Geometry* const geom = dynamic_cast< const Geometry* >( &spatial );
            if( geom != nullptr && ( geom->geometryType & geometrTypeMask ) == geometryType )
            {
                const math::Matrix4f modelViewTransform = viewTransform * geom->worldTransform();
                renderables.push_back( Renderable( *geom, modelViewTransform ) );
            }
        }
    );
    
    // order geometries as required
    RenderableSort< RenderableCompare >::sort( renderables );
}


template< typename RenderableCompare >
void RenderQueue< RenderableCompare >::rewind()
{
    nextRenderableIndex = 0;
}


template< typename RenderableCompare >
bool RenderQueue< RenderableCompare >::isEmpty() const
{
    return nextRenderableIndex >= renderables.size();
}


template< typename RenderableCompare >
const Renderable& RenderQueue< RenderableCompare >::poll()
{
    CARNA_ASSERT( !isEmpty() );
    return renderables[ nextRenderableIndex++ ];
}


template< typename RenderableCompare >
const Renderable& RenderQueue< RenderableCompare >::first() const
{
    CARNA_ASSERT( !isEmpty() );
    return renderables.front();
}


template< typename RenderableCompare >
const Renderable& RenderQueue< RenderableCompare >::last() const
{
    CARNA_ASSERT( !isEmpty() );
    return renderables.back();
}



}  // namespace Carna :: base :: view

}  // namespace Carna :: base

}  // namespace Carna

#endif // RENDERQUEUE_H_6014714286
