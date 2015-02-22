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

#include <Carna/base/view/Camera.h>

namespace Carna
{

namespace base
{

namespace view
{



// ----------------------------------------------------------------------------------
// Camera
// ----------------------------------------------------------------------------------

void Camera::updateWorldTransform()
{
    Spatial::updateWorldTransform();
    myViewTransform = worldTransform().inverse();
}


const Matrix4f& Camera::viewTransform() const
{
    return myViewTransform;
}


void Camera::setProjection( const Matrix4f& projection )
{
    myProjection = projection;
}


const Matrix4f& Camera::projection() const
{
    return myProjection;
}



}  // namespace Carna :: base :: view

}  // namespace Carna :: base

}  // namespace Carna
