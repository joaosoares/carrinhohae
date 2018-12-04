#include <opencv2/opencv.hpp>
#include "projeto.hpp"

using namespace std;
using namespace cv;

#define THRESHOLD 0.75

int main(int argc, char *argv[]) {
  if (argc < 2) {
    perror("cliente4 host\n");
    exit(-1);
  }
  // Pegar nomes
  string nomeMatch = "quadrado.png";
  Mat match = imread(nomeMatch, CV_64FC1);
  CLIENT client(argv[1]);

  // Tenta dar match pra cada frame e salva o resultado na saida
  printf("Comecando loop\n");
  Mat frameEntrada, frameSaida;
  // Cria vetor de imagens redimensionadas
  vector<int> matchSizes{30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 100, 110, 120};
  vector<Mat> resizedMatches(matchSizes.size());
  vector<Mat> results(matchSizes.size());

  for (int i = 0; i < matchSizes.size(); i++) {
    auto size = matchSizes[i];
    resize(match, resizedMatches[i], Size(size, size));
  }
  bool shouldContinue = true;
  while (shouldContinue) {
    entrada.read(frameEntrada);	
    // printf("\n\n\nNew frame\n");

    // Faz match com todos os resized images
    for (int i = 0; i < resizedMatches.size(); i++) {
      auto resizedMatch = resizedMatches[i];
      matchTemplate(frameEntrada, resizedMatch, results[i], CV_TM_CCORR_NORMED);
      printf("Results frameEntrada %d rows, %d cols\n", frameEntrada.rows, frameEntrada.cols);
    }
    // Acha melhor ponto
    Point bestLoc;
    Mat bestResult;
    int size;
    double max = 0;
    for (int i = 0; i < results.size(); i++) {
      auto result = results[i];
      double localMin, localMax;
      Point localMinLoc, localMaxLoc;
      minMaxLoc(result, &localMin, &localMax, &localMinLoc, &localMaxLoc);
      if (localMax > max) {
        bestResult = result;
        max = localMax;
        bestLoc = localMaxLoc;
        size = matchSizes[i];
      }
    }
    printf("Max %.2f\n", max);

    // Pintar quadrado sobre local
    frameEntrada.copyTo(frameSaida);
    if (max > THRESHOLD) {
      rectangle(frameSaida, bestLoc, Point(bestLoc.x + size, bestLoc.y + size), Scalar(0, 0, 255), 3);
    }
    imshow("display", frameSaida);
    if (waitKey(30) > 0) {
      break;
    }
  }
}





int main(int argc, char *argv[]) { 
  CLIENT client(argv[1]);
  Mat_<COR> image_cam; 
  namedWindow("janela");
  int ch=-1;
  uint32_t nc, nl;
  bool finish = false;
  uint32_t verification;
  COR cinza(128,128,128);
  COR vermelho(0,0,255);
  Mat_<COR> imagem(240,240,cinza);
  
  while(!finish){
	  client.receiveUint(nl);
	  client.sendUint(nl);
	  client.receiveUint(verification);
	  finish = testaUint(verification, 123);
  }
  finish = false;
  while(!finish){
	  client.receiveUint(nc);
	  client.sendUint(nc);
	  client.receiveUint(verification);
	  finish = testaUint(verification, 123);
  }
  string nomeVideo = "vazio";
  if (argc == 3) nomeVideo = argv[2];
  VideoWriter video (nomeVideo,CV_FOURCC('X','V','I','D'),30,Size(nc,nl));
  image_cam.create(nl, nc);
  Mat_<COR> imagem_final;
  imagem_final.create(240,240 + nc);
  imagem_final = grudaH(imagem, image_cam);
  resizeWindow("janela",4*imagem_final.cols,4*imagem_final.rows);
  setMouseCallback("janela", on_mouse);
  imagem_final = grudaH(imagem, image_cam);
  imshow("janela",imagem_final);
  
  while ((char)ch != (char)27) {
	imagem.setTo(cinza);
	for (int i = 0; i < 240; i++) {
		imagem(i, 80) = vermelho;
		imagem(i, 160) = vermelho;
		imagem(80, i) = vermelho;
		imagem(160, i) = vermelho;
	}
    if (estado != 0) {
		int l = ((estado - 1)/3)*80;
		int c = ((estado - 1)%3)*80;
		for (int i = 0 ; i < 80; i++) {
			for (int j = 0 ; j < 80; j++) {
				imagem(l + i, c + j)=vermelho;
			}
		}
    }
	client.receiveImgComp(image_cam);
	if (argc == 3) video << image_cam;
    client.sendUint(estados[estado]);
    
    imagem_final = grudaH(imagem, image_cam);
    imshow("janela",imagem_final);
    ch=waitKey(30);    
  }
  destroyWindow("janela");
  client.receiveImgComp(image_cam);
  client.sendUint(1234567890);
}



int seleciona_comando() {
	if ( (bestLoc.x + size/2) < 140 ) { // alvo a esquerda
		return 4;
	}
	if ( 160 + (bestLoc.x + size/2) < 180 ) { // alvo a direita
		return 6;
	}	
}