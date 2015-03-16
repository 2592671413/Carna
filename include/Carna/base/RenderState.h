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

#ifndef RENDERSTATE_H_6014714286
#define RENDERSTATE_H_6014714286

#include <Carna/Carna.h>
#include <Carna/base/noncopyable.h>
#include <memory>

/** \file   RenderState.h
  * \brief  Defines \ref Carna::base::RenderState.
  */

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// RenderState
// ----------------------------------------------------------------------------------

/** \brief
  * Manages the OpenGL render state.
  *
  * Each \ref GLContext "OpenGL context" has a so-called render state. This describes
  * things like whether alpha blending is to be used for rendering primitives or not.
  * The purpose of this class is to reduce the state transitions, that can be quite
  * expansive, and make it easy to enforce a fully-defined state.
  *
  * The mechanism behind this class exploits the fact that the *current* OpenGL
  * context is a thread-local variable as described \ref GLContext "here". This
  * allows us to utilize the execution stack to track the render state:
  *
  *     \code
  *     void render()
  *     {
  *         RenderState rs( GLContext::current() );
  *         rs.setDepthTest( false );
  *         rs.setBlend( true );
  *         render1();
  *         render2();
  *     }
  *
  *     void render1()
  *     {
  *         RenderState rs( GLContext::current() );
  *         rs.setDepthTest( false );
  *         rs.setBlendFunction( BlendFunction( GL_ONE, GL_ONE ) );
  *         renderSomething();
  *     }
  *
  *     void render2()
  *     {
  *         RenderState rs( GLContext::current() );
  *         rs.setDepthTest( false );
  *         renderSomething();
  *     }
  *     \endcode
  *
  * The constructor derives a new render state, that immediately becomes the
  * *current* one and equals the previous state. The method `render` disables depth
  * testing and enables alpha blending. Whether state transitions actually happen
  * here depends on how the previous state was configured. When `render1` turns off
  * depth testing again, nothing happens, because it already was disabled within the
  * previous state. Same is true for `render2`. The updated blend function of
  * `render1` does not affect `render2`. Note that it is forbidden to modify a render
  * state that is not the current one.
  *
  * \author Leonid Kostrykin
  * \date   22.2.15 - 16.3.15
  */
class CARNA_LIB RenderState
{

    NON_COPYABLE

    struct Details;
    const std::unique_ptr< Details > pimpl;

    RenderState();

protected:

    friend class GLContext;

    static RenderState* createDefaultRenderState( GLContext& );

    void commit() const;

public:

    enum CullFace
    {

        /** \brief
          * All polygons will pass the cull test.
          */
        cullNone,
        
        /** \brief
          * Polygons recognized as back-faces will fail the cull test.
          */
        cullBack,
        
        /** \brief
          * Polygons recognized as front-faces will fail the cull test.
          */
        cullFront

    }; // CullFace

    explicit RenderState( GLContext& );

    ~RenderState();

    void setDepthTest( bool dt );

    void setDepthWrite( bool dw );

    void setDepthTestFunction( int dtf );

    void setBlend( bool b );

    void setBlendFunction( const BlendFunction& bf );

    void setBlendEquation( int be );

    void setCullFace( CullFace cf );

    void setFrontFace( bool ccw );

private:

    void commitDepthTest() const;

    void commitDepthWrite() const;

    void commitDepthTestFunction() const;

    void commitBlend() const;

    void commitBlendFunction() const;

    void commitBlendEquation() const;

    void commitCullFace() const;

    void commitFrontFace() const;

}; // RenderState



}  // namespace Carna :: base

}  // namespace Carna

#endif // RENDERSTATE_H_6014714286
