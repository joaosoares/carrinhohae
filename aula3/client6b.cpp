//client6b.cpp
//testa sendBytes e receiveBytes
#include "projeto.hpp"

int main(int argc, char *argv[]) {
  if (argc!=2){
    perror("client6 servidorIpAddr\n");
    exit(1);
  }
  CLIENT client(argv[1]);
  
  const int n=100;
  BYTE buf[n];
  
  client.receiveBytes(n,(BYTE*)buf);
  if (testaBytes(buf,111,n)) printf("Recebeu corretamente %d bytes %d\n",n,111);
  else printf("Erro na recepcao de %d bytes %d\n",n,111);
 
  memset(buf,214,n);
  client.sendBytes(n,(BYTE*)buf);
  
  // Teste de recepcao errada (enviado 111, esperado 2)
  client.receiveBytes(n,(BYTE*)buf);
  if (testaBytes(buf,2,n)) printf("Recebeu corretamente %d bytes %d\n",n,2);
  else printf("Erro na recepcao de %d bytes %d\n",n,2);
}
