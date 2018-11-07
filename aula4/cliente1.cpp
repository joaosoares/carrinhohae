// cliente2.cpp
// compila cliente2 -c

// #include <cekeikon.h>
#include "projeto.hpp"

int estados[10] = {0, 7, 8, 9, 4, 5, 6, 1, 2, 3};

int estado = 0; // 0=nao_apertado, 1=apertou_botao_1 2=apertou_botao_2
void on_mouse(int event, int c, int l, int flags, void *userdata) {
  if (event == EVENT_LBUTTONDOWN) {
    if (0 <= l && l < 80 && 0 <= c && c < 80)
      estado = 1; // Virar à esquerda
    else if (0 <= l && l < 80 && 80 <= c && c < 160)
      estado = 2; // Ir para frente
    else if (0 <= l && l < 80 && 160 <= c && c < 240)
      estado = 3; // Virar à direita
    else if (80 <= l && l < 160 && 0 <= c && c < 80)
      estado = 4; // Virar acentuadamente à esquerda
    else if (80 <= l && l < 160 && 80 <= c && c < 160)
      estado = 5; // ???
    else if (80 <= l && l < 160 && 160 <= c && c < 240)
      estado = 6; // Virar acentuadamente à direita
    else if (160 <= l && l < 240 && 0 <= c && c < 80)
      estado = 7; // Virar à esquerda dando ré
    else if (160 <= l && l < 240 && 80 <= c && c < 160)
      estado = 8; // Dar ré
    else if (160 <= l && l < 240 && 160 <= c && c < 240)
      estado = 9; // Virar à direita dando ré
    else
      estado = 0;
  } else if (event == EVENT_LBUTTONUP) {
    estado = 0;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    erro("client6 servidorIpAddr\n");
  CLIENT client(argv[1]);
  Mat_<COR> image_cam;
  namedWindow("janela");
  int ch = -1;
  uint32_t nc, nl;
  bool finish = false;
  uint32_t verification;
  COR cinza(221, 244, 212);
  
  COR vermelho(187, 190, 123);
  Mat_<COR> arrow = imread("arrow.png");
  Mat_<COR> arrow_sel = imread("arrow_sel.png");
  Mat_<COR> imagem(240, 240, cinza);

  while (!finish) {
    client.receiveUint(nl);
    client.sendUint(nl);
    client.receiveUint(verification);
    finish = verification == 123;
  }
  finish = false;
  while (!finish) {
    client.receiveUint(nc);
    client.sendUint(nc);
    client.receiveUint(verification);
    finish = verification == 123;
  }
  std::string nomeVideo = "vazio";
  if (argc == 3)
    nomeVideo = argv[2];
  VideoWriter video(nomeVideo, CV_FOURCC('X', 'V', 'I', 'D'), 30, Size(nc, nl));
  image_cam.create(nl, nc);
  Mat_<COR> imagem_final;
  imagem_final.create(240, 240 + nc);
  imagem_final = grudaH(imagem, image_cam, 0, Vec3b(0, 0, 0));
  resizeWindow("janela", 2 * imagem_final.cols, 2 * imagem_final.rows);
  setMouseCallback("janela", on_mouse);
  imagem_final = grudaH(imagem, image_cam, 0, Vec3b(0, 0, 0));
  imshow("janela", imagem_final);

  while ((char)ch != (char)27) {
    imagem.setTo(cinza);

    arrow.copyTo(imagem(cv::Rect(0, 0, arrow.cols, arrow.rows)));
    arrow.copyTo(imagem(cv::Rect(0, 80, arrow.cols, arrow.rows)));
    arrow.copyTo(imagem(cv::Rect(0, 160, arrow.cols, arrow.rows)));
    arrow.copyTo(imagem(cv::Rect(80, 0, arrow.cols, arrow.rows)));
    arrow.copyTo(imagem(cv::Rect(80, 80, arrow.cols, arrow.rows)));
    arrow.copyTo(imagem(cv::Rect(80, 160, arrow.cols, arrow.rows)));
    arrow.copyTo(imagem(cv::Rect(160, 0, arrow.cols, arrow.rows)));
    arrow.copyTo(imagem(cv::Rect(160, 80, arrow.cols, arrow.rows)));
    arrow.copyTo(imagem(cv::Rect(160, 160, arrow.cols, arrow.rows)));
    // for (int i = 0; i < 3; i += 80) {
    //   for (int j = 0; j < 3; j += 80) {
    //     arrow.copyTo(imagem(cv::Rect(i, j, arrow.cols, arrow.rows)));
    //   }
    // }
    if (estado != 0) {
      int l = ((estado - 1) / 3) * 80;
      int c = ((estado - 1) % 3) * 80;
      arrow_sel.copyTo(imagem(cv::Rect(c, l, arrow_sel.cols, arrow_sel.rows)));
    }
    client.receiveImgComp(image_cam);
    if (argc == 3)
      video << image_cam;
    client.sendUint(estados[estado]);

    imagem_final = grudaH(imagem, image_cam, 0, Vec3b(0, 0, 0));
    imshow("janela", imagem_final);
    ch = waitKey(30);
  }
  destroyWindow("janela");
  client.receiveImgComp(image_cam);
  client.sendUint(1234567890);
}
