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

#ifndef RENDERSTAGE_H_6014714286
#define RENDERSTAGE_H_6014714286

#include <Carna/Carna.h>
#include <Carna/base/noncopyable.h>
#include <Carna/base/math.h>

/** \file   RenderStage.h
  * \brief  Defines \ref Carna::base::RenderStage.
  */

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// RenderStage
// ----------------------------------------------------------------------------------

/** \brief
  * Base abstract class of each rendering stage. Refer to the documentation of the
  * \ref RenderingProcess "rendering process".
  *
  * \author Leonid Kostrykin
  * \date   21.2.15 - 17.3.15
  */
class CARNA_LIB RenderStage
{

    NON_COPYABLE

    bool viewTransformFixed;

public:

    RenderStage();

    virtual ~RenderStage();
    
    /** \brief
      * Sets whether the view transform is pass-invariant for the duration of a
      * single frame.
      *
      * \note
      * This method is for internal usage only.
      */
    void setViewTransformFixed( bool viewTransformFixed );
    
    /** \brief
      * Tells whether the view transform is pass-invariant for the duration of a
      * single frame.
      */
    bool isViewTransformFixed() const;
    
    /** \brief
      * Orders this stage to reshape its buffers according to the specified
      * dimensions.
      */
    virtual void reshape( const FrameRenderer& fr, const Viewport& vp ) = 0;
    
    /** \brief
      * Tells whether this stage is ready for rendering.
      *
      * In particular, this method returns `true` if this rendering stage only
      * requires \ref reshape to be called, if the dimensions actually have changed.
      * In contrary, i.e. if this method returns `false`, \ref reshape will be called
      * before every frame.
      */
    virtual bool isInitialized() const = 0;
    
    /** \brief
      * Called once before each frame.
      */
    virtual void prepareFrame( Node& root );

    /** \brief
      * Called once per pass.
      *
      * If this rendering stage maintains one or more \ref RenderQueue objects, than
      * this is the right place to \ref RenderQueue::build "build" them. Note that
      * the queues need to be rebuilt only *once per frame* and not *per pass*,
      * unless \ref isViewTransformFixed is `false`. If it is `true` and this is not
      * the first invocation of this method since the last time \ref prepareFrame was
      * called, \ref RenderQueue::rewind "rewinding" the queue will be sufficient.
      */
    virtual void renderPass( const math::Matrix4f& viewTransform, RenderTask& rt, const Viewport& vp ) = 0;

}; // RenderStage



}  // namespace Carna :: base

}  // namespace Carna

#endif // RENDERSTAGE_H_6014714286
