#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <math.h>


// This node detects pickup in x=1.5 and dropoff in x=4.7,y=5

// Flag variables for detecting pickup and dropoff
int arrivedPick;
int arrivedDrop;

void amclCallback(const geometry_msgs::PoseWithCovarianceStamped msg)
{
  ROS_INFO("HELOOOOOOOOOOOO");

  if(std::abs(msg.pose.pose.position.x - 1.5) < 0.2)
  {
    arrivedPick = 1;
  }

    if((std::abs(msg.pose.pose.position.x - 4.7) < 0.2) && (std::abs(msg.pose.pose.position.y - 5) < 0.2))
  {
    arrivedDrop = 1;
  }

  return;

}

int main( int argc, char** argv )
{
  ros::init(argc, argv, "add_markers2");
  ros::NodeHandle n;
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);

  // Subscribe to odom values
  ros::Subscriber marker_sub = n.subscribe("amcl_pose", 100, amclCallback);

  // Set our initial shape type to be a cube
  uint32_t shape = visualization_msgs::Marker::CUBE;

  // Init flags to 0
  arrivedDrop = 0;
  arrivedPick = 0;

    visualization_msgs::Marker marker;
    // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "map";
    marker.header.stamp = ros::Time::now();

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // Any marker sent with the same namespace and id will overwrite the old one
    marker.ns = "basic_shapes";
    marker.id = 0;

    // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
    marker.type = shape;

    // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
    marker.action = visualization_msgs::Marker::ADD;

    // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
    marker.pose.position.x = 1.5;
    marker.pose.position.y = 0;
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;

    // Set the scale of the marker -- 1x1x1 here means 1m on a side
    marker.scale.x = 0.4;
    marker.scale.y = 0.4;
    marker.scale.z = 0.4;

    // Set the color -- be sure to set alpha to something non-zero!
    marker.color.r = 0.0f;
    marker.color.g = 0.0f;
    marker.color.b = 1.0f;
    marker.color.a = 1.0;

    // Publish the marker at pickup zone
    while (marker_pub.getNumSubscribers() < 1)
    {
      if (!ros::ok())
      {
        return 0;
      }
      ROS_WARN_ONCE("Please create a subscriber to the marker");
      sleep(1);
    }
    marker_pub.publish(marker);


  // Detecting the positions for appearing and disappearing the marker
  while(1)
  {
    if(arrivedPick)
    {
      marker.action = visualization_msgs::Marker::DELETEALL;
      marker_pub.publish(marker);

      // Wait 5 seconds to simulate a pickup
      ros::Duration(5.0).sleep();

      while(1)
      {
        if(arrivedDrop)
        {
          // Appear marker at drop off 
          marker.pose.position.x = 4.7;
          marker.pose.position.y = 5;
          marker.action = visualization_msgs::Marker::ADD;
          marker_pub.publish(marker);

          break;
        }

        // Spin
        ros::spinOnce();
      }

      break;

    }

      // Spin
      ros::spinOnce();

  }

  ros::spin();

  return 0;

  
}