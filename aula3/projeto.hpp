#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
// Cpp includes
#include <string>
#include <vector>
#include <iostream>

using namespace std;

#define BYTE unsigned char
#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 1   // how many pending connections queue will hold
#define MAXDATASIZE 256 // max number of bytes we can get at once 

class SERVER {
private:
  int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes;
  char s[INET6_ADDRSTRLEN];
  int rv;
public:
  SERVER();
  ~SERVER();
  void waitConnection();
  void sendBytes(int nBytesToSend, BYTE *buf);
  void receiveBytes(int nBytesToReceive, BYTE *buf);
  void sendUint(uint32_t m);
  void receiveUint(uint32_t& m);
  void sendVb(const vector<BYTE>& vb);
  void receiveVb(vector<BYTE>& st);
};

class CLIENT {
private:
  int sockfd, numbytes;  
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
public:
  CLIENT(string endereco);
  ~CLIENT();
  void sendBytes(int nBytesToSend, BYTE *buf);
  void receiveBytes(int nBytesToReceive, BYTE *buf);
  void sendUint(uint32_t m);
  void receiveUint(uint32_t& m);
  void sendVb(const vector<BYTE>& vb);
  void receiveVb(vector<BYTE>& st);
};

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool testaBytes(BYTE* buf, BYTE b, int n) {
  //Testa se n bytes da memoria buf possuem valor b
  bool igual=true;
  for (unsigned i=0; i<n; i++)
    if (buf[i]!=b) {
      igual=false; break;
    }
  return igual;
}

bool testaVb(const vector<BYTE> vb, BYTE b) {
  //Testa se todos os bytes de vb possuem valor b
  bool igual=true;
  for (unsigned i=0; i<vb.size(); i++) {
    printf("%i", vb[i]);
    if (vb[i]!=b) { igual=false; break; }
  }
  printf("\n");
  return igual;
}

SERVER::SERVER () {
  yes = 1;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    perror("erro obtendo endereco");
    exit(1);
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
        p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
        sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }
    break;
  }
  freeaddrinfo(servinfo); // all done with this structure
  if (p == NULL)  {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }  
}

void SERVER::waitConnection() {
  printf("server: waiting for connections...\n");
  while (1) {
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    } else break;
  }
  inet_ntop(their_addr.ss_family,
    get_in_addr((struct sockaddr *)&their_addr),
    s, sizeof s);
  printf("server: got connection from %s\n", s);
  close(sockfd); // doesn't need the listener anymore
}

void SERVER::sendBytes(int nBytesToSend, BYTE *buf) {
  int err = send(new_fd, buf, nBytesToSend, 0);
  if (err == -1) {
    perror("send error");
  }
}

void SERVER::receiveBytes(int nBytesToReceive, BYTE *buf) {
  int err = recv(new_fd, buf, nBytesToReceive, 0);
  if (err == -1) {
    perror("recv error");
  }
}

void SERVER::sendUint(uint32_t m) {
  m = ntohl(m);
  int err = send(new_fd, &m, sizeof(m), 0);
  if (err == -1) {
    perror("send error");
  }
}

void SERVER::receiveUint(uint32_t& m) {
  int err = recv(new_fd, &m, sizeof(m), 0);
  if (err == -1) {
    perror("recv error");
  }
  m = htonl(m);
}

void SERVER::sendVb(const vector<BYTE>& vb) {
  int err = send(new_fd, vb.data(), vb.size(), 0);
  if (err == -1) {
    perror("send error");
  }
}

void SERVER::receiveVb(vector<BYTE>& st) {
  int err = send(new_fd, st.data(), st.size(), 0);
  if (err == -1) {
    perror("send error");
  }
}

SERVER::~SERVER() {
  close(new_fd);
}

CLIENT::CLIENT(string hostname) {
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if ((rv = getaddrinfo(hostname.c_str(), PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    perror("getaddrinfo error");
    exit(1);
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
        p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("client: connect");
      close(sockfd);
      continue;
    }
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    exit(2);
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
      s, sizeof s);
  printf("client: connecting to %s\n", s);
  freeaddrinfo(servinfo); // all done with this structure

}

void CLIENT::sendBytes(int nBytesToSend, BYTE *buf) {
  int err = send(sockfd, buf, nBytesToSend, 0);
  if (err == -1) {
    perror("send error");
  }
}

void CLIENT::receiveBytes(int nBytesToReceive, BYTE *buf) {
  int err = recv(sockfd, buf, nBytesToReceive, 0);
  if (err == -1) {
    perror("recv error");
  }
}


void CLIENT::sendUint(uint32_t m) {
  m = ntohl(m);
  int err = send(sockfd, &m, sizeof(m), 0);
  if (err == -1) {
    perror("send error");
  }
}

void CLIENT::receiveUint(uint32_t& m) {
  int err = recv(sockfd, &m, sizeof(m), 0);
  if (err == -1) {
    perror("recv error");
  }
  m = htonl(m);
}


void CLIENT::sendVb(const vector<BYTE>& vb) {
  // Send the size of the array
  sendUint(vb.size());
  sendBytes(vb.size(), vb.data());
}

void CLIENT::receiveVb(vector<BYTE>& st) {
  BYTE temp_arr[MAXDATASIZE];
  while (rec )
  receiveBytes(MAXDATASIZE, temp_arr);
  printf("%s", temp_arr);
}

CLIENT::~CLIENT() {
  close(sockfd);
}
