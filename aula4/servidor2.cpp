// servidor2.cpp
// compila servidor2 -c -w -r

#include "projeto.hpp"
#include <cekraspicam.h>
#include <softPwm.h>
#include <wiringPi.h>

void anda(uint32_t comando);

int main(void) {
  SERVER server;
  CEKRASPICAM cam(240, 320);
  Mat_<COR> image, resized;
  uint32_t verification;
  bool finish = false;
  server.waitConnection();
  int ch = -1;
  wiringPiSetup();
  cam >> image;

  if (softPwmCreate(0, 0, 100))
    erro("erro");
  if (softPwmCreate(1, 0, 100))
    erro("erro");
  if (softPwmCreate(2, 0, 100))
    erro("erro");
  if (softPwmCreate(3, 0, 100))
    erro("erro");

  while (finish == false) {
    server.sendUint(image.rows);
    server.receiveUint(verification);
    finish = verification == image.rows;
    if (!finish)
      server.sendUint(321);
    else
      server.sendUint(123);
  }

  finish = false;
  while (finish == false) {
    server.sendUint(image.cols);
    server.receiveUint(verification);
    finish = verification == image.cols;
    if (!finish)
      server.sendUint(321);
    else
      server.sendUint(123);
  }

  finish = false;
  // string numero = "0";
  verification = 0;
  while (finish == false) {
    anda(verification);
    cam >> image;
    // putText(image, numero, cv::Point(10, 50),  cv::FONT_HERSHEY_SIMPLEX, 1.5,
    // CV_RGB(0, 0, 0), 4);
    server.sendImgComp(image);
    server.receiveUint(verification);
		if (verification != 0) printf("Comando recebido: %d\n", verification);
    finish = verification == 1234567890;
    // numero = std::to_string(verification);
  }
  std::cout << "Finished connection" << std::endl;
}

void anda(uint32_t comando) {
  if (comando == 0) {
    softPwmWrite(0, 0);
    softPwmWrite(1, 0);
    softPwmWrite(2, 0);
    softPwmWrite(3, 0);
  } else if (comando == 1) {
    softPwmWrite(0, 100);
    softPwmWrite(1, 0);
    softPwmWrite(2, 60);
    softPwmWrite(3, 0);
  } else if (comando == 2) {
    softPwmWrite(0, 100);
    softPwmWrite(1, 0);
    softPwmWrite(2, 100);
    softPwmWrite(3, 0);
  } else if (comando == 3) {
    softPwmWrite(0, 60);
    softPwmWrite(1, 0);
    softPwmWrite(2, 100);
    softPwmWrite(3, 0);
  } else if (comando == 4) {
    softPwmWrite(0, 0);
    softPwmWrite(1, 70);
    softPwmWrite(2, 70);
    softPwmWrite(3, 0);
  } else if (comando == 5) {
    softPwmWrite(0, 0);
    softPwmWrite(1, 0);
    softPwmWrite(2, 0);
    softPwmWrite(3, 0);
  } else if (comando == 6) {
    softPwmWrite(0, 70);
    softPwmWrite(1, 0);
    softPwmWrite(2, 0);
    softPwmWrite(3, 70);
  } else if (comando == 7) {
    softPwmWrite(0, 0);
    softPwmWrite(1, 100);
    softPwmWrite(2, 0);
    softPwmWrite(3, 60);
  } else if (comando == 8) {
    softPwmWrite(0, 0);
    softPwmWrite(1, 100);
    softPwmWrite(2, 100);
    softPwmWrite(3, 0);
  } else if (comando == 9) {
    softPwmWrite(0, 0);
    softPwmWrite(1, 60);
    softPwmWrite(2, 0);
    softPwmWrite(3, 100);
  }
}
