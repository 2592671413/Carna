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

/** \brief
  * Represents a spatial scene element. It's location is determined relatively to
  * another spatial that is called its *%parent*. This parent-child relationship
  * induces the \ref SceneGraph "scene graph concept".
  *
  * \author Leonid Kostrykin
  * \date   21.2.15 - 6.3.15
  */
class CARNA_LIB Spatial
{

    NON_COPYABLE

    Node* myParent;
    math::Matrix4f myWorldTransform;
    const void* myUserData;
    bool movable;

public:

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /** \brief
      * Instantiates.
      */
    Spatial();
    
    /** \brief
      * Does nothing.
      */
    virtual ~Spatial();
    
    /** \brief
      * Declares an entity that visits mutable \ref Spatial instances.
      */
    typedef std::function< void( Spatial& ) > MutableVisitor;
    
    /** \brief
      * Declares an entity that visits mutable \ref Spatial instances.
      */
    typedef std::function< void( const Spatial& ) > ImmutableVisitor;
    
    /** \brief
      * Tells whether this spatial has a parent node.
      */
    bool hasParent() const;

    /** \brief
      * Detaches this spatial from it's parent node in
      * \f$\mathcal O\left(\log n\right)\f$ where \f$n\f$ is the number of parent's
      * children. The caller takes possession of this spatial.
      *
      * \pre  `hasParent() == true`
      * \post `hasParent() == false`
      *
      * \returns
      * Possessing pointer to this spatial if it has successfully been detached from
      * the parent or \c nullptr if it has no parent.
      */
    Spatial* detachFromParent();

    /** \brief
      * Fixes tree consistency by updating parent of this spatial. This method is for
      * internal usage only.
      */
    void updateParent( Node& parent );
    
    /** \brief
      * References the parent node.
      * \pre `hasParent() == true`
      */
    Node& parent();
    
    /** \overload
      */
    const Node& parent() const;

    /** \brief
      * References the root node this spatial belongs to.
      */
    Node& findRoot();
    
    /** \brief
      * References the root node this spatial belongs to.
      */
    const Node& findRoot() const;
    
    /** \brief
      * Defines the location, rotation and scale of this spatial in relation to it's
      * parent. If this spatial has no parent, the value has no meaning.
      *
      * The default value is \ref math::identity4f.
      */
    math::Matrix4f localTransform;
    
    /** \brief
      * Computes the transformation to world space for this spatial.
      *
      * The default implementation concatenates the parent's world transformation
      * with the \ref localTransform "local transformation" of this spatial.
      */
    virtual void updateWorldTransform();
    
    /** \brief
      * Tells the transformation to world space for this spatial that was last
      * computed.
      */
    const math::Matrix4f& worldTransform() const;

    /** \brief
      * Links an arbitrary object with this \c %Spatial instance.
      *
      * \post
      * <code>hasUserData() == true</code>
      */
    template< typename UserDataType >
    void setUserData( const UserDataType& userData );
    
    /** \brief
      * Removes any object that has been linked with this \c %Spatial instance
      * through \ref setUserData previously.
      *
      * \post
      * <code>hasUserData() == false</code>
      *
      * Does nothing if \ref hasUserData is \c false.
      */
    void removeUserData();

    /** \brief
      * Tells whether an object has been linked with this \c %Spatial instance
      * through \ref setUserData previously.
      */
    bool hasUserData() const;

    /** \brief
      * Retrieves the object previously \ref setUserData "linked" with this
      * \c %Spatial instance.
      *
      * \pre
      * <code>hasUserData()</code>
      */
    template< typename UserDataType >
    const UserDataType& userData() const;

    /** \brief
      * Sets whether this spatial may be displaced w.r.t. it's parent through user
      * interaction. Usually this will be \c false when this spatial represents a
      * \em component of it's parent, like the shaft of an arrow.
      *
      * Each spatial is movable by default.
      */
    void setMovable( bool movable );
    
    /** \brief
      * Tells whether this spatial may be displaced w.r.t. it's parent through user
      * interaction. Usually this will be \c false when this spatial represents a
      * \em component of it's parent, like the shaft of an arrow.
      *
      * Each spatial is movable by default.
      */
    bool isMovable() const;
    
    /** \brief
      * Notifies \ref Node::addNodeListener "all listeners" of its parent nodes that
      * their subtrees have changed. This may include changes of the tree structure
      * as well as updated materials and suchlike.
      */
    virtual void invalidate();

}; // Spatial


template< typename UserDataType >
void Spatial::setUserData( const UserDataType& userData )
{
    myUserData = &userData;
    CARNA_ASSERT( hasUserData() );
}


template< typename UserDataType >
const UserDataType& Spatial::userData() const
{
    CARNA_ASSERT( hasUserData() );
    return *static_cast< const UserDataType* >( myUserData );
}



}  // namespace Carna :: base

}  // namespace Carna

#endif // SPATIAL_H_6014714286
