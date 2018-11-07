//camclient1.cpp
#include "projeto_hae.hpp"
#include "stdio.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define ESC_KEY 27

int main(int argc, char *argv[]) {
  if (argc!=2){
    perror("camclient1 servidorIpAddr\n");
    exit(1);
  }
  CLIENT client(argv[1]);
  cv::Mat_<cv::Vec3b> img;
  uint8_t ch = 0;
  printf("Entering loop... ch=%d\n", ch);
  while(ch != ESC_KEY) {
    printf("Ran loop... ch=%d\n", ch);
    client.receiveImgComp(img);
    cv::imshow("From RPI", img);
    ch = cv::waitKey(30);
  }
}