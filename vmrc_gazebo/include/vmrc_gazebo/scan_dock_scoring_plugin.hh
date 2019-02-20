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

#ifndef VMRC_GAZEBO_SCAN_DOCK_SCORING_PLUGIN_HH_
#define VMRC_GAZEBO_SCAN_DOCK_SCORING_PLUGIN_HH_

#include <ros/ros.h>
#include <string>
#include <vector>
#include <gazebo/gazebo.hh>
#include <sdf/sdf.hh>
#include "vmrc_gazebo/ColorSequence.h"
#include "vmrc_gazebo/scoring_plugin.hh"

/// \brief A class to monitor if the color sequence reported matches the color
/// displayed in the light buoy.
class ColorSequenceChecker
{
  /// \brief Constructor.
  /// \param[in] _expectedColors The sequence of expected colors.
  /// \param[in] _rosNameSpace ROS namespace.
  /// \param[in] _rosColorSequenceService The ROS service used to receive
  /// the color submisison.
  public: ColorSequenceChecker(const std::vector<std::string> &_expectedColors,
                               const std::string &_rosNameSpace,
                               const std::string &_rosColorSequenceService);

  /// Enable the ROS submission service.
  public: void Enable();

  /// Disable the ROS submission service.
  public: void Disable();

  /// Whether a team submitted the color sequence or not.
  /// \return True when the submission was received or false otherwise.
  public: bool SubmissionReceived() const;

  /// Wheter a team submitted the color sequence and is correct or not.
  /// \return True when the team submitted the color sequence and it is correct
  /// or false otherwise.
  public: bool Correct() const;

  /// \brief Callback executed when a new color submission is received.
  /// \param[in] _request Contains the submission.
  /// \param[out] _res The Response. Note that this will be true even if the
  /// reported sequence is incorrect.
  private: bool OnColorSequence(
    ros::ServiceEvent<vmrc_gazebo::ColorSequence::Request,
      vmrc_gazebo::ColorSequence::Response> &_event);

  /// \brief The expected color sequence.
  private: std::vector<std::string> expectedSequence;

  /// \brief ROS namespace.
  private: std::string ns;

  /// \brief ROS topic where the color sequence should be sent.
  private: std::string colorSequenceService;

  /// \brief ROS Node handle.
  private: ros::NodeHandle nh;

  /// \brief Service to generate and display a new color sequence.
  private: ros::ServiceServer colorSequenceServer;

  /// \brief Whether the color sequence has been received or not.
  private: bool colorSequenceReceived = false;

  /// \brief Whether the color sequence received is correct or not.
  private: bool correctSequence = false;
};

/// \brief A class to monitor if the vehicle docked in a given bay.
class DockChecker
{
  /// \brief Constructor.
  /// \param[in] _name The name of the checker (only used for debugging).
  /// \param[in] _activationTopic The gazebo topic used to receive notifications
  /// from the "contain" plugin.
  /// \param[in] _minDockTime Minimum amount of seconds to stay docked to be
  /// considered a fully successfull dock.
  /// \param[in] _dockAllowed Whether is allowed to dock in this bay or not.
  /// \param[in] _worldName Gazebo world name.
  public: DockChecker(const std::string &_name,
                      const std::string &_activationTopic,
                      const double _minDockTime,
                      const bool _dockAllowed,
                      const std::string &_worldName);

  /// \brief Whether the robot has docked in this bay or not.
  /// \return True when the robot has docked or false otherwise.
  public: bool Docked() const;

  /// \brief Whether it is allowed to dock in this bay or not.
  public: bool Allowed() const;

  /// \brief Update function that needs to be executed periodically.
  public: void Update();

  /// \brief Callback triggered when the vehicle enters or exits the activation
  /// zone.
  /// \param[in] _msg The current state (0: exiting, 1: entering).
  private: void OnActivationEvent(ConstIntPtr &_msg);

  /// \brief The name of this checker.
  private: std::string name;

  /// \brief The gazebo topic used to receive notifications
  /// from the "contain" plugin.
  private: std::string activationTopic;

  /// \brief Minimum amount of seconds to stay docked to be
  /// considered a fully successfull dock.
  private: double minDockTime;

  /// \brief Whether is allowed to dock in this bay or not.
  private: bool dockAllowed;

