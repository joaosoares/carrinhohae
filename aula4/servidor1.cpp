// servidor1.cpp

#include "projeto.hpp"
#include <cekraspicam.h>

int main(void) {
  SERVER server;
  CEKRASPICAM cam(240, 320);
  Mat_<COR> image, resized;
  uint32_t verification;
  bool finish = false;
  
  server.waitConnection();
  
  int ch=-1;

  cam >> image;
  while (finish == false) {
	  server.sendUint(image.rows);
	  server.receiveUint(verification);
	  finish = verification == image.rows;
	  if (!finish) server.sendUint(321);
	  else server.sendUint(123);
  }  
  finish = false; 
  while (finish == false) {
	server.sendUint(image.cols);
	server.receiveUint(verification);
	finish = verification == image.cols;
	if (!finish) server.sendUint(321);
	else server.sendUint(123);
  }
  
  finish = false;
  string numero = "8";
  while (finish == false) {
    cam >> image;
    if(verification != 0) putText(image, numero, cv::Point(280, 50),  cv::FONT_HERSHEY_TRIPLEX, 1.5, CV_RGB(214, 34, 70), 4);
    server.sendImgComp(image);
    server.receiveUint(verification);
    finish = verification == 1234567890;
    numero = std::to_string(verification);
  }
  std::cout << "Finished connection" << std::endl;
}
