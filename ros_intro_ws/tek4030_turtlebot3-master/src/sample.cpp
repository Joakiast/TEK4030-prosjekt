#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/PoseStamped.h"

#include <iostream>
#include <sstream>

void callback(const geometry_msgs::PoseStamped::ConstPtr& msg){
    ROS_INFO_STREAM("Recieved: " << msg);
}

int main (int argc, char **argv){
    ros::init(argc, argv, "sample");
    ros::NodeHandle n;
    ros::Rate loop_rate(10);

    ros::Subscriber sub = n.subscribe("/vrpn_client_node/waffle1/pose", 1000, callback);
    ros::Publisher pub = n.advertise<geometry_msgs::Twist>("/waffle1/cmd_vel", 1000);
    
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
     
        pub.publish(twist);

        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
