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

#include <Carna/base/glew.h>
#include <Carna/base/glError.h>
#include <Carna/base/GLContext.h>
#include <Carna/base/Framebuffer.h>
#include <Carna/base/RenderTexture.h>
#include <Carna/base/Viewport.h>
#include <Carna/base/CarnaException.h>
#include <stdexcept>
#include <sstream>

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// createGlFramebuffer
// ----------------------------------------------------------------------------------

static inline unsigned int createGlFramebuffer()
{
    unsigned int n;
    glGenFramebuffersEXT( 1, &n );
    return n;
}



// ----------------------------------------------------------------------------------
// createGlDepthbufferObject
// ----------------------------------------------------------------------------------

static inline unsigned int createGlDepthbufferObject()
{
    unsigned int n;
    glGenRenderbuffersEXT( 1, &n );
    return n;
}



// ----------------------------------------------------------------------------------
// bindSystemFramebuffer
// ----------------------------------------------------------------------------------

static void bindSystemFramebuffer()
{
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
    if( GLContext::current().isDoubleBuffered )
    {
        glDrawBuffer( GL_BACK );
    }
    REPORT_GL_ERROR;
}



// ----------------------------------------------------------------------------------
// Framebuffer :: BindingStack
// ----------------------------------------------------------------------------------

class Framebuffer::BindingStack
{

    NON_COPYABLE

private:

    typedef std::map< const GLContext*, BindingStack* > InstanceByContext;

    static InstanceByContext instances;

    static InstanceByContext::iterator current();

    std::stack< MinimalBinding* > bindings;

public:

    static void refresh();

    static MinimalBinding& top();

    static void pop();

    static void push( MinimalBinding* );

    static bool empty();

}; // Framebuffer :: BindingStack


Framebuffer::BindingStack::InstanceByContext Framebuffer::BindingStack::instances;


Framebuffer::BindingStack::InstanceByContext::iterator Framebuffer::BindingStack::current()
{
    GLContext& currentContext = GLContext::current();

    const auto it = instances.find( &currentContext );
    if( it != instances.end() )
    {
        return it;
    }
    else
    {
        instances[ &currentContext ] = new BindingStack();

#ifndef _NO_FRAMEBUFFER_DEBUG
        qDebug() << "Framebuffer now manages" << instances.size() << "contexts (+1)";
#endif

        return instances.find( &currentContext );
    }
}


void Framebuffer::BindingStack::refresh()
{
    if( BindingStack::empty() )
    {
        bindSystemFramebuffer();
    }
    else
    {
        BindingStack::top().refresh();
    }
}


Framebuffer::MinimalBinding& Framebuffer::BindingStack::top()
{
    const auto& bindings = current()->second->bindings;

    CARNA_ASSERT( !bindings.empty() );

    return *bindings.top();
}


void Framebuffer::BindingStack::pop()
{
    const auto it = current();
    auto& bindings = it->second->bindings;

    CARNA_ASSERT( !bindings.empty() );

    bindings.pop();

    if( bindings.empty() )
    {
        delete it->second;
        instances.erase( it );

#ifndef _NO_FRAMEBUFFER_DEBUG
        qDebug() << "Framebuffer now manages" << instances.size() << "contexts (-1)";
#endif
    }
}


void Framebuffer::BindingStack::push( MinimalBinding* binding )
{
    const auto it = current();
    auto& bindings = it->second->bindings;

    bindings.push( binding );
}


bool Framebuffer::BindingStack::empty()
{
    const auto& bindings = current()->second->bindings;

    return bindings.empty();
}



// ----------------------------------------------------------------------------------
// Framebuffer
// ----------------------------------------------------------------------------------

Framebuffer::Framebuffer( unsigned int w, unsigned int h )
    : id( createGlFramebuffer() )
    , depthBuffer( createGlDepthbufferObject() )
{

 // initialize depth buffer

    resize( w, h );

 // bind framebuffer
 
    MinimalBinding binding( *this );

 // bind depth buffer to framebuffer object

    glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT
                                , GL_DEPTH_ATTACHMENT_EXT
                                , GL_RENDERBUFFER_EXT
                                , depthBuffer );

    REPORT_GL_ERROR;
}


Framebuffer::Framebuffer( RenderTexture& color_buffer )
    : id( createGlFramebuffer() )
    , depthBuffer( createGlDepthbufferObject() )
{

 // initialize depth buffer

    resize( color_buffer.width(), color_buffer.height() );

 // bind framebuffer
 
    MinimalBinding binding( *this );

 // bind depth buffer to framebuffer object

    glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT
                                , GL_DEPTH_ATTACHMENT_EXT
                                , GL_RENDERBUFFER_EXT
                                , depthBuffer );

 // bind color buffer to framebuffer object

    binding.setColorComponent( color_buffer );

    REPORT_GL_ERROR;
}


Framebuffer::~Framebuffer()
{

 // release fbo

    glDeleteFramebuffersEXT( 1, &id );

 // release depth buffer

    glDeleteRenderbuffersEXT( 1, &depthBuffer );

}


void Framebuffer::resize( unsigned int w, unsigned int h )
{
    CARNA_ASSERT_EX( w > 0, "framebuffer width must be greater zero" );
    CARNA_ASSERT_EX( h > 0, "framebuffer height must be greater zero" );

    this->w = w;
    this->h = h;

 // bind framebuffer
 
    MinimalBinding binding( *this );

 // update depth buffer
    
    glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, depthBuffer );
    glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, w, h );

    REPORT_GL_ERROR;
}


