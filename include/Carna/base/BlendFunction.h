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

#ifndef BLENDFUNCTION_H_6014714286
#define BLENDFUNCTION_H_6014714286

/** \file   BlendFunction.h
  * \brief  Defines \ref Carna::base::BlendFunction.
  */

#include <Carna/Carna.h>

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// BlendFunction
// ----------------------------------------------------------------------------------

/** \brief
  * Defines how alpha blending is to be performed.
  *
  * \author Leonid Kostrykin
  * \date   22.2.15 - 17.3.15
  */
class CARNA_LIB BlendFunction
{

public:

    /** \brief
      * Creates blend function with \a sourceFactor and \a destinationFactor.
      *
      * \attention
      * The arguments \a sourceFactor and \a destinationFactor do not accept
      * arbitrary values. Instead, use constants like `GL_ONE` and `GL_SRC_ALPHA`.
      */
    BlendFunction( int sourceFactor, int destinationFactor );

    /** \brief
      * Holds the source factor.
      */
    int sourceFactor;

    /** \brief
      * Holds the destination factor.
      */
    int destinationFactor;
    
    /** \brief
      * Tells whether this function equals \a other.
      */
    bool operator==( const BlendFunction& other ) const;

}; // BlendFunction



}  // namespace Carna :: base

}  // namespace Carna

#endif // BLENDFUNCTION_H_6014714286
