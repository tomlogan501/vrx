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

#ifndef GAZEBO_PLUGINS_MODELPLUGINTHROTTLED_HH_
#define GAZEBO_PLUGINS_MODELPLUGINTHROTTLED_HH_

#include <limits>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/Time.hh>
#include <gazebo/physics/PhysicsTypes.hh>
#include <sdf/sdf.hh>

namespace gazebo
{
  /// \brief A model plugin class that lets you specify the frequency of
  /// execution of each periodic step.
  /// It accepts the following parameter:
  /// <update_rate> Sets the execution rate of the pluggin (Hz).
  class ModelPluginThrottled : public ModelPlugin
  {
    // Documentation inherited.
    public: virtual void Load(physics::ModelPtr /*_model*/,
                              sdf::ElementPtr _sdf);

    /// \brief Check whether the plugin is ready to execute the next iteration
    /// or not.
    /// \return True when is OK to execute the next iteration or false otherwise
    protected: bool UpdateThrottling();

    /// \brief Update rate of the plugin (Hz).
    /// By default, the plugin will run in sync with the physics engine.
    private: double updateRate = std::numeric_limits<double>::max();

    /// \brief Last time the plugin was executed (used for throttling).
    private: gazebo::common::Time lastUpdateTime;
  };
}

#endif
