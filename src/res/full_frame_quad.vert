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

layout( location = 0 ) in vec4 inPosition;

out vec2 textureCoordinates;


// ----------------------------------------------------------------------------------
// Vertex Procedure
// ----------------------------------------------------------------------------------

void main()
{
    textureCoordinates = ( inPosition.xy + 1 ) / 2;
    gl_Position = inPosition;
}
