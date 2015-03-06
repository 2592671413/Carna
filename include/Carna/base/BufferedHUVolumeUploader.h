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

#ifndef BUFFEREDHUVOLUMEUPLOADER_H_6014714286
#define BUFFEREDHUVOLUMEUPLOADER_H_6014714286

#include <Carna/Carna.h>
#include <Carna/base/UInt16HUVolume.h>
#include <Carna/base/CarnaException.h>

/** \file   BufferedHUVolumeUploader.h
  * \brief  Defines \ref Carna::base::BufferedHUVolumeUploader.
  */

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// BufferedHUVolumeUploader
// ----------------------------------------------------------------------------------

template< typename BufferedHUVolume >
struct BufferedHUVolumeUploader
{

    static void upload( const BufferedHUVolume& src, Texture3D& dst )
    {
        static_assert( false, "Unimplemented BufferedHUVolumeUploader<T> type parameter T." );
    }

}; // BufferedHUVolumeUploader


void CARNA_LIB uploadUInt16HUVolume( const UInt16HUVolume& src, Texture3D& dst );


template< >
struct CARNA_LIB BufferedHUVolumeUploader< UInt16HUVolume >
{

    static void upload( const UInt16HUVolume& src, Texture3D& dst )
    {
        uploadUInt16HUVolume( src, dst );
    }

}; // BufferedHUVolumeUploader< UInt16HUVolume >



}  // namespace Carna :: base

}  // namespace Carna

#endif // BUFFEREDHUVOLUMEUPLOADER_H_6014714286