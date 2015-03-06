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

#ifndef MIP_CHANNEL_H_6014714286
#define MIP_CHANNEL_H_6014714286

/** \file   Channel.h
  * \brief  Defines \ref Carna::VolumeRenderings::MIP::Channel.
  */

#include <Carna/Carna.h>
#include <Carna/base/noncopyable.h>
#include <Carna/base/Span.h>
#include <Carna/base/math.h>
#include <Carna/base/BlendFunction.h>

namespace Carna
{

namespace VolumeRenderings
{

namespace MIP
{



// ----------------------------------------------------------------------------------
// VolumeRenderings :: MIP :: Channel
// ----------------------------------------------------------------------------------

/** \brief  Defines an HUV range and the way it's visualized.
  *
  * \author Leonid Kostrykin
  * \date   26.7.11 - 26.2.15
  */
class CARNA_LIB Channel
{

    NON_COPYABLE

public:

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    const static base::BlendFunction CHANNEL_FUNCTION_ADD;
    const static base::BlendFunction CHANNEL_FUNCTION_REPLACE;

    /** \brief  Instantiates.
      */
    Channel
        ( const base::Span< base::HUV >& huRange
        , const base::math::Vector4f& color
        , const base::BlendFunction& function = CHANNEL_FUNCTION_REPLACE );

    /** \brief  Instantiates.
      */
    Channel
        ( base::HUV firstHuv, base::HUV lastHuv
        , const base::math::Vector4f& color
        , const base::BlendFunction& function = CHANNEL_FUNCTION_REPLACE );

    base::Span< base::HUV > huRange;

    base::math::Vector4f color;

    const base::BlendFunction& function;

}; // VolumeRenderings :: MIP :: Channel



}  // namespace Carna :: VolumeRenderings :: MIP

}  // namespace Carna :: VolumeRenderings

}  // namespace Carna

#endif // MIP_CHANNEL_H_6014714286
