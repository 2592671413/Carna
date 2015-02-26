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

#ifndef MATH_H_6014714286
#define MATH_H_6014714286

#include <Carna/Carna.h>
#include <algorithm>
#include <cmath>
#include <Eigen/Dense>

#ifdef min
#error MIN macro defined, define NOMINMAX first!
#endif

#ifdef max
#error MAX macro defined, define NOMINMAX first!
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

/** \file   Math.h
  * \brief  Defines \ref Carna::base::Math.
  *
  * \attention
  * If you include \c windows.h before including this file, make sure to have \c NOMINMAX
  * defined before including the \c windows.h header file.
  */

namespace Carna
{

namespace base
{

/** \brief  Provides set of math-related classes and functions.
  *
  * \date   26.1.14 - 26.2.15
  * \author Leonid Kostrykin
  */
namespace math
{

    /** \brief
      * Returns \f$ \min( \max( \f$ \a val \f$, \f$ \a my_min \f$ ), \f$ \a my_max \f$ ) \f$
      *
      * Returns \f$ \left\{ \begin{array}{ll}
      *     \mathrm{max} & \mbox{if $\mathrm{val} > \mathrm{max}$} \\
      *     \mathrm{min} & \mbox{if $\mathrm{val} < \mathrm{min}$} \\
      *     \mathrm{val} & \mbox{else}.\end{array} \right. \f$
      */
    template< typename T >
    inline T clamp( T val, T my_min, T my_max )
    {
        return std::min( std::max( val, my_min ), my_max );
    }

    /** \brief
      * Computes and returns \f$ x^2 \f$
      */
    template< typename T >
    inline T sq( T x )
    {
        return x * x;
    }
    
    /** \brief
      * Converts degrees to radians.
      */
    inline float deg2rad( float deg )
    {
        return deg * 3.1415926f / 180.f;
    }

    /** \brief
      * Converts radians to degrees.
      */
    inline float rad2deg( float rad )
    {
        return 180.f * rad / 3.1415926f;
    }

    /** \brief
      * Defines the maximum difference of two objects treated as equal.
      */
    template< typename T >
    inline T epsilon()
    {
        return static_cast< T >( 0 );
    }

    /** \brief
      * Defines the maximum difference of two single-precision floating point objects treated as equal.
      */
    template< >
    inline float epsilon< float >()
    {
        return 1e-4f;
    }

    /** \brief
      * Defines the maximum difference of two double-precision floating point objects treated as equal.
      */
    template< >
    inline double epsilon< double >()
    {
        return 1e-6;
    }

    /** \brief
      * Retrieves element types of vectors and scalars. General case assumes a scalar type.
      */
    template< typename T >
    struct element_type_of
    {

        /** \brief
          * Since \a T is assumed to be scalar type, it's element type is also \a T.
          */
        typedef typename T type;

    };

    /** \brief
      * Retrieves element types of vectors and scalars. Specialization for \ref Vector class.
      */
    template< typename VectorElementType, unsigned int dimension >
    struct element_type_of< Eigen::Matrix< VectorElementType, dimension, 1 > >
    {
    
        /** \brief
          * The vector element type is known implicitly for each vector type.
          */
        typedef typename VectorElementType type;

    };

#pragma warning( push )
#pragma warning( disable:4146 )

    /** \brief
      * Retrieves the squared length of vector and scalar types. General case assumes scalar type.
      */
    template< typename LengthType, typename InputType >
    inline LengthType length2( const InputType& x )
    {
        return x * x;
    }

#pragma warning( pop )

    /** \brief
      * Retrieves the length of vector and scalar types. Specialization for \ref Vector class.
      */
    template< typename VectorElementType, unsigned int dimension >
    inline VectorElementType length2( const Eigen::Matrix< VectorElementType, dimension, 1 >& x )
    {
        return x.squaredNorm();
    }

    /** \brief
      * Tells whether two objects are equal respectively to \ref epsilon.
      */
    template< typename InputType >
    inline bool isEqual( const InputType& x, const InputType& y )
    {
        typedef element_type_of< InputType >::type ScalarType;
        const InputType difference = x - y;
        const ScalarType distance = length2< ScalarType, InputType >( difference );
        const ScalarType _epsilon = epsilon< ScalarType >();
        return distance <= _epsilon * _epsilon;
    }

    typedef Eigen::Matrix< float, 4, 4, Eigen::ColMajor > Matrix4f;
    typedef Eigen::Matrix< float, 4, 1 > Vector4f;
    typedef Eigen::Matrix< float, 3, 1 > Vector3f;
    typedef Eigen::Matrix< unsigned int, 3, 1 > Vector3ui;

