#version 330

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

uniform sampler3D huVolume;
uniform float     minIntensity;
uniform float     maxIntensity;

in vec4 modelSpaceCoordinates;

out vec4 gl_FragColor;


// ----------------------------------------------------------------------------------
// Basic Sampling
// ----------------------------------------------------------------------------------

float intensityAt( vec3 p )
{
    return texture( huVolume, p ).r;
}


// ----------------------------------------------------------------------------------
// Fragment Procedure
// ----------------------------------------------------------------------------------

void main()
{
    if( abs( modelSpaceCoordinates.x ) > 0.5 || abs( modelSpaceCoordinates.y ) > 0.5 || abs( modelSpaceCoordinates.z ) > 0.5 )
    {
        discard;
    }

    float intensity = intensityAt( modelSpaceCoordinates.xyz + 0.5 );
    float f = step( minIntensity, intensity ) * ( 1 - step( maxIntensity, intensity ) ) * ( intensity - minIntensity ) / ( maxIntensity - minIntensity );
          f = f + step( maxIntensity, intensity );
    
    gl_FragColor = vec4( f, f, f, 1 );
}
