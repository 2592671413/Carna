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

#include <Carna/presets/RayMarchingStage.h>
#include <Carna/base/Mesh.h>
#include <Carna/base/Vertex.h>
#include <Carna/base/IndexBuffer.h>
#include <Carna/base/ShaderManager.h>
#include <Carna/base/RenderState.h>
#include <Carna/base/ShaderUniform.h>
#include <Carna/base/Log.h>
#include <Carna/base/math.h>
#include <Carna/base/text.h>

namespace Carna
{

namespace presets
{



// ----------------------------------------------------------------------------------
// RayMarchingStage :: Details
// ----------------------------------------------------------------------------------

struct RayMarchingStage::Details
{
    Details();

    base::RenderTask* renderTask;
    const base::Viewport* viewPort;
    unsigned int sampleRate;
};


RayMarchingStage::Details::Details()
    : renderTask( nullptr )
    , viewPort( nullptr )
    , sampleRate( DEFAULT_SAMPLE_RATE )
{
}



// ----------------------------------------------------------------------------------
// RayMarchingStage :: VideoResources
// ----------------------------------------------------------------------------------

struct RayMarchingStage::VideoResources
{
    VideoResources( const base::ShaderProgram& shader, unsigned int sampleRate );

    const base::ShaderProgram& shader;
    std::map< unsigned int, base::Sampler* > samplers;
    
    typedef base::Mesh< base::VertexBase, uint16_t > SlicesMesh;
    SlicesMesh::VideoResourceAcquisition& slicesMesh( unsigned int sampleRate );
    
private:

