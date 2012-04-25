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

int main(void)
{
  printf("%s%c%c",
	 "Content-Type:text/html;charset=iso-8859-1",13,10);

  // query data from URL
  char *data;

  // parameters to send to ROS
  double x, y, z, roll, pitch, yaw;
  char link[32];

  // joint values received from ROS
  double in1,in2,in3,in4,in5,in6,in7;

  data = getenv("QUERY_STRING");

  //printf("query: %s\n", data);

  int c = 0;

  int link_string_length = 0;
  int i;
  for(i = 0; i < strlen(data); i++) {
    if(data[i+5] != '&'){
      link[i] = data[i+5];
      link_string_length += 1;
    }
    else {
      link[i] = '\0';
      data += (6 + link_string_length);
      break;
    }
  }

  //printf("%s\n",link);
  //printf("%s\n",data);

  if(data == NULL){
    printf("No data");
  }
  else if((c = sscanf(data,"x=%lf&y=%lf&z=%lf&roll=%lf&pitch=%lf&yaw=%lf",&x,&y, &z, &roll, &pitch, &yaw))!=6){
    printf("Data format error\n");
  }
  else{
    //printf("Opening pipes\n");
    

  ///////////////////////////////////////////////////////////////////////
  // init CGItoROS FIFO stuff
  int cgiToRos = 0;
  if(access(cgiToRosPath,F_OK) == -1){
    //printf("mkfifo cgiToRos\n");
    cgiToRos = mkfifo(cgiToRosPath, S_IFIFO | 0777);
  }    
  if(cgiToRos == -1 )
    printf("mkfifo failed on pipe 1\n");

  //open CGItoROS
  cgiToRos = open(cgiToRosPath, O_WRONLY);
  if(cgiToRos == -1){
    printf("\nunrecoverable error on pipe 1 #: %d\n", errno);
    return -1;
  }
  //else printf("opened CGItoROS");

  // write query values to ROS
  write(cgiToRos,&link,32);
  write(cgiToRos,&x,sizeof(double));
  write(cgiToRos,&y,sizeof(double));
  write(cgiToRos,&z,sizeof(double));
  write(cgiToRos,&roll,sizeof(double));
  write(cgiToRos,&pitch,sizeof(double));
  write(cgiToRos,&yaw,sizeof(double));
  
  close(cgiToRos);
  ///////////////////////////////////////////////////////////////
  // init ROStoCGI FIFO stuff
  int rosToCgi = 0;
  if(access(rosToCGIPath,F_OK) == -1){
    //printf("mkfifo RosToCgi\n");
    rosToCgi = mkfifo(rosToCGIPath, S_IFIFO | 0777);
  }
  if(rosToCgi == -1)
    printf("mkfifo failed on pipe 2\n");

  // open ROStoCGI
  rosToCgi = open(rosToCGIPath, O_RDONLY);
  if(rosToCgi == -1){
    printf("\nunrecoverable error on pipe 2 #: %d\n", errno);
    return -1;
  }
  //else printf("opened ROStoCGI");
   
  read(rosToCgi,&in1,sizeof(double));
  read(rosToCgi,&in2,sizeof(double));
  read(rosToCgi,&in3,sizeof(double));
  read(rosToCgi,&in4,sizeof(double));
  read(rosToCgi,&in5,sizeof(double));
  read(rosToCgi,&in6,sizeof(double));
  read(rosToCgi,&in7,sizeof(double));
  
  close(rosToCgi);
    
  //printf("\nPipes closed!");
  
  printf("\n%lf %lf %lf %lf %lf %lf %lf", in1, in2, in3, in4, in5, in6, in7);

    
  }
    
  return 0;
}
