#ifndef MULTIVIEW_FRAGMENT
#define MULTIVIEW_FRAGMENT

// This file either enables or disables GL_OVR_multiview2 related code.
// For use in fragment shaders

// REQUIREMENT:
// GLSL version: 330 or greater
// GLSL profile: compatibility
// NOTE: If stereo is enabled using Misc::StereoView::shaderStereoDefines, version 330 compatibility (or greater) will be set.

// USAGE:
// Use the mw_stereoAwareSampler2D sampler and mw_stereoAwareTexture2D method to sample stereo aware RTT textures.
// Simply use these instead of sampler2D and texture2D for the appropriate textures.
//
// In OpenMW, most fragment shaders will not need this file.
// This file provides symbols for sampling stereo-aware RTT textures. In mono, these texture uniforms are regular sampler2D,
// while in stereo the same uniforms are sampler2DArray instead. The symbols defined in this file mask this difference, allowing
// automated switching. Simply use mw_stereoAwareSampler2D and mw_stereoAwareTexture2D exactly as you would sampler2D and texture2D()
//
// Using water reflection as an example, your old code for these textures needs only change from
//    uniform sampler2D reflectionMap;
//    ...
//    vec3 reflection = texture2D(reflectionMap, screenCoords + screenCoordsOffset).rgb;
//    
// to
//    uniform mw_stereoAwareSampler2D reflectionMap;
//    ...
//    vec3 reflection = mw_stereoAwareTexture2D(reflectionMap, screenCoords + screenCoordsOffset).rgb;
//

#if @useOVR_multiview

#extension GL_OVR_multiview : require
#extension GL_OVR_multiview2 : require
#extension GL_EXT_texture_array : require

#define mw_stereoAwareSampler2D sampler2DArray
#define mw_stereoAwareTexture2D(texture, uv) texture2DArray(texture, vec3((uv), gl_ViewID_OVR))

#else // useOVR_multiview

#define mw_stereoAwareSampler2D sampler2D
#define mw_stereoAwareTexture2D(texture, uv) texture2D(texture, uv)

#endif // useOVR_multiview

#endif // MULTIVIEW_FRAGMENT