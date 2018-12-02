#include "cekeikon.h"

using namespace std;
using namespace cv;

#define THRESHOLD 0.85

int main(int argc, char *argv[]) {

  // Pegar nomes
  string nomeEntrada = "capturado.avi";
  string nomeMatch = "quadrado.png";
  string nomeSaida = "locarec.avi";

  // Criar MNIST
  MnistFlann mnist(14, true, true);
  mnist.le(".");
  mnist.train();

  // Abrir arquivos de video e arquivo de imagem
  VideoCapture entrada(nomeEntrada);
  auto fps = entrada.get(CV_CAP_PROP_FPS);
  Mat match = imread(nomeMatch, CV_64FC1);
  printf("Abrindo arquivos\n");
  // VideoWriter saida(nomeSaida, CV_FOURCC('X', '2', '6', '4'), fps,
  //                   Size(240, 320));

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


  while (entrada.isOpened()) {
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
      Rect rRect(bestLoc, Point(bestLoc.x + size, bestLoc.y + size));
      Mat numberTemp = frameSaida(rRect);
      Mat number;
      numberTemp.copyTo(number);
      mnist.bbox(number);
      rectangle(frameSaida, bestLoc, Point(bestLoc.x + size, bestLoc.y + size), Scalar(0, 0, 255), 3);
    }
    imshow("display", frameSaida);
    if (waitKey(30) > 0) {
      break;
    }
  }
}