  /// \brief Timer used to calculate the elapsed time docked in the bay.
  private: gazebo::common::Timer timer;

  /// \brief Create a node for communication.
  private: gazebo::transport::NodePtr node;

  /// \brief Subscriber to receive notifications from the contain plugin.
  private: gazebo::transport::SubscriberPtr containSub;

  /// \brief Whether the vehicle has successfully docked or not.
  private: bool docked = false;
};

/// \brief A plugin for computing the score of the scan and dock task.
/// This plugin derives from the generic ScoringPlugin class. Check out that
/// plugin for other required SDF elements.
/// This plugin requires the following SDF parameters:
///
/// <robot_namespace>: Optional parameter with the ROS namespace.
/// <color_sequence_service>: Optional paramter with the ROS service used to
/// receive the color submission.
/// <color_1>: Expected first color of the sequence (RED, GREEN, BLUE, YELLOW).
/// <color_2>: Expected second color of the sequence (RED, GREEN, BLUE, YELLOW).
/// <color_3>: Expected third color of the sequence (RED, GREEN, BLUE, YELLOW).
/// <bays>: Contains at least one of the following blocks:
///   <bay>: A bay represents a potential play where a vehicle can dock. It has
///   the following required elements:
///     <name>The name of the bay. This is used for debugging only.
///     <activation_topic>The gazebo topic used to receive notifications
///     from the "contain" plugin.
///     <min_dock_time>Minimum amount of seconds to stay docked to be
///     considered a fully successfull dock.
///     <dockAllowed> Whether is allowed to dock in this bay or not.
///
/// Here's an example:
/// <plugin name="scan_dock_scoring_plugin"
///               filename="libscan_dock_scoring_plugin.so">
///   <!-- Parameters for scoring_plugin -->
///   <vehicle>wamv</vehicle>
///   <task_name>scan_dock</task_name>
///   <initial_state_duration>3</initial_state_duration>
///   <ready_state_duration>3</ready_state_duration>
///   <running_state_duration>300</running_state_duration>
///   <release_joints>
///     <joint>
///       <name>wamv_external_pivot_joint</name>
///     </joint>
///     <joint>
///       <name>wamv_external_riser</name>
///     </joint>
///   </release_joints>
///
///   <!-- Color sequence checker -->
///   <robot_namespace>vrx</robot_namespace>
///   <color_sequence_service>scan_dock/color_sequence</color_sequence_service>
///   <color_1>red</color_1>
///   <color_2>green</color_2>
///   <color_3>blue</color_3>
///
///   <!-- Dock checkers -->
///   <bays>
///     <bay>
///       <name>bay1</name>
///       <activation_topic>/vrx/dock_2018/bay_1/contain</activation_topic>
///       <min_dock_time>10.0</min_dock_time>
///       <dock_allowed>false</dock_allowed>
///     </bay>
///
///     <bay>
///       <name>bay2</name>
///       <activation_topic>/vrx/dock_2018/bay_2/contain</activation_topic>
///       <min_dock_time>10.0</min_dock_time>
///       <dock_allowed>true</dock_allowed>
///     </bay>
///   </bays>
/// </plugin>
class ScanDockScoringPlugin : public ScoringPlugin
{
  // Constructor.
  public: ScanDockScoringPlugin();

  // Documentation inherited.
  private: void Load(gazebo::physics::WorldPtr _world,
                     sdf::ElementPtr _sdf);

  /// \brief Parse all SDF parameters.
  /// \param[in] _sdf SDF elements.
  private: bool ParseSDF(sdf::ElementPtr _sdf);

  /// \brief Callback executed at every world update.
  private: void Update();

  // Documentation inherited.
  private: void OnRunning() override;

  /// \brief Pointer to the update event connection.
  private: gazebo::event::ConnectionPtr updateConnection;

  /// \brief In charge of receiving the team submission and compare it with
  /// the color sequence from the light buoy.
  private: std::unique_ptr<ColorSequenceChecker> colorChecker;

  /// \brief Monitor all the available bays to decide when the vehicle
  /// docks.
  private: std::vector<std::unique_ptr<DockChecker>> dockCheckers;

  /// \brief Whether we have processed the color sequence submission or not.
  private: bool colorSubmissionProcessed = false;
};

#endif
