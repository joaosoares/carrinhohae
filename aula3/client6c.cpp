//client6c.cpp
#include "projeto.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc!=2){
    perror("client6b servidorIpAddr\n");
    exit(1);
  }
  CLIENT client(argv[1]);
  uint32_t u; 
  client.receiveUint(u);
  cout << u << endl;
  client.sendUint(3333333333);
}