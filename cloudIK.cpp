#include <ros/ros.h>
#include <kinematics_msgs/GetKinematicSolverInfo.h>
#include <kinematics_msgs/GetPositionIK.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


#define cgiToRosPath "/home/ubuntu/pipes/CGItoROS"
#define rosToCGIPath "/home/ubuntu/pipes/ROStoCGI"


int main(int argc, char **argv){
  int cgiToRos;
  if(access(cgiToRosPath,F_OK) == -1){
    printf("mkfifo cgiToRos\n");
    cgiToRos = mkfifo(cgiToRosPath, S_IFIFO | 0777);
  }
  /*int rosToCgi;
  if(access(rosToCGIPath,F_OK) == -1){
    rosToCgi = mkfifo(rosToCGIPath, 0777);
    }*/

  //while(1){ //open while
  printf("opening fifo\n");
  cgiToRos = open(cgiToRosPath, O_RDONLY);

  // buffers for data from FIFO
  double x_in,y_in,z_in,roll_in,pitch_in,yaw_in;
  char link_in[32];

  // read from FIFO
  read(cgiToRos,&link_in,32);
  read(cgiToRos,&x_in,sizeof(double));
  read(cgiToRos,&y_in,sizeof(double));
  read(cgiToRos,&z_in,sizeof(double));
  read(cgiToRos,&roll_in,sizeof(double));
  read(cgiToRos,&pitch_in,sizeof(double));
  read(cgiToRos,&yaw_in,sizeof(double));

  printf("got %s\n",link_in);
  printf("got %lf\n",x_in);
  printf("got %lf\n",y_in);
  printf("got %lf\n",z_in);
  printf("got %lf\n",roll_in);
  printf("got %lf\n",pitch_in);
  printf("got %lf\n",yaw_in);


  /*if(cgiToRos == -1){
    printf("\nunrecoverable error on pipe 1 #: %d\n", errno);
  }
  else{
    printf("open succeeded: cgiToRos = %d\n",cgiToRos);
    }*/

  /*rosToCgi = open(rosToCGIPath, O_WRONLY);
  if(rosToCgi == -1){
    printf("\nunrecoverable error on pipe 2 #: %d\n", errno);
    }*/

  //if(/*rosToCgi == -1 || */cgiToRos == -1) return -1;

 

  
  ros::init (argc, argv, "get_ik");
  ros::NodeHandle rh;

  ros::service::waitForService("pr2_right_arm_kinematics/get_ik_solver_info");
  ros::service::waitForService("pr2_right_arm_kinematics/get_ik");

  ros::ServiceClient query_client = rh.serviceClient<kinematics_msgs::GetKinematicSolverInfo>("pr2_right_arm_kinematics/get_ik_solver_info");
  ros::ServiceClient ik_client = rh.serviceClient<kinematics_msgs::GetPositionIK>("pr2_right_arm_kinematics/get_ik");

  // define the service messages
  kinematics_msgs::GetKinematicSolverInfo::Request request;
  kinematics_msgs::GetKinematicSolverInfo::Response response;

  if(query_client.call(request,response))
  {
    for(unsigned int i=0; i< response.kinematic_solver_info.joint_names.size(); i++)
    {
      ROS_DEBUG("Joint: %d %s",i,response.kinematic_solver_info.joint_names[i].c_str());
    }
  }
  else
  {
    ROS_ERROR("Could not call query service");
    ros::shutdown();
    exit(1);
  }
  // define the service messages
  kinematics_msgs::GetPositionIK::Request  gpik_req;
  kinematics_msgs::GetPositionIK::Response gpik_res;
  gpik_req.timeout = ros::Duration(5.0);
  gpik_req.ik_request.ik_link_name = link_in;

  gpik_req.ik_request.pose_stamped.header.frame_id = "torso_lift_link";
  gpik_req.ik_request.pose_stamped.pose.position.x = x_in;
  gpik_req.ik_request.pose_stamped.pose.position.y = y_in;
  gpik_req.ik_request.pose_stamped.pose.position.z = z_in;

  gpik_req.ik_request.pose_stamped.pose.orientation.x = roll_in;
  gpik_req.ik_request.pose_stamped.pose.orientation.y = pitch_in;
  gpik_req.ik_request.pose_stamped.pose.orientation.z = yaw_in;
  gpik_req.ik_request.pose_stamped.pose.orientation.w = 1.0;
  gpik_req.ik_request.ik_seed_state.joint_state.position.resize(response.kinematic_solver_info.joint_names.size());
  gpik_req.ik_request.ik_seed_state.joint_state.name = response.kinematic_solver_info.joint_names;
  for(unsigned int i=0; i< response.kinematic_solver_info.joint_names.size(); i++)
  {
    gpik_req.ik_request.ik_seed_state.joint_state.position[i] = (response.kinematic_solver_info.limits[i].min_position + response.kinematic_solver_info.limits[i].max_position)/2.0;
  }
  if(ik_client.call(gpik_req, gpik_res))
  {
    if(gpik_res.error_code.val == gpik_res.error_code.SUCCESS)
      for(unsigned int i=0; i < gpik_res.solution.joint_state.name.size(); i ++)
        ROS_INFO("Joint: %s %f",gpik_res.solution.joint_state.name[i].c_str(),gpik_res.solution.joint_state.position[i]);
    else
      ROS_ERROR("Inverse kinematics failed");
  }
  else
    ROS_ERROR("Inverse kinematics service call failed");

  //} //close while


  ros::shutdown();
}
