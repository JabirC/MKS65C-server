#include "pipe_networking.h"

static void sighandler(int signal){
     if(signal == SIGINT){
        printf("\nThis client will now exit due to SIGINT\n");
        remove("srvr");
        remove("privi");
        exit(1);
     }
  }

int main() {
  signal(SIGINT, sighandler);
  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );
}
