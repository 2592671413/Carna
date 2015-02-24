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

#ifndef BASEBUFFER_H_6014714286
#define BASEBUFFER_H_6014714286

#include <cstdlib>

/** \file   BaseBuffer.h
  * \brief  Defines \ref Carna::base::view::BaseBuffer.
  */

namespace Carna
{

namespace base
{

namespace view
{



// ----------------------------------------------------------------------------------
// BaseBuffer
// ----------------------------------------------------------------------------------

class CARNA_LIB BaseBuffer
{

    std::size_t mySize;

protected:

    explicit BaseBuffer( unsigned int target );

    void setSize( std::size_t size );

public:

    const unsigned int target;

    const unsigned int id;

    std::size_t size() const;

    void bind() const;

    virtual ~BaseBuffer();

}; // BaseBuffer



}  // namespace Carna :: base view

}  // namespace Carna :: base

}  // namespace Carna

#endif // BASEBUFFER_H_6014714286
