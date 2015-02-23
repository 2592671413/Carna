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

#ifndef GEOMETRYSTAGE_H_6014714286
#define GEOMETRYSTAGE_H_6014714286

#include <Carna/base/view/RenderStage.h>
#include <Carna/base/view/RenderQueue.h>
#include <Carna/base/Matrix4f.h>
#include <memory>

/** \file   GeometryStage.h
  * \brief  Defines \ref Carna::base::view::GeometryStage.
  */

namespace Carna
{

namespace base
{

namespace view
{



// ----------------------------------------------------------------------------------
// GeometryStage
// ----------------------------------------------------------------------------------

template< typename RenderableCompare >
class GeometryStage : public RenderStage
{

    bool initialized;
    Node* root;
    std::size_t passesRendered;
    std::set< GeometryDefinition* > acquiredVideoResources;
    GLContext* myContext;

protected:

    RenderQueue< RenderableCompare > rq;

public:

    GeometryStage( int geometryType );

    /** \brief
      * Releases acquired video resources.
      */
    virtual ~GeometryStage();
    
    virtual void reshape( unsigned int width, unsigned int height ) override;
    
    virtual bool isInitialized() const override;

    virtual void prepareFrame( const FrameRenderer& fr, Node& root ) override;
    
    /** \brief
      * Builds the render queue.
      */
    virtual void preparePass( const Matrix4f& viewTransform ) override;
    
    virtual void renderPass( RenderTask& rt, const Viewport& vp ) override;

protected:

    virtual void render( const Renderable& ) = 0;

}; // GeometryStage


template< typename RenderableCompare >
GeometryStage< RenderableCompare >::GeometryStage( int geometryType )
    : initialized( false )
    , root( nullptr )
    , passesRendered( 0 )
    , myContext( nullptr )
    , rq( geometryType )
{
}


template< typename RenderableCompare >
GeometryStage< RenderableCompare >::~GeometryStage()
{
    if( myContext != nullptr )
    {
        myContext->makeActive();
    }
    std::for_each( acquiredVideoResources.begin(), acquiredVideoResources.end(), [&]( GeometryDefinition* gd )
        {
            gd->releaseVideoResources();
        }
    );
}


template< typename RenderableCompare >
void GeometryStage< RenderableCompare >::prepareFrame( const FrameRenderer& fr, Node& root )
{
    RenderStage::prepareFrame( fr, root );
    this->root = &root;
    this->passesRendered = 0;
    this->myContext = &fr.glContext();
}


template< typename RenderableCompare >
void GeometryStage< RenderableCompare >::preparePass( const Matrix4f& viewTransform )
{
    if( ++passesRendered == 1 || !isViewTransformFixed() )
    {
        rq.build( *root, viewTransform );
    }
    else
    {
        rq.rewind();
    }
}


template< typename RenderableCompare >
void GeometryStage< RenderableCompare >::renderPass( RenderTask& rt, const Viewport& vp )
{
    const bool isFirstPass = passesRendered == 1;
    std::set< GeometryDefinition* > usedVideoResources;
    while( !rq.isEmpty() )
    {
        const Renderable& renderable = rq.poll();
        if( isFirstPass )
        {
            // denote that the geometry definition was used
            GeometryDefinition& gd = renderable.geometry().definition();
            usedVideoResources.insert( &gd );

            // check whether video resources need to be acquired
            if( acquiredVideoResources.find( &gd ) == acquiredVideoResources.end() )
            {
                gd.acquireVideoResources();
                acquiredVideoResources.insert( &gd );
            }
        }
        render( renderable );
    }
    if( isFirstPass )
    {
        // release unused video resources
        for( auto itr = acquiredVideoResources.begin(); itr != acquiredVideoResources.end(); )
        {
            if( usedVideoResources.find( *itr ) == usedVideoResources.end() )
            {
                acquiredVideoResources.erase( itr++ );
            }
            else
            {
                ++itr;
            }
        }
    }
}


template< typename RenderableCompare >
void GeometryStage< RenderableCompare >::reshape( unsigned int width, unsigned int height )
{
    initialized = true;
}


template< typename RenderableCompare >
bool GeometryStage< RenderableCompare >::isInitialized() const
{
    return initialized;
}



}  // namespace Carna :: base :: view

}  // namespace Carna :: base

}  // namespace Carna

#endif // GEOMETRYSTAGE_H_6014714286
