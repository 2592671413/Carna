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

#ifndef SPATIAL_H_6014714286
#define SPATIAL_H_6014714286

#include <Carna/Carna.h>
#include <Carna/base/math.h>
#include <Carna/base/noncopyable.h>
#include <functional>

/** \file   Spatial.h
  * \brief  Defines \ref Carna::base::Spatial.
  */

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// Spatial
// ----------------------------------------------------------------------------------

class CARNA_LIB Spatial
{

    NON_COPYABLE

    Node* myParent;
    math::Matrix4f myWorldTransform;

public:

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Spatial();

    virtual ~Spatial();
    
    typedef std::function< void( Spatial& ) > MutableVisitor;
    
    typedef std::function< void( const Spatial& ) > ImmutableVisitor;
    
    bool hasParent() const;

    /** \brief
      * Detaches this spatial from it's parent node.
      * The caller takes possession of this spatial.
      *
      * \returns
      * Possessing pointer to this spatial if it has successfully been detached from
      * the parent or \c nullptr if it has no parent.
      */
    Spatial* detachFromParent();

    /** \brief
      * Fixes tree consistency by updating parent of this spatial.
      *
      * This method is only required for internal usage.
      */
    void updateParent( Node& parent );
    
    Node& parent();
    
    const Node& parent() const;
    
    math::Matrix4f localTransform;
    
    /** \brief
      * Computes the transformation to world space for this spatial.
      *
      * The default implementation concatenates the parent's world transformation with
      * the \ref localTransform "local transformation" of this spatial.
      */
    virtual void updateWorldTransform();
    
    /** \brief
      * Tells the transformation to world space for this spatial that was last computed.
      */
    const math::Matrix4f& worldTransform() const;

}; // Spatial



}  // namespace Carna :: base

}  // namespace Carna

#endif // SPATIAL_H_6014714286