unsigned int Framebuffer::currentId()
{
    if( BindingStack::empty() )
    {
        return 0;
    }
    else
    {
        return BindingStack::top().framebuffer().id;
    }
}


void Framebuffer::copy( unsigned int srcId, unsigned int dstId, const Viewport& src, const Viewport& dst, unsigned int flags )
{
    const unsigned int  width = std::min( src.width , dst.width  );
    const unsigned int height = std::min( src.height, dst.height );
    copy( srcId, dstId, src.left, src.top, dst.left, dst.top, width, height, flags );
}


void Framebuffer::copy
    ( unsigned int idFrom, unsigned int idTo
    , unsigned int srcX0, unsigned int srcY0
    , unsigned int dstX0, unsigned int dstY0
    , unsigned int width, unsigned int height
    , unsigned int flags )
{
    /* Bind framebuffers.
     */
    glBindFramebufferEXT( GL_READ_FRAMEBUFFER, idFrom );
    glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER, idTo   );

    /* Do the copying.
     */
    glBlitFramebufferEXT
        ( srcX0, srcY0, srcX0 + width - 1, srcY0 + height - 1
        , dstX0, dstY0, dstX0 + width - 1, dstY0 + height - 1
        , flags
        , GL_NEAREST );

    /* Restore previous bindings.
     */
    BindingStack::refresh();
}



// ----------------------------------------------------------------------------------
// Framebuffer::MinimalBinding
// ----------------------------------------------------------------------------------

Framebuffer::MinimalBinding::MinimalBinding( Framebuffer& fbo )
    : fbo( fbo )
{
    Framebuffer* const currentFBO = BindingStack::empty() ? 0 : &( BindingStack::top().fbo );
    BindingStack::push( this );
    if( currentFBO != &fbo )
    {
        bindFBO();
    }
}


Framebuffer::MinimalBinding::~MinimalBinding()
{
    Framebuffer* const currentFBO = &( BindingStack::top().fbo );
    BindingStack::pop();

    if( BindingStack::empty() )
    {
        bindSystemFramebuffer();
    }
    else
    if( currentFBO != &( BindingStack::top().fbo ) )
    {
        BindingStack::top().refresh();
    }
}


void Framebuffer::MinimalBinding::bindFBO() const
{
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo.id );
    REPORT_GL_ERROR;
}


void Framebuffer::MinimalBinding::setColorComponent( RenderTexture& texture, unsigned int position )
{
    Framebuffer& fbo = BindingStack::top().fbo;

    CARNA_ASSERT_EX
        ( texture.width() == fbo.width() && texture.height() == fbo.height()
        , "render texture size must be same as framebuffer size" );

 // bind texture to framebuffer object

    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT
                             , GL_COLOR_ATTACHMENT0_EXT + position
                             , GL_TEXTURE_2D
                             , texture.id
                             , 0 );

    fbo.boundColorBuffers.insert( position );

    REPORT_GL_ERROR;
}


const Framebuffer& Framebuffer::MinimalBinding::framebuffer() const
{
    return fbo;
}


Color Framebuffer::MinimalBinding::readPixel( unsigned int x, unsigned int y, unsigned int color_attachment ) const
{
    // TODO: store position -> render texture mapping, read whether it's a floating point
    /*
    if( floatingPoint )
    {
        float data[ 4 ];
        glReadPixels( x, y, 1, 1, GL_RGBA, GL_FLOAT, data );
        return CVector( data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ] );
    }
    else
    {
    */
        unsigned char data[ 4 ];
        glReadBuffer( GL_COLOR_ATTACHMENT0_EXT + color_attachment );
        glReadPixels( x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data );
        return Color( data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ] );
    /*
    }
    */
}


void Framebuffer::MinimalBinding::refresh() const
{
    bindFBO();
}



// ----------------------------------------------------------------------------------
// Framebuffer::Binding
// ----------------------------------------------------------------------------------

Framebuffer::Binding::Binding( Framebuffer& fbo )
    : MinimalBinding( fbo )
{
    Binding::refresh();
}


void Framebuffer::Binding::refresh()
{
    MinimalBinding::refresh();

 // check FBO state for validity

    const GLenum error = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );

    switch( error )
    {

    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        {
            throw std::runtime_error( "framebuffer object configuration not supported" );
        }

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        {
            throw std::runtime_error( "framebuffer object misses color buffer" );
        }

    case GL_FRAMEBUFFER_COMPLETE_EXT:
        {
            break;
        }

    default:
        {
            std::stringstream ss;
            ss << "framebuffer object error code " << error;
            throw std::runtime_error( ss.str() );
        }

    }

 // setup render buffer

    if( !fbo.boundColorBuffers.empty() )
    {
        std::vector< GLenum > buffers( fbo.boundColorBuffers.size() );
        unsigned int index = 0;
        for( std::set< unsigned int >::const_iterator it  = fbo.boundColorBuffers.begin();
                                                      it != fbo.boundColorBuffers.end();
                                                    ++it, ++index )
        {
            CARNA_ASSERT_EX( index < buffers.size(), "color buffers are not bound continuously" );

            buffers[ index ] = GL_COLOR_ATTACHMENT0_EXT + index;
        }
        glDrawBuffers( buffers.size(), &buffers[ 0 ] );
    }

 // query GL error state

    REPORT_GL_ERROR;
}



}  // namespace Carna :: base

}  // namespace Carna