    unsigned int mySampleRate;
    std::unique_ptr< SlicesMesh::VideoResourceAcquisition > mySlicesMesh;
    static SlicesMesh::VideoResourceAcquisition* createSlicesMesh( unsigned int sampleRate );
};


RayMarchingStage::VideoResources::VideoResources( const base::ShaderProgram& shader, unsigned int sampleRate )
    : shader( shader )
    , mySampleRate( sampleRate + 1 )
{
    /* Create the slices mesh.
     */
    slicesMesh( sampleRate );
}


RayMarchingStage::VideoResources::SlicesMesh::VideoResourceAcquisition* RayMarchingStage::VideoResources::createSlicesMesh
    ( unsigned int sampleRate )
{
    /* Actually, one would assume that the radius should be _half_ of the square root
     * of 3. But if specified so, one encounters "holes" in volume renderings. For
     * the moment, just the square root of 3 seems to produce slices that are large
     * enough, although this particular value is somewhat "random".
     */
    const float radius = std::sqrt( 3.f );
    
    /* Create slices.
     */
    std::vector< typename SlicesMesh::Vertex > vertices( 4 * sampleRate );
    std::vector< typename SlicesMesh::Index  >  indices( 6 * sampleRate );
    for( unsigned int sliceIdx = 0; sliceIdx < sampleRate; ++sliceIdx )
    {
        const float progress = static_cast< float >( sliceIdx ) / ( sampleRate - 1 );
        const float offset = std::sqrt( 3.f ) * ( 0.5f - progress );

        /* Create slice vertices.
         */
        vertices[ 4 * sliceIdx + 0 ].x = -radius;
        vertices[ 4 * sliceIdx + 0 ].y = -radius;
        vertices[ 4 * sliceIdx + 0 ].z =  offset;

        vertices[ 4 * sliceIdx + 1 ].x = +radius;
        vertices[ 4 * sliceIdx + 1 ].y = -radius;
        vertices[ 4 * sliceIdx + 1 ].z =  offset;

        vertices[ 4 * sliceIdx + 2 ].x = +radius;
        vertices[ 4 * sliceIdx + 2 ].y = +radius;
        vertices[ 4 * sliceIdx + 2 ].z =  offset;

        vertices[ 4 * sliceIdx + 3 ].x = -radius;
        vertices[ 4 * sliceIdx + 3 ].y = +radius;
        vertices[ 4 * sliceIdx + 3 ].z =  offset;
        
        /* Create slice indices.
         */
        indices[ 6 * sliceIdx + 0 ] = 4 * sliceIdx + 0;
        indices[ 6 * sliceIdx + 1 ] = 4 * sliceIdx + 1;
        indices[ 6 * sliceIdx + 2 ] = 4 * sliceIdx + 2;
        indices[ 6 * sliceIdx + 3 ] = 4 * sliceIdx + 2;
        indices[ 6 * sliceIdx + 4 ] = 4 * sliceIdx + 3;
        indices[ 6 * sliceIdx + 5 ] = 4 * sliceIdx + 0;
    }

    /* Create the mesh.
     */
    SlicesMesh& mesh = SlicesMesh::create
        ( base::IndexBufferBase::PRIMITIVE_TYPE_TRIANGLES
        , &vertices.front(), vertices.size()
        , &indices.front(), indices.size() );
    
    /* Acquire the video resources and release the mesh s.t. it is deleted when the
     * video resources are released.
     */
    SlicesMesh::VideoResourceAcquisition* const meshVR = new SlicesMesh::VideoResourceAcquisition( mesh );
    mesh.release();
    return meshVR;
}


RayMarchingStage::VideoResources::SlicesMesh::VideoResourceAcquisition& RayMarchingStage::VideoResources::slicesMesh
    ( unsigned int sampleRate )
{
    if( mySampleRate != sampleRate )
    {
        mySampleRate = sampleRate;
        mySlicesMesh.reset( createSlicesMesh( sampleRate ) );

        std::stringstream msg;
        msg << "RayMarchingStage: Created new slices mesh with " << sampleRate << " samples per pixel.";
        base::Log::instance().record( base::Log::debug, msg.str() );
    }
    return *mySlicesMesh;
}



// ----------------------------------------------------------------------------------
// RayMarchingStage
// ----------------------------------------------------------------------------------

RayMarchingStage::RayMarchingStage( unsigned int geometryType )
    : base::GeometryStage< base::Renderable::BackToFront >::GeometryStage( geometryType )
    , pimpl( new Details() )
{
}


RayMarchingStage::~RayMarchingStage()
{
    activateGLContext();
    if( vr.get() != nullptr )
    {
        /* Release main shader.
         */
        base::ShaderManager::instance().releaseShader( vr->shader );

        /* Release texture samplers.
         */
        for( auto samplerItr = vr->samplers.begin(); samplerItr != vr->samplers.end(); ++samplerItr )
        {
            delete samplerItr->second;
        }
    }
}


void RayMarchingStage::render( const base::Renderable& renderable )
{
    using base::math::Matrix4f;
    using base::math::Vector4f;

    /* Hereinafter the term 'model' is identified with 'segment'.
     */
    const Matrix4f& modelView = renderable.modelViewTransform();

    /* Compute the directional vector from eye to segment center.
     * This vector needs to be renormalized since 'viewModel' may contain scalings.
     */
    const Matrix4f viewModel = modelView.inverse();
    const Vector4f viewDirectionInModelSpace = ( viewModel * Vector4f( 0, 0, -1, 0 ) ).normalized();

    /* Construct billboard at segment center, i.e. plane that always faces the camera.
     */
    const float s = std::sqrt( 2.f ) / 2;
    const Vector4f modelNormal    = s * -viewDirectionInModelSpace;
    const Vector4f modelTangent   = s * ( viewModel * Vector4f( 1, 0, 0, 0 ) ).normalized();
    const Vector4f modelBitangent = s * ( viewModel * Vector4f( 0, 1, 0, 0 ) ).normalized();
    const Matrix4f tangentModel   = base::math::basis4f( modelTangent, modelBitangent, modelNormal );
    
    /* Bind all 'Texture3D' geometry features.
     */
    unsigned int lastUnit = base::Texture3D::SETUP_UNIT;
    std::vector< unsigned int > roles;
    const base::Texture3D* anyTexture;
    renderable.geometry().visitFeatures( [&]( base::GeometryFeature& gf, unsigned int role )
        {
            if( dynamic_cast< base::Texture3D* >( &gf ) != nullptr )
            {
                const base::Texture3D& texture = static_cast< const base::Texture3D& >( gf );
                anyTexture = &texture;
                videoResource( texture ).bind( ++lastUnit );
                vr->samplers[ role ]->bind( lastUnit );
                roles.push_back( role );
            }
        }
    );

    /* We assume here that the texture coordinates correction is same for all
     * textures, i.e. all textures have same resolution.
     */
    const base::math::Matrix4f modelTexture =
        ( anyTexture == nullptr ? base::math::identity4f() : anyTexture->textureCoordinatesCorrection )
        * base::math::translation4f( 0.5f, 0.5f, 0.5f );
        
    /* Configure shader.
     */
    base::ShaderUniform< base::math::Matrix4f >( "modelViewProjection", pimpl->renderTask->projection * modelView ).upload();
    base::ShaderUniform< base::math::Matrix4f >( "modelTexture", modelTexture ).upload();
    base::ShaderUniform< base::math::Matrix4f >( "tangentModel", tangentModel ).upload();
    for( unsigned int samplerOffset = 0; samplerOffset < roles.size(); ++samplerOffset )
    {
        const unsigned int role = roles[ samplerOffset ];
        const unsigned int unit = base::Texture3D::SETUP_UNIT + 1 + samplerOffset;
        const std::string& uniformName = this->uniformName( role );
        base::ShaderUniform< int >( uniformName, unit ).upload();
    }

    /* Invoke shader.
     */
    VideoResources::SlicesMesh::VideoResourceAcquisition& slicesMesh = vr->slicesMesh( pimpl->sampleRate );
    slicesMesh.render();
}


void RayMarchingStage::loadVideoResources()
{
    const base::ShaderProgram& shader = loadShader();
    vr.reset( new VideoResources( shader, pimpl->sampleRate ) );
    createSamplers( [&]( unsigned int role, base::Sampler* sampler )
        {
            CARNA_ASSERT( vr->samplers.find( role ) == vr->samplers.end() );
            vr->samplers[ role ] = sampler;
        }
    );
}


void RayMarchingStage::renderPass
    ( const base::math::Matrix4f& vt
    , base::RenderTask& rt
    , const base::Viewport& vp )
{
    if( vr.get() == nullptr )
    {
        loadVideoResources();
    }

    rt.renderer.glContext().setShader( vr->shader );
    configureShader();

    pimpl->renderTask = &rt;
    pimpl->viewPort = &vp;
    
    /* Configure proper OpenGL state.
     */
    base::RenderState rs;
    rs.setDepthWrite( false );
    
    /* Do the rendering.
     */
    base::GeometryStage< base::Renderable::BackToFront >::renderPass( vt, rt, vp );

    /* There is no guarantee that 'renderTask' will be valid later.
     */
    pimpl->renderTask = nullptr;
}


void RayMarchingStage::setSampleRate( unsigned int sampleRate )
{
    CARNA_ASSERT( sampleRate >= 2 );
    pimpl->sampleRate = sampleRate;
}


unsigned int RayMarchingStage::sampleRate() const
{
    return pimpl->sampleRate;
}



}  // namespace Carna :: presets

}  // namespace Carna
