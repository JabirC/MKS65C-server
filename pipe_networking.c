#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/

char rot13c( char ch){
  char alpha[] = "abcdefghijklmnopqrstuvwxyz";
  char Alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  if(strchr(alpha, ch) != NULL){
    int index = (strchr(alpha,ch) - alpha + 13) % 26;
    return *(alpha + index);
  }
  else if (strchr(Alpha, ch) != NULL ){
    int index = (strchr(Alpha,ch) - Alpha + 13) % 26;
    return *(Alpha + index);
  }
  else return ' ';
}
char * rot13(char * msg){
     char * result = malloc(BUFFER_SIZE * sizeof(char));
     int i = 0;
     while(*(msg + i)){
       *(result + i) = rot13c(*(msg + i));
       i++;
     }
     return result;
}

int server_handshake(int *to_client) {

        int up;
        mkfifo("srvr", 0644);

        printf("\n$$SERVER$$ Server has created well known pipe, Waiting for client\n");

        up =open("srvr",O_RDONLY);
        if(up == -1){
          printf("%s \n",strerror(errno));
        }
        else{
          char buffer[HANDSHAKE_BUFFER_SIZE];
          read(up, buffer, HANDSHAKE_BUFFER_SIZE);
          printf("$$SERVER$$ Server received private pipe name %s, sending acknowledgement\n", buffer);
          close(up);
          remove("srvr");

          *to_client = open(buffer, O_WRONLY);
          if(*to_client == -1){
            printf("%s \n",strerror(errno));
          }
          else{
            write(*to_client,ACK , HANDSHAKE_BUFFER_SIZE);
            close(*to_client);

            char client_resp[HANDSHAKE_BUFFER_SIZE];
            *to_client = open(buffer, O_RDONLY);
            read(*to_client, client_resp, HANDSHAKE_BUFFER_SIZE);
            printf("$$SERVER$$ Received response from client: %s\n", client_resp);
            close(*to_client);
            printf("$$SERVER$$ Handshake Complete\n\n");

            int down;
            char server_buf[BUFFER_SIZE];
            char client_prompt[BUFFER_SIZE];
            while(1){
                down = open(buffer, O_RDONLY);
                read(down, client_prompt, BUFFER_SIZE);
                close(down);
                printf("Processing user given data: %s\n", client_prompt);
                up = open(buffer, O_WRONLY);
                strcpy(server_buf, rot13(client_prompt));
                write(up, server_buf, BUFFER_SIZE);
                close(up);
            }
            return up;
          }
        }
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  mkfifo("privi", 0644);

  *to_server = open("srvr", O_WRONLY);
  if(*to_server== -1){
    printf("%s, Well-known pipe not found \n",strerror(errno));
  }
  else{
    char pvt_name[HANDSHAKE_BUFFER_SIZE];
    strcpy(pvt_name, "privi");
    write(*to_server, pvt_name, HANDSHAKE_BUFFER_SIZE);
    printf("$$CLIENT$$ Client has sent private pipe name: %s\n", pvt_name);
    char buffer_client[BUFFER_SIZE];

    int down = open(pvt_name, O_RDONLY);
    if (down == -1){
      printf("%s\n",strerror(errno));
    }
    else{
      read(down, buffer_client, HANDSHAKE_BUFFER_SIZE);
      printf("$$CLIENT$$ Client received acknowledgement from server: '%s', sending response\n", buffer_client);
      close(down);


      down = open(pvt_name, O_WRONLY);
      char last[HANDSHAKE_BUFFER_SIZE] = "Adios!";
      write(down, last, HANDSHAKE_BUFFER_SIZE);
      close(down);
      printf("$$CLIENT$$ Handshake Complete\n\n");

      int up;
      char client_buf[BUFFER_SIZE];
      char server_resp[BUFFER_SIZE];
      while(1){
        printf("Say something to the server: ");
        fgets(client_buf, BUFFER_SIZE, stdin);
        client_buf[strlen(client_buf) - 1] = 0;
        up= open(pvt_name, O_WRONLY);
        write(up, client_buf, BUFFER_SIZE);
        close(up);
        down = open(pvt_name , O_RDONLY);
        read(down, server_resp, BUFFER_SIZE);
        close(down);
        printf("The server says: %s\n",server_resp );
      }
      return down;
    }
  }
}
