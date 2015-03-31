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

#ifndef VERTEX_H_6014714286
#define VERTEX_H_6014714286

#include <Carna/Carna.h>
#include <Carna/base/VertexAttributes.h>

/** \file   Vertex.h
  * \brief  Defines \ref Carna::base::VertexBase.
  */

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// VertexBase
// ----------------------------------------------------------------------------------

/** \brief
  * Defines simple-most vertex that only consists of a positional attribute.
  *
  * \section CustomVertexFormats Custom Vertex Formats
  *
  * It is easy to define custom vertex formats. The procedure is best explained with
  * an example. Lets assume you want to define a vertex that has additional
  * properties for normal vectors and 2D texture coordinates.
  *
  * The first step is to define the missing vertex components. The \ref VertexNormal
  * type already provides a vertex component for normal vectors, so lets define a
  * component for 2D texture coordinates:
  *
  *     \code
  *     struct VertexTexCoord2
  *     {
  *         float u, v;
  *     };
  *     \endcode
  *
  * It is necessary that a vertex component is implemented as a POD, i.e. *plain old
  * data type*. Virtual methods would mess up the memory layout. However, you
  * might define a constructor that initializes default values, if you wanted.
  *
  * The next step is to compose the vertex format:
  *
  *     \code
  *     using namespace Carna::base;
  *     struct LightedTexturedVertex
  *         : public VertexBase
  *         , public VertexNormal
  *         , public VertexTexCoord2
  *     {
  *         static const VertexAttributes attributes;
  *     };
  *     \endcode
  *
  * The order of the base classes is arbitrary, but it must be consistent with what
  * comes next, namely the specification of the vertex format.
  *
  *     \code
  *     #include <vector>
  *     using namespace Carna::base;
  *     const VertexAttributes LightedTexturedVertex::attributes = []()->VertexAttributes
  *     {
  *         using Carna::base::VertexAttribute;  // msvc++ requires us to repeat this
  *         std::vector< VertexAttribute > attributes;
  *         attributes.push_back( VertexAttribute( 0, 4, VertexAttribute::TYPE_FLOAT ) );
  *         attributes.push_back( VertexAttribute( 4, 4, VertexAttribute::TYPE_FLOAT ) );
  *         attributes.push_back( VertexAttribute( 8, 2, VertexAttribute::TYPE_FLOAT ) );
  *         return attributes;
  *     }();
  *     \endcode
  *
  * You should read the above like:
  *
  *   - Attribute 1 starts at offset 0 and has 4 components, namely the
  *     positional \c x, \c y, \c z, \c w.
  *   - Attribute 2 starts at offset 4 and has 4 components, namely the
  *     \c nx, \c ny, \c nz, \c nw of the normal vector.
  *   - Attribute 3 starts at offset 8 and has 2 components, namely the
  *     \c u, \c v of the texture coordinates vector.
  *
  * When writing your shader, you must declare the vertex format consistently:
  *
  *     \code
  *     layout( location = 0 ) in vec4 inPosition;
  *     layout( location = 1 ) in vec4 inNormal;
  *     layout( location = 2 ) in vec2 inTexCoord;
  *     \endcode
  *
  * \author Leonid Kostrykin
  * \date   1.9.14 - 10.3.15
  */
struct CARNA_LIB VertexBase
{
    /** \brief
      * Holds the declaration of the \ref CustomVertexFormats "vertex format".
      */
    static const VertexAttributes attributes;

    /** \property x
      * \brief Holds the positional x-component of this vertex.
      *
      * \property y
      * \brief Holds the positional y-component of this vertex.
      *
      * \property z
      * \brief Holds the positional z-component of this vertex.
      *
      * \property w
      * \brief Holds the positional w-component of this vertex.
      *        This will be `1` usually.
      */
    float x, y, z, w;

    /** \brief
      * Initializes position to \f$\left(0, 0, 0, 1\right)\f$.
      */
    VertexBase();
};



// ----------------------------------------------------------------------------------
// VertexNormal
// ----------------------------------------------------------------------------------

/** \brief
  * Defines vertex component for normal vectors.
  * Usage is explained \ref CustomVertexFormats "here".
  *
  * \author Leonid Kostrykin
  * \date   1.9.14 - 10.3.15
  */
struct VertexNormal
{
    /** \property nx
      * \brief Holds the normal vector x-component of this vertex.
      *
      * \property ny
      * \brief Holds the normal vector y-component of this vertex.
      *
      * \property nz
      * \brief Holds the normal vector z-component of this vertex.
      *
      * \property nw
      * \brief Holds the normal vector w-component of this vertex.
      *        This will be `0` usually.
      */
    float nx, ny, nz, nw;
};



// ----------------------------------------------------------------------------------
// VertexColor
// ----------------------------------------------------------------------------------

/** \brief
  * Defines vertex component for colors.
  * Usage is explained \ref CustomVertexFormats "here".
  *
  * \author Leonid Kostrykin
  * \date   1.9.14 - 31.3.15
  */
struct VertexColor
{
    /** \property r
      * \brief Holds the red color component of this vertex.
      *
      * \property g
      * \brief Holds the green color component of this vertex.
      *
      * \property b
      * \brief Holds the blue color component of this vertex.
      *
      * \property a
      * \brief Holds the alpha color component of this vertex.
      */
    float r, g, b, a;
};



// ----------------------------------------------------------------------------------
// ColoredVertex
// ----------------------------------------------------------------------------------

/** \brief
  * Defines vertex that consists of the two attributes position and color.
  *
  * \author Leonid Kostrykin
  * \date   1.9.14 - 31.3.15
  */
struct ColoredVertex
    : public VertexBase
    , public VertexColor
{
    /** \brief
      * Holds the declaration of the \ref CustomVertexFormats "vertex format".
      */
    static const VertexAttributes attributes;
};



}  // namespace Carna :: base

}  // namespace Carna

#endif // VERTEX_H_6014714286
