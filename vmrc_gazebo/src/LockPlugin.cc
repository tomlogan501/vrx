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

#include <string>
#include <gazebo/physics/Model.hh>
#include <gazebo/physics/World.hh>

#include "vmrc_gazebo/LockPlugin.hh"

//////////////////////////////////////////////////
void LockPlugin::Load(gazebo::physics::WorldPtr _world, sdf::ElementPtr _sdf)
{
  if (!_sdf->HasElement("model"))
  {
    gzerr << "Unable to find <model> parameter in plugin." << std::endl;
    return;
  }

  std::string modelName = _sdf->Get<std::string>("model");
  auto model = _world->GetModel(modelName);
  if (model)
  {
    gzerr << "Unable to find model name [" << modelName << "]" << std::endl;
    return;
  }

  // Load all the harness joints
  sdf::ElementPtr jointElem = _sdf->GetElement("joint");
  while (jointElem)
  {
    std::string jointName = jointElem->Get<std::string>("name");
    try
    {
      auto joint = model->CreateJoint(jointElem);
    }
    catch(gazebo::common::Exception &_e)
    {
      gzerr << "Unable to load joint [" << jointName << "]. "
            << _e.GetErrorStr()
            << std::endl;
    }

    jointElem = jointElem->GetNextElement("joint");
  }
}
