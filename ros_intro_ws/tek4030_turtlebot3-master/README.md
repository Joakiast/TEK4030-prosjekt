This is a guide for TEK4030 project with the Turtlebot 3 Waffle-pi and

Prerequisites
=============
- Ubuntu 20.04
- ROS Noetic

Details
=======

Each robot has their own computer that is recommended to use with the robots.

### Wifi
SSID: Robotlab
Password: 123Robotlab

### Turtlebots
#### waffle1
* **IP:** 192.168.50.12
* **ssh login:** ubuntu
* **ssh password:** turtlebot

#### waffle2
* **IP:** 192.168.50.13
* **Ssh login:** ubuntu
* **ssh password:** turtlebot

#### burger1
* **IP:** 192.168.50.10
* **ssh login:** ubuntu
* **ssh password:** turtlebot

#### burger2
* **IP:** 192.168.50.11
* **ssh login:** ubuntu
* **ssh password:** turtlebot

Configuration
===============

Motion-capture system
---------------------

We use the *Qualisys Track Manager* software for streaming the robot's position into ROS. There is a computer already set up for streaming. At this computer open the *Qualisys Track Manager* software. Then select the project *tek4030*. To start the streaming push the record button and then start. Now the software has started to stream robot positions. To get the position into ROS, see the next sections.

Turtlebot3
----------

To start the turtlebot robot dint the corresponding laptop for the robot. This laptop is already set up. Then go to the Turtlebot3 manual bring up section, and follow the instructions:

```
https://emanual.robotis.com/docs/en/platform/turtlebot3/bringup/#bringup
```

### Streaming position data

For streaming position data to the ROS laptop clone the following repositories to your workspace:

```
git clone https://github.uio.no/TEK4030/qualisys_streaming.git
git clone https://github.uio.no/TEK4030/qualisys_ros.git
```

The compile the software. Then you can run the following command to see all the objects streamed by the system

```
rosrun qualisys_streaming streaming_test
```

In order to select which robot you want the node receiving position data to pubish position data for, then you need to edit the source code of the node. Edit the file [streaming_node.cpp](https://github.uio.no/TEK4030/qualisys_streaming/blob/master/src/streaming_node.cpp#L56), and on line 56 it should look like this

```c++
const std::string    objectName = "waffle_13";
```

Edit the object name to correspond to the robot you want to use.

Start the node with the following command

```
rosrun qualisys_streaming streaming_node
```

Now the node should publish `geometry_msgs::Pose` messages on the topic `/pose`.

### Angle normalization

To keep an angle within [-pi,pi], you can use this code

```
static double smallestDeltaAngle(const double& x, const double& y)
{
  // From https://stackoverflow.com/questions/1878907/the-smallest-difference-between-2-angles
  return atan2(sin(x - y), cos(x - y));
}
```

### Get Yew from quaternion

To get yaw from a orientation message in ROS you can use the TF library. First include the headerfile

```
#include <tf2/utils.h>
```

Then use the following

```
geometry_msgs::Pose pose;                 // message received from somewhere
double yaw = tf2::getYaw(pose.orientation);
```
