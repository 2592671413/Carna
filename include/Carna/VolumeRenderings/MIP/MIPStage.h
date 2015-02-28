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

#ifndef MIPSTAGE_H_6014714286
#define MIPSTAGE_H_6014714286

#include <Carna/VolumeRenderings/RayMarchingStage.h>
#include <Carna/Carna.h>
#include <memory>

/** \file   MIPStage.h
  * \brief  Defines \ref Carna::base::view::MIPStage.
  */

namespace Carna
{

namespace VolumeRenderings
{

namespace MIP
{



// ----------------------------------------------------------------------------------
// MIPStage
// ----------------------------------------------------------------------------------

class CARNA_LIB MIPStage : public RayMarchingStage
{

    struct Details;
    const std::unique_ptr< Details > pimpl;

public:

    const static unsigned int ROLE_HU_VOLUME = 0;

    MIPStage( int geometryType );

    virtual ~MIPStage();

    virtual void reshape( const base::view::FrameRenderer& fr, const base::view::Viewport& vp ) override;

    virtual void renderPass
        ( const base::math::Matrix4f& viewTransform
        , base::view::RenderTask& rt
        , const base::view::Viewport& vp ) override;
    
    /** \brief
      * Swaps positions of \a channel with it's successor in the \ref MIP_Channels "channels list".
      */
    void ascendChannel( const Channel& channel );
    
    /** \brief
      * Appends \a channel to the \ref MIP_Channels "channels list" and takes it's ownership.
      */
    void appendChannel( Channel* channel );
    
    /** \brief
      * Removes \a channel from the \ref MIP_Channels "channels list".
      * The ownership is transferred to the caller.
      */
    Channel* removeChannel( const Channel& channel );

    void clearChannels();

protected:

    virtual void createSamplers( const std::function< void( unsigned int, base::view::Sampler* ) >& registerSampler ) override;

    virtual const base::view::ShaderProgram& loadShader() override;

    virtual const std::string& uniformName( unsigned int role ) const override;

    virtual void configureShader( base::view::GLContext& ) override;

}; // MIPStage



}  // namespace Carna :: VolumeRenderings :: MIP

}  // namespace Carna :: VolumeRenderings

}  // namespace Carna

#endif // MIPSTAGE_H_6014714286
