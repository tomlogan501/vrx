/*
 * Copyright (C) 2019 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#pragma once

#include <cassert>
#include <vector>
#include <gazebo/common/common.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/math/Quaternion.hh>

namespace buoyancy
{
  using Vec3 = ignition::math::Vector3d;

  /// \brief Represents a plane as a normal and offset
  struct Plane
  {
    /// \brief Initializes plane at z=0
    Plane();

    /// \brief Vector3 normal to plane
    Vec3 normal;

    /// \brief Offset w.r.t. normal
    float offset;
  };

  /// \brief Represents output volume with centroid
  struct Volume
  {
    Volume();

    /// \brief Overloads += for volume object
    Volume& operator+=(const Volume& rhs);

    /// \brief Submerged volume of shape
    double volume;

    /// \brief Vector3 representing volume centroid
    Vec3 centroid;
  };

  /// \brief Submerged volume calculation using polyhedron
  /// based on: Exact Buoyancy for Polyhedra by Eric Catto
  class Polyhedron
  {
    public:

    /// \brief Store vertex index for a triangular face
    struct Face
    {
      Face() = default;

      Face(int i1, int i2, int i3);

      /// \brief Index of vertices
      int i1, i2, i3;
    };

    /// \brief Generate a cube polyhedron centered at origin
    /// @param x: length of cube
    /// @param y: width of cube
    /// @param z: height of cube
    /// @return Polyhedron object
    static Polyhedron makeCube(double x, double y, double z);

    /// \brief Generate a cylinder polyhedron centered at origin
    /// @param r: radius of cylinder
    /// @param l: length of cylinder
    /// @param n: number of segments
    /// @return Polyhedron object
    static Polyhedron makeCylinder(double r, double l, int n);

    /// \brief Compute full volume and center of buoyancy of the polyhedron
    /// @return Volume object with volume and centroid
    Volume computeFullVolume();

    /// \brief Compute submerge volume and center of buoyancy of a polyhedron
    /// @param x: our position
    /// @param q: our orientation (quaternions)
    /// @param plane: water surface defined as a plane
    /// @return Volume object with volume and centroid (relative to world)
    Volume submergedVolume(const Vec3& x, const ignition::math::Quaterniond& q,
        Plane& plane);

    private:
    /// \brief Computes volume and centroid of tetrahedron
    /// tetrahedron formed by triangle + arbitrary point
    /// @param v1: point on triangle
    /// @param v2: point on triangle
    /// @param v3: point on triangle
    /// @param p: arbitrary point
    /// @return Volume object with volume and centroid
    static Volume tetrahedronVolume(const Vec3& v1, const Vec3& v2,
        const Vec3& v3, const Vec3& p = Vec3({0., 0., 0.}));

    /// \brief Clips a partially submerged triangle
    /// @param v1: point on triangle
    /// @param v2: point on triangle
    /// @param v3: point on triangle
    /// @param d1: distance of point v1 to the splitting plane
    /// @param d2: distance of point v2 to the splitting plane
    /// @param d3: distance of point v3 to the splitting plane
    /// @return Volume object for clipped tetrahedron
    static Volume clipTriangle(const Vec3& v1, const Vec3& v2,
        const Vec3& v3, double d1, double d2, double d3,
        const Vec3& p = Vec3({0., 0., 0.}));

    /// \brief Object vertices
    std::vector<Vec3> vertices;

    /// \brief Object faces
    std::vector<Face> faces;

    /// \brief Values below this are zeroed out
    const double EPSILON = 1e-6;
  };  // class Polyhedron

}
