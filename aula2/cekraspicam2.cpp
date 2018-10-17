#include <cekeikon.h>
#include <cekraspicam.h>
int main (int argc, char **argv) {
  CEKRASPICAM cam;
  Mat_<COR> image;
  namedWindow("janela",1);
  int ch=-1;
  TimePoint t1=timePoint();
  int i=0;
  while (ch<0) {
    cam >> image;
    imshow("janela",image);
    ch=waitKey(1);
    i++;
  }
  TimePoint t2=timePoint();
  double t=timeSpan(t1,t2);
  printf("Quadros=%d tempo=%8.2fs fps=%8.2f\n",i,t,i/t);
}

