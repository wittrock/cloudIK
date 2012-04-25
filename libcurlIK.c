#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define ROS_IP 50.17.72.223

struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

int main(void)
{
  CURL *curl;
  CURLcode res;
  double joints_in[7];

  curl = curl_easy_init();
  if(curl) {
    struct string s;
    init_string(&s);

    curl_easy_setopt(curl, CURLOPT_URL, "http://50.17.72.223/cgi-bin/cloudIKCGI.cgi?link=r_wrist_roll_link&x=0.45&y=-0.188&z=0&roll=0&pitch=0&yaw=0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    res = curl_easy_perform(curl);

    sscanf(s.ptr,"%lf %lf %lf %lf %lf %lf %lf",&joints_in[0],&joints_in[1],&joints_in[2],&joints_in[3],&joints_in[4],&joints_in[5],&joints_in[6]);
    free(s.ptr);

    int i;
    for(i = 0; i < 7; i++){
      printf("%lf\n",joints_in[i]);
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return 0;
}
