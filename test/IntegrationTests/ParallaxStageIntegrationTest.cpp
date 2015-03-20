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

#include "ParallaxStageIntegrationTest.h"
#include <Carna/base/Node.h>
#include <Carna/base/Geometry.h>
#include <Carna/base/Vertex.h>
#include <Carna/base/Material.h>
#include <Carna/base/Mesh.h>
#include <Carna/base/MeshFactory.h>
#include <Carna/base/FrameRenderer.h>
#include <Carna/presets/DRRStage.h>
#include <Carna/presets/ParallaxStage.h>



// ----------------------------------------------------------------------------------
// ParallaxStageIntegrationTest
// ----------------------------------------------------------------------------------

void ParallaxStageIntegrationTest::initTestCase()
{
    const unsigned int width  = 640;
    const unsigned int height = 480;

    qglContextHolder.reset( new QGLContextHolder() );
    testFramebuffer.reset( new TestFramebuffer( qglContextHolder->glContext(), width, height ) );
    scene.reset( new TestScene() );
    renderer.reset( new base::FrameRenderer( qglContextHolder->glContext(), width, height, true ) );
    scene->cam().localTransform *= base::math::translation4f( 0, 0, -200 );
    
    /* Create parallax rendering stage.
     */
    parallax = new presets::ParallaxStage( presets::ParallaxStage::aside );
    parallax->setEyeDistance( presets::ParallaxStage::DEFAULT_EYE_DISTANCE * 3 );
    renderer->appendStage( parallax );

    /* Create opaque rendering stage.
     */
    presets::OpaqueRenderingStage* const opaque = new presets::OpaqueRenderingStage( GEOMETRY_TYPE_OPAQUE );
    renderer->appendStage( opaque );

    /* Create volumetric rendering stage.
     */
    presets::DRRStage* const drr = new presets::DRRStage( TestScene::GEOMETRY_TYPE_VOLUMETRIC );
    renderer->appendStage( drr );
    drr->setSampleRate( 100 );
    drr->setWaterAttenuation( 2.f );
    drr->setBaseIntensity( 1.f );
    drr->setLowerThreshold( -400 );
    drr->setUpperThreshold( +400 );
    drr->setUpperMultiplier( 1.5f );

    /* Create and add opaque objects to scene.
     */
    base::MeshBase& boxMesh = base::MeshFactory< base::VertexBase >::createBox( 40, 40, 40 );
    base::Material& material = base::Material::create( "unshaded" );
    material.setParameter( "color", base::Color::RED );
    
    base::Geometry* const box = new base::Geometry( GEOMETRY_TYPE_OPAQUE );
    box->putFeature( presets::OpaqueRenderingStage::ROLE_DEFAULT_MESH, boxMesh );
    box->putFeature( presets::OpaqueRenderingStage::ROLE_DEFAULT_MATERIAL, material );
    scene->root->attachChild( box );

    boxMesh.release();
    material.release();
}


void ParallaxStageIntegrationTest::cleanupTestCase()
{
    renderer.reset();
    scene.reset();
    testFramebuffer.reset();
    qglContextHolder.reset();
}


void ParallaxStageIntegrationTest::init()
{
}


void ParallaxStageIntegrationTest::cleanup()
{
}


void ParallaxStageIntegrationTest::test_aside()
{
    renderer->render( scene->cam(), *scene->root );
    VERIFY_FRAMEBUFFER( *testFramebuffer );
}


void ParallaxStageIntegrationTest::test_interleaved()
{
    QWARN( "not implemented yet" );
}
