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

uniform mat4 sliceTangentModel;
uniform mat4 modelViewProjection;

layout( location = 0 ) in vec4 inPosition;

struct VertexData
{
    vec4 modelSpaceCoordinates;
};

out VertexData vert;


// ----------------------------------------------------------------------------------
// Vertex Procedure
// ----------------------------------------------------------------------------------

void main()
{
    vert.modelSpaceCoordinates = sliceTangentModel * inPosition;
    vec4 clippingCoordinates = modelViewProjection * vert.modelSpaceCoordinates;
    gl_Position = clippingCoordinates;
}
