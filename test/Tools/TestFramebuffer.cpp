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

#include "TestFramebuffer.h"
#include <Carna/base/CarnaException.h>
#include <Carna/base/RenderTexture.h>
#include <Carna/base/math.h>
#include <QFileInfo>
#include <regex>

namespace Carna
{

namespace testing
{



// ----------------------------------------------------------------------------------
// FramebufferTester
// ----------------------------------------------------------------------------------

const double TestFramebuffer::DEFAULT_EPSILON = 0.01;


TestFramebuffer::TestFramebuffer( base::GLContext& glContext, unsigned int width, unsigned int height )
    : frame( new QImage( width, height, QImage::Format_RGB888 ) )
    , renderTexture( [&]()->base::RenderTexture*
            {
                glContext.makeActive();
                return new base::RenderTexture( width, height );
            }()
        )
    , fbo( new base::Framebuffer( *renderTexture ) )
    , fboBinding( new base::Framebuffer::Binding( *fbo ) )
    , myEpsilon( DEFAULT_EPSILON )
    , glContext( glContext )
{
}


TestFramebuffer::~TestFramebuffer()
{
    glContext.makeActive();
}


void TestFramebuffer::grabFrame() const
{
    glContext.makeActive();
    base::Framebuffer::MinimalBinding binding( *fbo );

    glReadBuffer( GL_COLOR_ATTACHMENT0_EXT );
    glReadPixels( 0, 0, fbo->width(), fbo->height(), GL_RGB, GL_UNSIGNED_BYTE, frame->bits() );

    *frame = frame->mirrored();
}


void TestFramebuffer::verifyFramebuffer( const std::string& signature ) const
{
    const std::regex expr( ".*::([^:]+)::test_([^\\(]+?)\\(" );
    std::smatch match;
    if( std::regex_search( signature, match, expr ) )
    {
        const std::string filename = std::string( match[ 1 ] ) + "/" + std::string( match[ 2 ] ) + ".png";
        verifyFramebuffer( filename, filename );
    }
    else
    {
        CARNA_FAIL( "Unknown signature format." );
    }
}


void TestFramebuffer::verifyFramebuffer
    ( const std::string& expectedImageFilename
    , const std::string& failureImageOutputFilename ) const
{
    const std::string expectedPath = SOURCE_PATH + "/res/renderings/" + expectedImageFilename;
    const std::string   actualPath = BINARY_PATH + "/test_output/"    + failureImageOutputFilename;

    grabFrame();

    if( !QFileInfo( QString::fromStdString( expectedPath ) ).isFile() )
    {
        if( saveActualResult( *frame, actualPath ) )
        {
            QFAIL( ( "Expected comparison bitmap file missing: \"" + expectedPath +
                "\" Actual results have been written to: \"" + actualPath + "\"" ).c_str() );
        }
        else
        {
            QFAIL( ( "Expected comparison bitmap file missing: \"" + expectedPath +
                "\" Saving actual results FAILED!" ).c_str() );
        }
    }

    const QImage expected( expectedPath.c_str() );
    if( !areSimilar( *frame, expected, myEpsilon ) )
    {
        if( saveActualResult( *frame, actualPath ) )
        {
            QFAIL( ( "Rendered image differs from expected. Result has been written to: " + actualPath ).c_str() );
        }
        else
        {
            QFAIL( ( "Rendered image differs from expected. Result FAILED to be written to: " + actualPath ).c_str() );
        }
    }
}


void TestFramebuffer::setEpsilon( double epsilon )
{
    myEpsilon = epsilon;
}


double TestFramebuffer::epsilon() const
{
    return myEpsilon;
}


bool TestFramebuffer::saveActualResult( const QImage& frame, const std::string& filename )
{
    const QFileInfo actual_path_info( QString::fromStdString( filename ) );
    const QDir actual_path_parent = actual_path_info.dir();
    if( !actual_path_parent.exists() )
    {
        actual_path_parent.mkpath( "." );
    }
    return frame.save( QString::fromStdString( filename ) );
}


bool TestFramebuffer::areSimilar( const QImage& img1, const QImage& img2, double epsilon )
{
    if( img1.width() != img2.width() || img1.height() != img2.height() )
    {
        return false;
    }

    double sqError = 0;
    for( int y = 0; y < img1.height(); ++y )
    for( int x = 0; x < img1.width(); ++x )
    {
        const QRgb rgb1 = img1.pixel( x, y );
        const QRgb rgb2 = img2.pixel( x, y );
        
        const base::math::Vector3i color1( qRed( rgb1 ), qGreen( rgb1 ), qBlue( rgb1 ) );
        const base::math::Vector3i color2( qRed( rgb2 ), qGreen( rgb2 ), qBlue( rgb2 ) );
        
        sqError += ( ( color1 - color2 ).cast< float >() / 255 ).squaredNorm();
    }
    
    const double rms = std::sqrt( sqError / ( img1.width() * img1.height() ) );
    return rms < epsilon;
}



}  // namespace Carna :: testing

}  // namespace Carna
