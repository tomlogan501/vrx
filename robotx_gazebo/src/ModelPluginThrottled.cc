/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#include <gazebo/common/Time.hh>
#include <gazebo/physics/PhysicsIface.hh>
#include <gazebo/physics/PhysicsTypes.hh>
#include <gazebo/physics/World.hh>
#include <robotx_gazebo/ModelPluginThrottled.hh>
#include <sdf/sdf.hh>

using namespace gazebo;

/////////////////////////////////////////////////
void ModelPluginThrottled::Load(physics::ModelPtr /*_model*/,
    sdf::ElementPtr _sdf)
{
  if (_sdf->HasElement("update_rate"))
    this->updateRate = _sdf->Get<double>("update_rate");
}

/////////////////////////////////////////////////
bool ModelPluginThrottled::UpdateThrottling()
{
  auto now = gazebo::physics::get_world()->GetSimTime();
  auto dt = (now - this->lastUpdateTime).Double();

  // We probably had a simulation reset.
  if (dt < 0)
  {
    this->lastUpdateTime = now;
    return false;
  }

  // It's not time to run the plugin yet.
  if (dt < 1.0 / this->updateRate)
    return false;

  this->lastUpdateTime = now;
  return true;
}
