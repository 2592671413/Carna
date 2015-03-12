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

#ifndef BUFFEREDHUVOLUMETEXTURE_H_6014714286
#define BUFFEREDHUVOLUMETEXTURE_H_6014714286

#include <Carna/base/HUVolumeTexture.h>
#include <Carna/base/BufferedHUVolumeUploader.h>
#include <memory>

/** \file   BufferedHUVolumeTexture.h
  * \brief  Defines \ref Carna::base::BufferedHUVolumeTexture.
  */

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// BufferedHUVolumeTexture
// ----------------------------------------------------------------------------------

/** \brief
  * Generic \ref Texture3D specialization that initializes the texture with data from
  * \ref BufferedHUVolume instances.
  *
  * \author Leonid Kostrykin
  * \date 22.2.2015
  */
template< typename BufferedHUVolumeType >
class BufferedHUVolumeTexture : public HUVolumeTexture
{

//protected:
public:

    BufferedHUVolumeTexture( const BufferedHUVolumeType& volume );

public:

    /** \brief
      * Creates new OpenGL texture object and initializes it with data from \a volume.
      * Invoke \ref release when it isn't needed any longer.
      */
    static BufferedHUVolumeTexture< BufferedHUVolumeType >& create( const BufferedHUVolumeType& volume );

    virtual bool controlsSameVideoResource( const GeometryFeature& ) const override;

}; // BufferedHUVolumeTexture


template< typename BufferedHUVolumeType >
BufferedHUVolumeTexture< BufferedHUVolumeType >::BufferedHUVolumeTexture( const BufferedHUVolumeType& volume )
    : HUVolumeTexture( volume )
{
    uploadBufferedHUVolume( volume, *this );
}


template< typename BufferedHUVolumeType >
BufferedHUVolumeTexture< BufferedHUVolumeType >& BufferedHUVolumeTexture< BufferedHUVolumeType >::create( const BufferedHUVolumeType& volume )
{
    return *new BufferedHUVolumeTexture< BufferedHUVolumeType >( volume );
}


template< typename BufferedHUVolumeType >
bool BufferedHUVolumeTexture< BufferedHUVolumeType >::controlsSameVideoResource( const GeometryFeature& other ) const
{
    typedef BufferedHUVolumeTexture< BufferedHUVolumeType > BufferedHUVolume;
    const BufferedHUVolume* const other2 = dynamic_cast< const BufferedHUVolume* >( &other );
    if( other2 == nullptr )
    {
        return false;
    }
    else
    {
        return HUVolumeTexture::controlsSameVideoResource( other );
    }
}



}  // namespace Carna :: base

}  // namespace Carna

#endif // BUFFEREDHUVOLUMETEXTURE_H_6014714286
