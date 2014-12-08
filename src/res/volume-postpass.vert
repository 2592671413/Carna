#version 120

/*
 *  Copyright (C) 2010 - 2014 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 *
 */
 
uniform mat4 trafo;

void main()
{
    gl_Position = ftransform();
    gl_TexCoord[ 0 ] = trafo * gl_MultiTexCoord0;
}
