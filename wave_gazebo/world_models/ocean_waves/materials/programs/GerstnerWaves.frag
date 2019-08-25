// Copyright (c) 2016 The UUV Simulator Authors.
// All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Input parameters
uniform sampler2D bumpMap;
uniform samplerCube cubeMap;
uniform sampler2D reflectMap;
uniform sampler2D refractMap;
uniform vec4 deepColor;
uniform vec4 shallowColor;
uniform float fresnelPower;
uniform float hdrMultiplier;

uniform float shallowRefractRatio;
uniform float envReflectRatio;

// Input computed in vertex shader
varying mat3 rotMatrix;
varying vec3 eyeVec;
varying vec2 bumpCoord;

varying vec4 projectionCoord;

void main(void)
{
  // Do the tex projection manually so we can distort _after_
  vec2 final = projectionCoord.xy / projectionCoord.w;

  // Reflection / refraction
  vec4 reflectionColor = texture2D(reflectMap, final);
  vec4 refractionColor = texture2D(refractMap, final);

  // Apply bump mapping to normal vector to make waves look more detailed:
  vec4 bump = texture2D(bumpMap, bumpCoord)*2.0 - 1.0;
  vec3 N = normalize(rotMatrix * bump.xyz);

  // Reflected ray:
  vec3 E = normalize(eyeVec);
  vec3 R = reflect(E, N);
  // Gazebo requires rotated cube map lookup.
  R = vec3(R.x, R.z, R.y);

  // Get environment color of reflected ray:
  vec4 envColor = textureCube(cubeMap, R, 0.0);

  // Cheap hdr effect:
  envColor.rgb *= (envColor.r+envColor.g+envColor.b)*hdrMultiplier;

  // Compute refraction ratio (Fresnel):
  float facing = 1.0 - dot(-E, N);
  float waterEnvRatio = clamp(pow(facing, fresnelPower), 0.0, 1.0);

  // Water color is mix of refraction color, shallow color, and deep color
  vec4 realShallowColor = mix(shallowColor, refractionColor, shallowRefractRatio);
  vec4 waterColor = mix(realShallowColor, deepColor, facing);

  // Environment color is mix of clouds and reflection
  vec4 realEnvColor = mix(envColor, reflectionColor, envReflectRatio);

  // Perform linear interpolation between reflection and refraction.
  vec4 color = mix(waterColor, realEnvColor, waterEnvRatio);
  gl_FragColor = vec4(color.xyz, 0.9);
}
