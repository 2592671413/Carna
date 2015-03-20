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

#ifndef CUTTINGPLANESSTAGE_H_6014714286
#define CUTTINGPLANESSTAGE_H_6014714286

#include <Carna/base/GeometryStage.h>
#include <Carna/base/Renderable.h>
#include <Carna/Carna.h>

/** \file   CuttingPlanesStage.h
  * \brief  Defines \ref Carna::presets::CuttingPlanesStage.
  */

namespace Carna
{

namespace presets
{



// ----------------------------------------------------------------------------------
// CuttingPlanesStage
// ----------------------------------------------------------------------------------

/** \brief
  * Renders cutting planes of volume geometries in the scene.
  *
  * \image html CuttingPlanesStageTest/threePlanes.png "exemplary rendering of three interleaved cutting planes"
  *
  * \author Leonid Kostrykin
  * \date   22.2.15 - 20.3.15
  */
class CARNA_LIB CuttingPlanesStage : public base::GeometryStage< void >
{

    struct Details;
    const std::unique_ptr< Details > pimpl;

    struct VideoResources;
    std::unique_ptr< VideoResources > vr;

public:

    const static unsigned int DEFAULT_WINDOWING_WIDTH;
    const static    base::HUV DEFAULT_WINDOWING_LEVEL;
    const static unsigned int ROLE_HU_VOLUME;

    CuttingPlanesStage( unsigned int volumeGeometryType, unsigned int planeGeometryType );

    virtual ~CuttingPlanesStage();

    virtual void renderPass
        ( const base::math::Matrix4f& viewTransform
        , base::RenderTask& rt
        , const base::Viewport& vp ) override;

    void setWindowingLevel( base::HUV windowingLevel );

    void setWindowingWidth( unsigned int windowingWidth );

    void setRenderingInverse( bool inverse );

    base::HUV windowingLevel() const;

    unsigned int windowingWidth() const;

    base::HUV minimumHUV() const;

    base::HUV maximumHUV() const;

    bool isRenderingInverse() const;

protected:
    
    virtual void buildRenderQueues( base::Node& root, const base::math::Matrix4f& viewTransform ) override;

    virtual void rewindRenderQueues() override;

    virtual void updateRenderQueues( const base::math::Matrix4f& viewTransform ) override;

    virtual void render( const base::Renderable& ) override;

}; // CuttingPlanesStage



}  // namespace Carna :: presets

}  // namespace Carna

#endif // CUTTINGPLANESSTAGE_H_6014714286
