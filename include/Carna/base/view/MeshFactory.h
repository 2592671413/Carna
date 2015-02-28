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

#ifndef MESHFACTORY_H_6014714286
#define MESHFACTORY_H_6014714286

#include <Carna/base/view/VertexAttributes.h>
#include <Carna/base/view/VertexBuffer.h>
#include <Carna/base/view/IndexBuffer.h>
#include <Carna/base/view/Mesh.h>
#include <Carna/base/math.h>

/** \file   MeshFactory.h
  * \brief  Defines \ref Carna::base::view::MeshFactory.
  */

namespace Carna
{

namespace base
{

namespace view
{



// ----------------------------------------------------------------------------------
// MeshFactory
// ----------------------------------------------------------------------------------

template< typename Vertex >
class MeshFactory
{

    template< typename Vector >
    static Vertex vertex( const Vector& );

public:

    static Mesh< Vertex, uint8_t >& createBox( float sizeX, float sizeY, float sizeZ );

}; // MeshFactory


template< typename Vertex >
template< typename Vector >
Vertex MeshFactory< Vertex >::vertex( const Vector& v )
{
    Vertex vertex;
    vertex.x = v.x();
    vertex.y = v.y();
    vertex.z = v.z();
    return vertex;
}


template< typename Vertex >
Mesh< Vertex, uint8_t >& MeshFactory< Vertex >::createBox( float sizeX, float sizeY, float sizeZ )
{
    typedef Mesh< Vertex, uint8_t > MeshInstance;
    MeshInstance& mesh = MeshInstance::create( IndexBufferBase::PRIMITIVE_TYPE_TRIANGLES );

    const math::Matrix4f baseTransform = math::scaling4f( sizeX / 2, sizeY / 2, sizeZ / 2 );

    /* Define faces.
     */
    math::Matrix4f transforms[ 6 ];
    transforms[ 0 ] = math::basis4f( math::Vector3f(  0,  0, +1 ), math::Vector3f(  0, +1,  0 ), math::Vector3f( -1,  0,  0 ) );  // left
    transforms[ 1 ] = math::basis4f( math::Vector3f(  0,  0, -1 ), math::Vector3f(  0, +1,  0 ), math::Vector3f( +1,  0,  0 ) );  // right
    transforms[ 2 ] = math::basis4f( math::Vector3f( +1,  0,  0 ), math::Vector3f(  0, +1,  0 ), math::Vector3f(  0,  0, +1 ) );  // front
    transforms[ 3 ] = math::basis4f( math::Vector3f( -1,  0,  0 ), math::Vector3f(  0, +1,  0 ), math::Vector3f(  0,  0, -1 ) );  // back
    transforms[ 4 ] = math::basis4f( math::Vector3f( +1,  0,  0 ), math::Vector3f(  0,  0, -1 ), math::Vector3f(  0, +1,  0 ) );  // top
    transforms[ 5 ] = math::basis4f( math::Vector3f( +1,  0,  0 ), math::Vector3f(  0,  0, +1 ), math::Vector3f(  0, -1,  0 ) );  // bottom

    const std::size_t verticesCount = 6 * 4;
    const std::size_t indicesCount  = 6 * 2 * 3;

    Vertex              vertices[ verticesCount ];
    MeshInstance::Index  indices[  indicesCount ];

    int lastVertex = -1;
    int lastIndex  = -1;

    /* Create vertices and indices.
     */
    for( unsigned int faceIndex = 0; faceIndex < 6; ++faceIndex )
    {
        const math::Matrix4f transform = baseTransform * transforms[ faceIndex ];
        vertices[ ++lastVertex ] = vertex( transform * math::Vector4f( -1, -1, 1, 1 ) );
        vertices[ ++lastVertex ] = vertex( transform * math::Vector4f( +1, -1, 1, 1 ) );
        vertices[ ++lastVertex ] = vertex( transform * math::Vector4f( +1, +1, 1, 1 ) );
        vertices[ ++lastVertex ] = vertex( transform * math::Vector4f( -1, +1, 1, 1 ) );

        indices[ ++lastIndex ] = lastVertex - 3;
        indices[ ++lastIndex ] = lastVertex - 2;
        indices[ ++lastIndex ] = lastVertex;

        indices[ ++lastIndex ] = lastVertex;
        indices[ ++lastIndex ] = lastVertex - 2;
        indices[ ++lastIndex ] = lastVertex - 1;
    }

    mesh.vertexBuffer().copy( vertices, verticesCount );
    mesh. indexBuffer().copy(  indices,  indicesCount );
    return mesh;
}



}  // namespace Carna :: base view

}  // namespace Carna :: base

}  // namespace Carna

#endif // MESHFACTORY_H_6014714286
