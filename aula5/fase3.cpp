#include <opencv2/opencv.hpp>
#include <thread>

using namespace std;
using namespace cv;

#define THRESHOLD 0.75

int main(int argc, char *argv[]) {
  if (argc < 4) {
    perror("fase3 entrada.avi match.png saida.avi\n");
    exit(-1);
  }
  // Pegar nomes
  string nomeEntrada = argv[1];
  string nomeMatch = argv[2];
  string nomeSaida = argv[3];

  // Abrir arquivos de video e arquivo de imagem
  VideoCapture entrada(nomeEntrada);
  auto fps = entrada.get(CV_CAP_PROP_FPS);
  Mat match = imread(nomeMatch, CV_64FC1);
  printf("Abrindo arquivos\n");
  VideoWriter saida(nomeSaida, CV_FOURCC('X', 'V', 'I', 'D'), fps,
                    Size(240, 320));

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
      // auto result = results[i];
      matchTemplate(frameEntrada, resizedMatch, results[i], CV_TM_CCORR_NORMED);
      // printf("Results size %d rows, %d cols\n", results[i].rows, result[i].cols);
      printf("Results frameEntrada %d rows, %d cols\n", frameEntrada.rows, frameEntrada.cols);
      // results[i] = result;
      // imshow("results", result);
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

void cv_match_template(Mat& frameEntrada, Mat& resizedMatch, Mat& result) {
  matchTemplate(frameEntrada, resizedMatch, result, CV_TM_CCORR_NORMED);
}