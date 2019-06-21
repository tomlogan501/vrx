#pragma once

#include <cmath>
#include <exception>
#include <memory>
#include <string>
#include <ignition/math/Pose3.hh>
#include <sdf/sdf.hh>
#include "usv_gazebo_plugins/polyhedron_volume.hh"

namespace buoyancy
{
  /// \brief type of geometry shape
  enum class ShapeType
  {
    None,
    Box,
    Sphere,
    Cylinder
  };

  /// \brief parent shape object for volume objects
  struct ShapeVolume
  {

    /// \brief Default destructor
    virtual ~ShapeVolume() = default;

    /// \brief factory method for shape. Parses a shape object from sdf data
    static std::unique_ptr<ShapeVolume> makeShape(const sdf::ElementPtr sdf);

    /// \brief display string for shape object
    virtual std::string display();

    /// \brief calculates volume + centroid of submerged shape
    /// if the shape is out of water returns Volume{}
    /// @param pose: world pose of volume
    /// @param fluidLevel: height of fluid
    /// @return volume object with volume + centroid (relative to world)
    virtual Volume calculateVolume(const ignition::math::Pose3d& pose,
                                   double fluidLevel) = 0;

    /// \brief type of shape
    ShapeType type;

    /// \brief full volume of object
    double volume;
  };
  typedef std::unique_ptr<ShapeVolume> ShapeVolumePtr;

  /// \brief box shape volume
  struct BoxVolume : public ShapeVolume
  {

    /// \brief Default constructor
    /// @param x: length
    /// @param y: width
    /// @param z: height
    explicit BoxVolume(double x, double y, double z);

    /// \brief display string for box shape
    std::string display() override;

    // Documentation inherited.
    Volume calculateVolume(const ignition::math::Pose3d& pose,
                           double fluidLevel) override;

    /// \brief length
    double x;

    /// \brief width
    double y;

    /// \brief height
    double z;

  private:
    /// \brief polyhedron defining a box
    Polyhedron polyhedron_;
  };

  /// \brief cylinder shape volume
  struct CylinderVolume : public ShapeVolume
  {

    /// \brief Default constructor
    /// @param r: radius
    /// @param l: length
    explicit CylinderVolume(double r, double l);

    /// \brief display string for cylinder shape
    std::string display() override;

    // Documentation inherited.
    Volume calculateVolume(const ignition::math::Pose3d& pose,
                           double fluidLevel) override;

    /// \brief radius of cylinder
    double r;

    /// \brief height of cylinder
    double h;

  private:
    /// \brief polyhedron defining a cylinder
    Polyhedron polyhedron_;
  };

  /// \brief sphere shape volume
  struct SphereVolume : public ShapeVolume
  {

    /// \brief Default constructor
    /// @param r: radius
    explicit SphereVolume(double r);

    /// \brief display string for sphere shape
    std::string display() override;

    // Documentation inherited.
    Volume calculateVolume(const ignition::math::Pose3d& pose,
                           double fluidLevel) override;

    /// \brief radius of sphere
    double r;
  };

  /// \brief custom exception for parsing errors
  struct ParseException : public std::exception
  {
    ParseException(const char* shape, const char* message)
    {
      std::stringstream ss;
      ss << "Parse error for <" << shape << ">: " << message;
      output_ = ss.str();
    }

    const char* what() const throw()
    {
      return output_.c_str();
    }

  private:
    std::string output_;
  };


} // namespace buoyancy
