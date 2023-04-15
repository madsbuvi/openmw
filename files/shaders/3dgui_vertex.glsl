#version 120

#include "openmw_vertex.h.glsl"

varying vec2 diffuseMapUV;

void main()
{
    gl_Position = mw_modelToClip(gl_Vertex);
    diffuseMapUV = gl_MultiTexCoord0.xy;
}
