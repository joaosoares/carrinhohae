#include "projeto.hpp"
#include <iostream>

int main(void) {
  SERVER server;
  server.waitConnection();
  server.sendUint(1234567890);
  uint32_t u; 
  server.receiveUint(u);
  cout << u << endl;
}