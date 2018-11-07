//camserver1.cpp
#include <cekeikon.h>
#include <cekraspicam.h>
#include "projeto_hae.hpp"

int main(void) {
  SERVER server;
  server.waitConnection();
  CEKRASPICAM cam;
  Mat_<COR> image;

  while (1) {
    printf("Entered loop image\n");
    cam >> image;
    printf("Got image, sending...\n");
    server.sendImgComp(image);
    printf("Sent image\n");
  }  
}
