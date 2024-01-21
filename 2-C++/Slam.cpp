#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Twist.h"
#include "tf/transform_listener.h"
#include "tf/transform_broadcaster.h"
#include "gmapping/slam_gmapping.h"

class SlamNavigation {
public:
    SlamNavigation() : nh_("~") {
        scan_sub_ = nh_.subscribe("scan", 1, &SlamNavigation::scanCallback, this);
        odom_sub_ = nh_.subscribe("odom", 1, &SlamNavigation::odomCallback, this);
        vel_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel", 1);

        // Initialize GMapping
        gmapping_ = new GMapping::SlamGMapping();
        gmapping_->setLaserParameters(360, 0.01745, 0.0, M_PI, 0.0);
        gmapping_->setOdomParameters(0.2, 0.2, 0.2);

        // Other initialization steps for GMapping if needed

        tf_listener_.waitForTransform("base_link", "odom", ros::Time(0), ros::Duration(5.0));
    }

    void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan) {
        // Process laser scan data and update GMapping
        gmapping_->setLaserData(*scan);
        gmapping_->setOdomData(odom_);
        gmapping_->computeMap();

        // Get the transformed pose from base_link to odom
        tf_listener_.lookupTransform("odom", "base_link", ros::Time(0), base_link_transform_);

        // Send navigation commands based on GMapping output
        geometry_msgs::Twist vel_cmd;
        // Implement logic to compute velocity commands based on GMapping output
        // Example: vel_cmd.linear.x = 0.2; vel_cmd.angular.z = 0.1;
        vel_pub_.publish(vel_cmd);
    }

    void odomCallback(const nav_msgs::Odometry::ConstPtr& odom) {
        // Update the stored odometry data
        odom_ = *odom;
    }

private:
    ros::NodeHandle nh_;
    ros::Subscriber scan_sub_;
    ros::Subscriber odom_sub_;
    ros::Publisher vel_pub_;
    tf::TransformListener tf_listener_;
    tf::StampedTransform base_link_transform_;
    nav_msgs::Odometry odom_;
    GMapping::SlamGMapping* gmapping_;
};

int main(int argc, char** argv) {
    ros::init(argc, argv, "slam_navigation_node");
    SlamNavigation slam_navigation;
    ros::spin();
    return 0;
}
