#include <qualisys_ros/RTProtocol.h>
#include <qualisys_ros/RTPacket.h>

#include <string>

#include "ros/ros.h"
#include "tf2/LinearMath/Matrix3x3.h"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2/utils.h"

#include <geometry_msgs/Pose.h>
#include <std_msgs/Float64.h>

#ifdef _WIN32
#define sleep Sleep
#else
#include <unistd.h>
#endif

int main(int argc, char **argv)
{
  ros::init(argc, argv, "qualisys_streaming");
  ros::NodeHandle nh;
  
  ros::Publisher pose_pub = nh.advertise<geometry_msgs::Pose>("pose", 100);
  ros::Publisher yaw_pub = nh.advertise<std_msgs::Float64>("debug/yaw", 100);
  
    try
    {
        CRTProtocol rtProtocol;

        //Example code for how to use discovery calls.
        //if (rtProtocol.DiscoverRTServer(4534, false))
        //{
        //    sleep(1);
        //    const auto numberOfResponses = rtProtocol.GetNumberOfDiscoverResponses();
        //    for (auto index = 0; index < numberOfResponses; index++)
        //    {
        //        unsigned int addr;
        //        unsigned short basePort;
        //        std::string message;
        //        if (rtProtocol.GetDiscoverResponse(index, addr, basePort, message))
        //        {
        //            printf("%2d - %d.%d.%d.%d:%d\t- %s\n", index, 0xff & addr, 0xff & (addr >> 8), 0xff & (addr >> 16), 0xff & (addr >> 24), basePort, message.c_str());
        //        }
        //    }
        //}
        //else
        //{
        //    printf("%s", rtProtocol.GetErrorString());
        //}

        const char           serverAddr[] = "192.168.50.50";
        const unsigned short basePort = 22222;
        const int            majorVersion = 1;
        const int            minorVersion = 19;
        const bool           bigEndian = false;
        
        const std::string    objectName = "waffle_13";

        bool dataAvailable = false;
        bool streamFrames = false;
        unsigned short udpPort = 6734;
        while (ros::ok())
        {
            if (!rtProtocol.Connected())
            {
                if (!rtProtocol.Connect(serverAddr, basePort, &udpPort, majorVersion, minorVersion, bigEndian))
                {
                    printf("rtProtocol.Connect: %s\n\n", rtProtocol.GetErrorString());
                    sleep(1);
                    continue;
                }
            }

            if (!dataAvailable)
            {
                if (!rtProtocol.Read6DOFSettings(dataAvailable))
                {
                    printf("rtProtocol.Read6DOFSettings: %s\n\n", rtProtocol.GetErrorString());
                    sleep(1);
                    continue;
                }
            }

            if (!streamFrames)
            {
                if (!rtProtocol.StreamFrames(CRTProtocol::RateAllFrames, 0, udpPort, NULL, CRTProtocol::cComponent6d))
                {
                    printf("rtProtocol.StreamFrames: %s\n\n", rtProtocol.GetErrorString());
                    sleep(1);
                    continue;
                }
                streamFrames = true;

                printf("Starting to streaming 6DOF data\n\n");
            }

            CRTPacket::EPacketType packetType;

            if (rtProtocol.Receive(packetType, true) == CNetwork::ResponseType::success)
            {
                if (packetType == CRTPacket::PacketData)
                {
                    float fX, fY, fZ;
                    float rotationMatrix[9];

                    CRTPacket* rtPacket = rtProtocol.GetRTPacket();

                    //printf("Frame %d\n", rtPacket->GetFrameNumber());
                    //printf("======================================================================================================================\n");

                    for (unsigned int i = 0; i < rtPacket->Get6DOFBodyCount(); i++)
                    {
                        if (rtPacket->Get6DOFBody(i, fX, fY, fZ, rotationMatrix))
                        {
                            const char* pTmpStr = rtProtocol.Get6DOFBodyName(i);
                            if (pTmpStr)
                            {
                                //printf("%-12s ", pTmpStr);
                                if (objectName.compare(pTmpStr) == 0)
                                {
                                  geometry_msgs::Pose pose;
                                  
                                  pose.position.x = fX/1000.0;
                                  pose.position.y = fY/1000.0;
                                  pose.position.z = fZ/1000.0;
                                  
                                  tf2::Matrix3x3 m(rotationMatrix[0], rotationMatrix[3], rotationMatrix[6], rotationMatrix[1], rotationMatrix[4], rotationMatrix[7], rotationMatrix[2], rotationMatrix[5], rotationMatrix[8]);
                                  ::tf2::Quaternion q;
                                  m.getRotation(q);
                                  
                                  pose.orientation.x = q.x();
                                  pose.orientation.y = q.y();
                                  pose.orientation.z = q.z();
                                  pose.orientation.w = q.w();
                                  
                                  pose_pub.publish(pose);
                                  
                                  std_msgs::Float64 yaw;
                                  yaw.data = tf2::getYaw(pose.orientation);
                                  yaw_pub.publish(yaw);
                                }
                            }
                            //else
                            //{
                            //    printf("Unknown     ");
                            //}
                            
                            //printf("Pos: %9.3f %9.3f %9.3f    Rot: %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f\n",
                            //    fX, fY, fZ, rotationMatrix[0], rotationMatrix[1], rotationMatrix[2],
                            //    rotationMatrix[3], rotationMatrix[4], rotationMatrix[5], rotationMatrix[6], rotationMatrix[7], rotationMatrix[8]);
                        }
                    }
                    //printf("\n");
                    
                    
                }
            }
        }
        rtProtocol.StopCapture();
        rtProtocol.Disconnect();
    }
    catch (std::exception& e)
    {
        printf("%s\n", e.what());
    }
    return 1;
}
