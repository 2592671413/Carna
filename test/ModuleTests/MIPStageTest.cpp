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

#include "MIPStageTest.h"
#include <Carna/base/Node.h>
#include <Carna/base/FrameRenderer.h>
#include <Carna/presets/MIPStage.h>
#include <Carna/presets/MIPChannel.h>



// ----------------------------------------------------------------------------------
// MIPStageTest
// ----------------------------------------------------------------------------------

void MIPStageTest::initTestCase()
{
    const unsigned int width  = 640;
    const unsigned int height = 480;

    qglContextHolder.reset( new QGLContextHolder() );
    testFramebuffer.reset( new TestFramebuffer( qglContextHolder->glContext(), width, height ) );
    scene.reset( new TestScene() );
    renderer.reset( new base::FrameRenderer( qglContextHolder->glContext(), width, height, true ) );

    mip = new presets::MIPStage( TestScene::GEOMETRY_TYPE_VOLUMETRIC );
    mip->appendChannel( new presets::MIPChannel( -1024,    0, base::math::Vector4f( 0, 0, 1, 0.5f ) ) );
    mip->appendChannel( new presets::MIPChannel(     0, 3071, base::math::Vector4f( 1, 1, 0, 0.5f ) ) );
    renderer->appendStage( mip );
}


void MIPStageTest::cleanupTestCase()
{
    renderer.reset();
    scene.reset();
    testFramebuffer.reset();
    qglContextHolder.reset();
}


void MIPStageTest::init()
{
    mip->setSampleRate( 100 );
}


void MIPStageTest::cleanup()
{
}


void MIPStageTest::test_channelReplace()
{
    mip->channel( 1 ).setFunction( presets::MIPChannel::CHANNEL_FUNCTION_REPLACE );
    renderer->render( scene->cam(), *scene->root );
    VERIFY_FRAMEBUFFER( *testFramebuffer );
}


void MIPStageTest::test_channelAdditive()
{
    mip->channel( 1 ).setFunction( presets::MIPChannel::CHANNEL_FUNCTION_ADD );
    renderer->render( scene->cam(), *scene->root );
    VERIFY_FRAMEBUFFER( *testFramebuffer );
}
