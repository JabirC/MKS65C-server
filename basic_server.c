#include "pipe_networking.h"

static void sighandler(int signal){
     if(signal == SIGINT){
        printf("\nThis server will now exit due to SIGINT\n");
        remove("srvr");
        remove("privi");
        exit(1);
     }
  }

int main() {
  signal(SIGINT, sighandler);
  int to_client;
  int from_client;
  from_client = server_handshake( &to_client );

}
