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

#ifndef MESH_H_6014714286
#define MESH_H_6014714286

#include <Carna/base/VertexAttributes.h>
#include <Carna/base/VertexBuffer.h>
#include <Carna/base/IndexBuffer.h>
#include <Carna/base/GeometryFeature.h>
#include <memory>
#include <vector>

/** \file   Mesh.h
  * \brief  Defines \ref Carna::base::Mesh.
  */

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// MeshBase
// ----------------------------------------------------------------------------------

/** \brief
  * Format-independent abstract \ref Mesh base class. Each mesh consists of a
  * \ref VertexBuffer, an \ref IndexBuffer and a \ref VertexArrays "vertex array".
  *
  * \see
  * The \ref MeshFactory class contains a few examples.
  *
  * The class maintains one \ref VertexBuffer and one \ref IndexBuffer on an
  * application level. The buffers are created the first time the
  * \ref MeshBase::VideoResourceAcquisition "mesh's video resources" are acquired.
  * The buffers are deleted when the last acquisition is released. The buffer
  * instances are available across all \ref GLContext "OpenGL contexts".
  *
  * The class also maintains one so-called OpenGL vertex array, which, contrary to
  * its name, basically is a conjunction of vertex and index buffer. This is
  * maintained on a per-context level. This is explained further below.
  *
  * \section VertexArrays OpenGL Background on Vertex Arrays
  *
  * OpenGL vertex arrays cannot be shared across OpenGL contexts. Hence we need to
  * create one vertex array per context that it is acquired within.
  *
  * > Any OpenGL object types which are not containers are sharable, as well as Sync
  * > Objects and GLSL Objects (excluding program pipeline objects). All container
  * > objects are not shared between contexts. [1]
  * >
  * > Container objects:
  * > - %Framebuffer Objects
  * > - Program Pipeline
  * > - Transform Feedback Objects
  * > - Vertex Array Objects [2]
  *
  * References:
  *  -# https://www.opengl.org/wiki/OpenGL_Context
  *  -# https://www.opengl.org/wiki/OpenGL_Object#Container_objects
  *
  * \author Leonid Kostrykin
  * \date   22.2.15 - 14.3.15
  */
class CARNA_LIB MeshBase : public GeometryFeature
{

    NON_COPYABLE
    
    struct Details;
    const std::unique_ptr< Details > pimpl;

protected:

    friend class GeometryFeature;

    /** \brief
      * Instantiates.
      */
    MeshBase( unsigned int primitiveType, const VertexAttributes& va );

    /** \brief
      * Deletes.
      */
    virtual ~MeshBase();
    
    /** \brief
      * Creates OpenGL vertex buffer object and fills it with data.
      */
    virtual VertexBufferBase* loadVertexBuffer() = 0;
    
    /** \brief
      * Creates OpenGL index buffer object and fills it with data.
      */
    virtual IndexBufferBase* loadIndexBuffer() = 0;

public:

    virtual bool controlsSameVideoResource( const GeometryFeature& ) const override;
    
    /** \brief
      * Holds the primitive type, like `GL_TRIANGLES`, that should be used when
      * rendering this mesh.
      */
    const unsigned int primitiveType;
    
    /** \brief
      * Holds the vertex format of the vertices contained by the vertex buffer.
      */
    const VertexAttributes vertexAttributes;

    // ------------------------------------------------------------------------------
    // MeshBase :: VideoResourceAcquisition
    // ------------------------------------------------------------------------------
    
    /** \brief
      * Represents an acquisition of video resources from a particular
      * \ref MeshBase "mesh". This realizes the RAII idiom.
      *
      * \author Leonid Kostrykin
      * \date   22.2.15 - 18.3.15
      */
    class CARNA_LIB VideoResourceAcquisition : public GeometryFeature::VideoResourceAcquisition
    {
    
        unsigned int myId;
    
    public:
    
        /** \brief
          * Acquires the video resources from \a mesh.
          *
          * \copydetails GeometryFeature::VideoResourceAcquisition::VideoResourceAcquisition(GeometryFeature&)
          */
        VideoResourceAcquisition( MeshBase& mesh );
    