    inline Matrix4f identity4f()
    {
        Matrix4f result;
        result.setIdentity();
        return result;
    }

    inline Matrix4f basis4f( const Vector4f& x, const Vector4f& y, const Vector4f& z, const Vector4f& t = Vector4f( 0, 0, 0, 0 ) )
    {
        Matrix4f result;
        result.col( 0 ) = x;
        result.col( 1 ) = y;
        result.col( 2 ) = z;
        result.col( 3 ) = t;
        for( unsigned int i = 0; i < 3; ++i )
        {
            result( 3, i ) = 0;
        }
        result( 3, 3 ) = 1;
        return result;
    }

    inline Matrix4f translation4f( float x, float y, float z )
    {
        Matrix4f result;
        result.setIdentity();
        result( 0, 3 ) = x;
        result( 1, 3 ) = y;
        result( 2, 3 ) = z;
        return result;
    }

    template< typename Vector >
    Matrix4f translation4f( const Vector& v )
    {
        return translation4f( v.x(), v.y(), v.z() );
    }

    inline Matrix4f scaling4f( float x, float y, float z )
    {
        Matrix4f result;
        result.setIdentity();
        result( 0, 0 ) = x;
        result( 1, 1 ) = y;
        result( 2, 2 ) = z;
        return result;
    }

    template< typename VectorElementType >
    inline Matrix4f scaling4f( const Eigen::Matrix< VectorElementType, 3, 1 >& v )
    {
        return scaling4f( v.x(), v.y(), v.z() );
    }

    inline Matrix4f scaling4f( float uniform_scale_factor )
    {
        return scaling4f( uniform_scale_factor, uniform_scale_factor, uniform_scale_factor );
    }

    inline Matrix4f rotation4f( float x, float y, float z, float radians )
    {
        const float c = std::cos( radians );
        const float s = std::sin( radians );

        Matrix4f result;
        result.setIdentity();

        result( 0, 0 ) = x * x * ( 1 - c ) + c;
        result( 1, 0 ) = y * x * ( 1 - c ) + z * s;
        result( 2, 0 ) = x * z * ( 1 - c ) - y * s;

        result( 0, 1 ) = x * y * ( 1 - c ) - z * s;
        result( 1, 1 ) = y * y * ( 1 - c ) + c;
        result( 2, 1 ) = y * z * ( 1 - c ) + x * s;

        result( 0, 2 ) = x * z * ( 1 - c ) + y * s;
        result( 1, 2 ) = y * z * ( 1 - c ) - x * s;
        result( 2, 2 ) = z * z * ( 1 - c ) + c;

        return result;
    }

    inline float translationDistanceSq( const Matrix4f& m )
    {
        return m( 1, 3 ) * m( 1, 3 ) + m( 2, 3 ) * m( 2, 3 ) + m( 3, 3 ) * m( 3, 3 );
    }

    template< typename Vector >
    Vector& normalized( Vector& v )
    {
        v.normalize();
        return v;
    }

    template< typename Matrix >
    float maxAbsElement( const Matrix& m )
    {
        const std::size_t length = m.rows() * m.cols();
        float maxAbs = 0;
        for( std::size_t i = 0; i < length; ++i )
        {
            maxAbs = std::max( maxAbs, abs( m.data()[ i ] ) );
        }
        return maxAbs;
    }

    Matrix4f frustum4f( float left, float right, float bottom, float top, float zNear, float zFar )
    {
        Matrix4f result;
        result.setZero();

        result( 0, 0 ) = 2 * zNear / ( right - left );
        result( 1, 1 ) = 2 * zNear / ( top - bottom );
        result( 0, 2 ) = ( right + left ) / ( right - left );
        result( 1, 2 ) = ( top + bottom ) / ( top - bottom );
        result( 2, 2 ) = -( zFar + zNear ) / ( zFar - zNear );
        result( 3, 2 ) = -1;
        result( 2, 3 ) = -2 * zFar * zNear / ( zFar - zNear );

        return result;
    }

    Matrix4f frustum4f( float fovRadiansHorizontal, float heightOverWidth, float zNear, float zFar )
    {
        const float halfProjPlaneWidth  = zNear * std::tan( fovRadiansHorizontal );
        const float halfProjPlaneHeight = halfProjPlaneWidth * heightOverWidth;
        return frustum4f( -halfProjPlaneWidth, +halfProjPlaneWidth, -halfProjPlaneHeight, +halfProjPlaneHeight, zNear, zFar );
    }



}  // namespace Carna :: base :: math

}  // namespace Carna :: base

}  // namespace Carna

#endif // MATH_H_6014714286