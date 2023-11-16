#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/PoseStamped.h"

#include <iostream>
#include <sstream>

#include <tf2/utils.h>

void callback(const geometry_msgs::PoseStamped::ConstPtr& msg){
    ROS_INFO_STREAM("Recieved: " << msg);
}


static double smallestDeltaAngle(const double& x, const double& y)
{
    // From https://stackoverflow.com/questions/1878907/the-smallest-difference-between-2-angles
    return atan2(sin(x - y), cos(x - y));
}


int main (int argc, char **argv){
    ros::init(argc, argv, "sample");
    ros::NodeHandle n;
    ros::Rate loop_rate(10);

    ros::Subscriber sub = n.subscribe("/vrpn_client_node/burger2/pose", 1000, callback);
    ros::Publisher pub = n.advertise<geometry_msgs::Twist>("/burger2/cmd_vel", 1000);
    
    while (ros::ok()){
        
        // Creating Twist message to send to turtlebot
        // Max linear velocity: 0.26, max angular velocity: 1.82
        geometry_msgs::Twist twist;
        twist.linear.x = 0.0;
        twist.linear.y = 0.0;
        twist.linear.z = 0.0;
        twist.angular.x = 0.0;
        twist.angular.y = 0.0;
        twist.angular.z = 0.50;


        //===========formlene=================================

        int k = 0;
        



        geometry_msgs::Pose pose;                 // message received from somewhere
        double yaw = tf2::getYaw(pose.orientation);
     
        pub.publish(twist);

        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
