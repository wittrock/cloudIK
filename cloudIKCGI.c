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
//This is a comment
int main(void)
{
  printf("%s%c%c\n",
	 "Content-Type:text/html;charset=iso-8859-1",13,10);

  char *data;
  double x, y, z, roll, pitch, yaw;
  char link[32];

  data = getenv("QUERY_STRING");

  printf("query: %s\n", data);

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

  printf("%s\n",link);
  printf("%s\n",data);

  if(data == NULL){
    printf("No data");
  }
  else if((c = sscanf(data,"x=%lf&y=%lf&z=%lf&roll=%lf&pitch=%lf&yaw=%lf",&x,&y, &z, &roll, &pitch, &yaw))!=6){
    printf("Data format error\n");
  }
  else{
  printf("Opening pipes\n");

    
  int cgiToRos;
    /*if(access(cgiToRosPath,F_OK) == -1){
      cgiToRos = mkfifo(cgiToRosPath, 0777);
      }*/
    /*int rosToCgi;
    if(access(rosToCGIPath,F_OK) == -1){
      rosToCgi = mkfifo(rosToCGIPath, 0777);
      }*/
    

    
    //if(cgiToRos == -1 ){
    //printf("Pipe 1 error");

    //uncomment
    cgiToRos = open(cgiToRosPath, O_WRONLY);
      if(cgiToRos == -1){
	printf("\nunrecoverable error on pipe 1 #: %d\n", errno);
	//return -1;
      }

      // test
      //printf("testing...\n");
      //int num = write(cgiToRos,data,strlen(data));
      //end test


      //}
      //if(rosToCgi == -1){
      //printf("Pipe 2 error");
      /*rosToCgi = open(rosToCGIPath, O_RDONLY);
      if(rosToCgi == -1){
	printf("\nunrecoverable error on pipe 2 #: %d\n", errno);
	//return -1;
	}*/
      //}

      //if(rosToCgi == -1 || cgiToRos == -1) return -1;


      write(cgiToRos,&link,32);
      write(cgiToRos,&x,sizeof(double));
      write(cgiToRos,&y,sizeof(double));
      write(cgiToRos,&z,sizeof(double));
      write(cgiToRos,&roll,sizeof(double));
      write(cgiToRos,&pitch,sizeof(double));
      write(cgiToRos,&yaw,sizeof(double));

    
    
      close(cgiToRos);
    //close(rosToCgi);
    
    printf("\nPipes closed!");

    printf("\n%lf %lf %lf %lf %lf %lf", x, y, z, roll, pitch, yaw);

    
  }
    
  return 0;
}
