﻿/*
 *  Copyright (C) 2010 - 2015 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 *
 */

#include <Carna/VolumeRenderings/RayMarchingStage.h>
#include <Carna/base/view/Mesh.h>
#include <Carna/base/view/Vertex.h>
#include <Carna/base/view/IndexBuffer.h>
#include <Carna/base/view/Texture3DManager.h>
#include <Carna/base/view/ShaderManager.h>
#include <Carna/base/Matrix4f.h>

namespace Carna
{

namespace VolumeRenderings
{



// ----------------------------------------------------------------------------------
// RayMarchingStage :: Details
// ----------------------------------------------------------------------------------

struct RayMarchingStage::Details
{

    Details();

    base::view::RenderTask* renderTask;
    const base::view::Viewport* viewPort;
    unsigned int mySampleRate;

}; // RayMarchingStage :: Details


RayMarchingStage::Details::Details()
    : renderTask( nullptr )
    , viewPort( nullptr )
    , mySampleRate( DEFAULT_SAMPLE_RATE )
{
}



// ----------------------------------------------------------------------------------
// RayMarchingStage :: VideoResources
// ----------------------------------------------------------------------------------

struct RayMarchingStage::VideoResources
{

    VideoResources( const base::view::ShaderProgram& shader );

    base::view::Mesh< base::view::VertexBase, uint8_t > sliceMesh;
    const base::view::ShaderProgram& shader;

