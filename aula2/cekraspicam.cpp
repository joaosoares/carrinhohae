// cekraspicam.cpp
// Exemplo de captura de imagem colorida do raspicam
// compila cekraspicam -c -r
#include <cekeikon.h>
#include <cekraspicam.h>
int main (int argc, char **argv) {
  CEKRASPICAM cam;
  Mat_<COR> image;
  namedWindow("janela");
  int ch=-1;
  while (ch<0) {
    cam >> image;
    imshow("janela",image);
    ch=waitKey(30);
  }
}