        /** \copydoc GeometryFeature::VideoResourceAcquisition::~VideoResourceAcquisition()
          */
        virtual ~VideoResourceAcquisition();
    
        /** \brief
          * Tells the ID of the \ref VertexArrays "OpenGL vertex array object".
          */
        unsigned int id() const;

        /** \brief
          * Binds the \ref VertexArrays "vertex array object".
          */
        void bind() const;

        /** \brief
          * Renders the mesh.
          */
        void render() const;

        const VertexBufferBase& vertexBuffer() const;   ///< References the mesh's vertex buffer.
        const  IndexBufferBase&  indexBuffer() const;   ///< References the mesh's index buffer.
        
        VertexBufferBase& vertexBuffer();   ///< References the mesh's vertex buffer.
         IndexBufferBase&  indexBuffer();   ///< References the mesh's index buffer.
    
        /** \brief
          * References the mesh.
          */
        MeshBase& mesh;
    
    }; // MeshBase :: VideoResourceAcquisition
    
    virtual VideoResourceAcquisition* acquireVideoResource() override;

}; // MeshBase



// ----------------------------------------------------------------------------------
// Mesh
// ----------------------------------------------------------------------------------

/** \brief
  * Implements \ref MeshBase class for particular \a VertexType and \a IndexType.
  *
  * \param VertexType specifies the \ref VertexBuffer "vertex buffer" format.
  * \param VertexType specifies the \ref  IndexBuffer  "index buffer" format.
  *
  * \see
  * The \ref MeshFactory class contains a few examples.
  *
  * \author Leonid Kostrykin
  * \date   22.2.15 - 14.3.15
  */
template< typename VertexType, typename IndexType >
class Mesh : public MeshBase
{

    const std::vector< VertexType > vertices;
    const std::vector<  IndexType > indices;

    Mesh( unsigned int primitiveType
        , const VertexType* vertices
        , const std::size_t vertexCount
        , const IndexType* indices
        , const std::size_t indexCount );
    
protected:
    
    virtual VertexBufferBase* loadVertexBuffer() override;
    
    virtual IndexBufferBase* loadIndexBuffer() override;

public:

    typedef VertexType Vertex;  ///< Holds the element type of the vertex buffer.
    typedef  IndexType  Index;  ///< Holds the element type of the  index buffer.

    /** \brief
      * Instantiates. Call \ref release when you do not need the object any longer.
      */
    static Mesh< VertexType, IndexType >& create
        ( unsigned int primitiveType
        , const VertexType* vertices
        , const std::size_t vertexCount
        , const IndexType* indices
        , const std::size_t indexCount );

}; // Mesh


template< typename VertexType, typename IndexType >
Mesh< VertexType, IndexType >::Mesh
        ( unsigned int primitiveType
        , const VertexType* vertices
        , const std::size_t vertexCount
        , const IndexType* indices
        , const std::size_t indexCount )
    : MeshBase( primitiveType, Vertex::attributes )
    , vertices( vertices, vertices + vertexCount )
    , indices (  indices,  indices +  indexCount )
{
}


template< typename VertexType, typename IndexType >
VertexBufferBase* Mesh< VertexType, IndexType >::loadVertexBuffer()
{
    VertexBuffer< VertexType >* const vb = new VertexBuffer< VertexType >();
    vb->copy( &vertices.front(), vertices.size() );
    return vb;
}


template< typename VertexType, typename IndexType >
IndexBufferBase* Mesh< VertexType, IndexType >::loadIndexBuffer()
{
    IndexBuffer< IndexType >* const ib = new IndexBuffer< IndexType >( primitiveType );
    ib->copy( &indices.front(), indices.size() );
    return ib;
}


template< typename VertexType, typename IndexType >
Mesh< VertexType, IndexType >& Mesh< VertexType, IndexType >::create
    ( unsigned int primitiveType
    , const VertexType* vertices
    , const std::size_t vertexCount
    , const IndexType* indices
    , const std::size_t indexCount )
{
    return *new Mesh< Vertex, Index >( primitiveType, vertices, vertexCount, indices, indexCount );
}



}  // namespace Carna :: base

}  // namespace Carna

#endif // MESH_H_6014714286
