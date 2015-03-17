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
#include <Carna/base/Node.h>
#include <Carna/base/Geometry.h>
#include <Carna/base/Renderable.h>
#include <Carna/base/math.h>
#include <Carna/base/CarnaException.h>
#include <Carna/base/noncopyable.h>
#include <vector>
#include <algorithm>
#include <climits>

/** \file   RenderQueue.h
  * \brief  Defines \ref Carna::base::RenderQueue.
  */

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// RenderQueue
// ----------------------------------------------------------------------------------

/** \brief
  * Gathers \ref Renderable "renderable" geometry nodes from scene graph and provides
  * those in a particular order, if required.
  *
  * \param RenderableCompare
  *     Binary function that establishes partial order on `%Renderable` objects.
  *     Typical choices are \ref Renderable::BackToFront,
  *     \ref Renderable::FrontToBack or \ref Renderable::VideoResourcesOrder. Use
  *     `void` if no particular order is required.
  *
  * The geometry nodes are only enqueued if their geometry type matches. The matching
  * is done as follows. First a bit-wise *AND* operation is applied to the node's
  * geometry type and the \ref geometryTypeMask "mask" of this `%RenderQueue`
  * instance. Than the result from this operation is compared to the
  * \ref geometryType of this instance. Only if they match, the `%Geometry` node is
  * enqueued.
  *
  * \author Leonid Kostrykin
  * \date   22.2.15 - 17.3.15
  */
template< typename RenderableCompare >
class RenderQueue
{

    NON_COPYABLE

    std::vector< Renderable > renderables;
    std::size_t nextRenderableIndex;

public:

    /** \brief
      * Holds the geometry type that this queue uses for matching geometry nodes.
      */
    const unsigned int geometryType;

    /** \brief
      * Holds the mask that this queue uses for matching geometry nodes.
      */
    const unsigned int geometrTypeMask;

    /** \brief
      * Holds the mask that makes this queue only accept such geometry nodes whose
      * geometry type matches the geometry type of this queue exactly.
      */
    const static unsigned int EXACT_MATCH_GEOMETRY_TYPE_MASK;

    /** \brief
      * Creates new instance that enqueues \ref Geometry scene graph nodes if their
      * geometry type *AND*-linked with \a geometryTypeMask equals the
      * \a geometryType of this `RenderQueue` instance.
      */
    RenderQueue( unsigned int geometryType, unsigned int geometryTypeMask = EXACT_MATCH_GEOMETRY_TYPE_MASK );
    
    /** \brief
      * Rebuilds this queue by gathering matching geometry nodes from children of
      * \a root recursively. The argument \a viewTransform is required for the
      * computation of the \ref ViewSpace "model-view matrix".
      */
    void build( const Node& root, const math::Matrix4f& viewTransform );
    
    /** \brief
      * Rewinds this queue. This is an \f$\mathcal O\left(1\right)\f$ operation in
      * contrast to \ref build, so prefer it whenever possible.
      */
    void rewind();
    
    /** \brief
      * Tells whether this queue has reached it's end.
      */
    bool isEmpty() const;
    
    /** \brief
      * References the next element of the queue and moves ahead. The referenced
      * object stays alive until the queue is deleted or rebuilt.
      *
      * \pre `isEmpty() == false`
      */
    const Renderable& poll();

    /** \brief
      * References the next element of the queue, but does not move ahead. The
      * referenced object stays alive until the queue is deleted or rebuilt.
      *
      * \pre `isEmpty() == false`
      */
    const Renderable& first() const;
    
    /** \brief
      * References the last element of the queue. The referenced object stays alive
      * until the queue is deleted or rebuilt.
      *
      * \pre `isEmpty() == false`
      */
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



}  // namespace Carna :: base

}  // namespace Carna

#endif // RENDERQUEUE_H_6014714286