    void renderSlice( RayMarchingStage& self, const base::view::Renderable& renderable, const base::Matrix4f& sliceTangentModel, const base::Matrix4f& modelView );

}; // RayMarchingStage :: VideoResources


RayMarchingStage::VideoResources::VideoResources( const base::view::ShaderProgram& shader )
    : sliceMesh( base::view::IndexBufferBase::PRIMITIVE_TYPE_TRIANGLE_FAN )
    , shader( shader )
{
    const float radius = std::sqrt( 3.f ) / 2;
    base::view::VertexBase vertices[ 4 ];

    vertices[ 0 ].x = -radius;
    vertices[ 0 ].y = -radius;

    vertices[ 1 ].x = +radius;
    vertices[ 1 ].y = -radius;

    vertices[ 2 ].x = +radius;
    vertices[ 2 ].y = +radius;

    vertices[ 3 ].x = -radius;
    vertices[ 3 ].y = +radius;

    sliceMesh.vertexBuffer().copy( vertices, 4 );
}


void RayMarchingStage::VideoResources::renderSlice
    ( RayMarchingStage& self
    , const base::view::Renderable& renderable
    , const base::Matrix4f& sliceTangentModel
    , const base::Matrix4f& modelView )
{
    unsigned int lastSampler = base::view::Texture3D::SETUP_SAMPLER;
    std::vector< unsigned int > roles;
    renderable.geometry().visitAggregates( [&lastSampler, &roles]( base::view::GeometryAggregate& ga, unsigned int role )
        {
            if( dynamic_cast< const base::view::Texture3DManager* >( &ga.videoResources() ) != nullptr )
            {
                const base::view::Texture3DManager& textureManager = static_cast< const base::view::Texture3DManager& >( ga.videoResources() );
                textureManager.texture().bind( ++lastSampler );
                roles.push_back( role );
            }
        }
    );

    /* Configure shader.
     */
    base::view::ShaderProgram::putUniform4x4f( "sliceTangentModel", sliceTangentModel );
    base::view::ShaderProgram::putUniform4x4f( "modelViewProjection", self.pimpl->renderTask->projection * modelView );
    for( unsigned int samplerOffset = 0; samplerOffset < roles.size(); ++samplerOffset )
    {
        const unsigned int role = roles[ samplerOffset ];
        const unsigned int sampler = base::view::Texture3D::SETUP_SAMPLER + samplerOffset;
        const std::string& uniformName = self.uniformName( role );
        base::view::ShaderProgram::putUniform1i( uniformName, sampler );
    }

    /* Invoke shader.
     */
    sliceMesh.render();
}



// ----------------------------------------------------------------------------------
// RayMarchingStage
// ----------------------------------------------------------------------------------

RayMarchingStage::RayMarchingStage()
    : base::view::GeometryStage< base::view::Renderable::DepthOrder< base::view::Renderable::ORDER_BACK_TO_FRONT > >
        ::GeometryStage( GEOMETRY_TYPE )
    , pimpl( new Details() )
{
}


RayMarchingStage::~RayMarchingStage()
{
    activateGLContext();
    if( vr.get() != nullptr )
    {
        base::view::ShaderManager::instance().releaseShader( vr->shader );
    }
}


void RayMarchingStage::render( const base::view::Renderable& renderable )
{
    using base::Matrix4f;
    using base::Vector4f;

    /* Hereinafter the term 'model' is identified with 'segment'.
     */
    const Matrix4f& modelView = renderable.modelViewTransform();

    /* Compute the directional vector from eye to segment center.
     * This vector needs to be renormalized since 'viewModel' may contain scalings.
     */
    const Matrix4f viewModel = modelView.inverse();
    const Vector4f viewDirectionInModelSpace = base::normalized( Vector4f( viewModel * Vector4f( 0, 0, -1, 0 ) ) );

    /* Construct billboard at segment center, i.e. plane that always faces the camera.
     */
    const Vector4f modelNormal = viewModel * Vector4f( 0, 0, 1, 0 );
    const Vector4f modelTangent = viewModel * Vector4f( 1, 0, 0, 0 );
    const Vector4f modelBitangent = viewModel * Vector4f( 0, 1, 0, 0 );
    const Matrix4f tangentModel = base::basis4f( modelTangent, modelBitangent, modelNormal );

    /* NOTE: This can be optimized using geometry shader, by sending only the central
     * slice to the GPU and constructing the others in the shader.
     */
    for( unsigned int sampleIdx = 0; sampleIdx < pimpl->mySampleRate; ++sampleIdx )
    {
        const Vector4f offset = viewDirectionInModelSpace * std::sqrt( 3.f ) * ( 0.5f - static_cast< float >( sampleIdx ) / ( pimpl->mySampleRate - 1 ) );

        /* Construct transformation from tangent to model space for specific slice.
         */
        const Matrix4f sliceOffset = base::translation4f( offset );
        const Matrix4f sliceTangentModel = sliceOffset * tangentModel;

        vr->renderSlice( *this, renderable, sliceTangentModel, modelView );
    } 
}


void RayMarchingStage::renderPass( base::view::RenderTask& rt, const base::view::Viewport& vp )
{
    if( vr.get() == nullptr )
    {
        const base::view::ShaderProgram& shader = loadShader();
        vr.reset( new VideoResources( shader ) );
    }

    rt.renderer.glContext().setShader( vr->shader );
    configureShader( rt.renderer.glContext() );

    pimpl->renderTask = &rt;
    pimpl->viewPort = &vp;
    
    /* Ensure proper OpenGL state.
     */
    glDepthMask( GL_FALSE );
    glEnable( GL_DEPTH_TEST );
    
    /* Do the rendering.
     */
    base::view::GeometryStage< base::view::Renderable::DepthOrder< base::view::Renderable::ORDER_BACK_TO_FRONT > >::renderPass( rt, vp );

    /* There is no guarantee that 'renderTask' will be valid later.
     */
    pimpl->renderTask = nullptr;
}


void RayMarchingStage::setSampleRate( unsigned int sampleRate )
{
    CARNA_ASSERT( sampleRate >= 2 );
    pimpl->mySampleRate = sampleRate;
}


unsigned int RayMarchingStage::sampleRate() const
{
    return pimpl->mySampleRate;
}



}  // namespace Carna :: VolumeRenderings

}  // namespace Carna
