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

#ifndef DRRStage_H_6014714286
#define DRRStage_H_6014714286

#include <Carna/presets/VolumeRenderingStage.h>
#include <Carna/Carna.h>
#include <memory>

/** \file   DRRStage.h
  * \brief  Defines \ref Carna::presets::DRRStage.
  */

namespace Carna
{

namespace presets
{



// ----------------------------------------------------------------------------------
// DRRStage
// ----------------------------------------------------------------------------------

/** \brief
  * Renders digital radiograph reconstructs of volume geometries in the scene.
  *
  * The `%DRRStage` constructor takes a geometry type parameter:
  *
  * \snippet ModuleTests/DRRStageTest.cpp drr_instantiation
  *
  * The concept of geometry types is explained \ref QuickStart_FrameRenderer "here".
  *
  * \note
  * In the \ref RenderingProcess "rendering process" this stage will usually be
  * inserted \em after such stages that render opaque geometry, like
  * \ref CuttingPlanesStage and \ref OpaqueRenderingStage.
  *
  * The following example code configures the `%DRRStage` s.t. it produces the
  * rendering presented further below:
  *
  * \snippet ModuleTests/DRRStageTest.cpp drr_setup_base
  *
  * The last step is to state that the brightness of any particular pixel shall be
  * *inverse* proportional to the ray attenuation at that pixel:
  *
  * \snippet ModuleTests/DRRStageTest.cpp drr_setup_inverse
  *
  * \image html DRRStageTest/inverted.png "exemplary rendering in inverse-mode from code above"
  *
  * \author Leonid Kostrykin
  * \date   22.2.15 - 11.3.15
  */
class CARNA_LIB DRRStage : public VolumeRenderingStage
{

    struct Details;
    const std::unique_ptr< Details > pimpl;

public:

    const static float      DEFAULT_WATER_ATTENUATION;
    const static float      DEFAULT_BASE_INTENSITY;
    const static base::HUV  DEFAULT_LOWER_THRESHOLD;
    const static base::HUV  DEFAULT_UPPER_THRESHOLD;
    const static float      DEFAULT_UPPER_MULTIPLIER;
    const static bool       DEFAULT_RENDER_INVERSE;

    const static unsigned int ROLE_HU_VOLUME = 0;

    DRRStage( unsigned int geometryType );

    virtual ~DRRStage();

    virtual void reshape( const base::FrameRenderer& fr, unsigned int width, unsigned int height ) override;

    virtual void renderPass
        ( const base::math::Matrix4f& viewTransform
        , base::RenderTask& rt
        , const base::Viewport& vp ) override;

    float waterAttenuation() const;

    float baseIntensity() const;

    base::HUV lowerThreshold() const;

    base::HUV upperThreshold() const;

    float upperMultiplier() const;

    /** \brief
      * Tells whether ray attenuation at any particular pixel is *inverse*
      * proportional to the brightness of that pixel or just proportional.
      */
    bool isRenderingInverse() const;

    void setWaterAttenuation( float muWater );

    void setBaseIntensity( float baseIntensity );

    void setLowerThreshold( base::HUV lower );

    void setUpperThreshold( base::HUV upper );

    void setUpperMultiplier( float multiplier );

    /** \brief
      * Sets whether the colors of the DRR should be \a inverse or not. The ray
      * attenuation at a particular pixel is proportional to the brightness of that
      * pixel if \a inverse is `false`.
      *
      * If you set \a inverse to `true`, you will also have to change the
      * \ref base::FrameRenderer::setBackgroundColor "rendering's background color"
      * from default \ref base::Color::BLACK_NO_ALPHA "black" to something brighter,
      * like \ref base::Color::WHITE_NO_ALPHA "white". Otherwise you might not see
      * any rendering results from this stage. Default value is `false`.
      */
    void setRenderingInverse( bool inverse );

protected:

    virtual void loadVideoResources() override;

    virtual void createSamplers( const std::function< void( unsigned int, base::Sampler* ) >& registerSampler ) override;

    virtual const base::ShaderProgram& loadShader() override;

    virtual const std::string& uniformName( unsigned int role ) const override;

    virtual void configureShader() override;

}; // DRRStage



}  // namespace Carna :: presets

}  // namespace Carna

#endif // DRRStage_H_6014714286
