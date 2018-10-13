#include <netinet/in.h>
#include <unistd.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <arpa/inet.h> //inet_ntoa

int create_server_socket(int port);


int main(int argc, char *argv[]){
  int new_socket_descriptor, socket_descriptor; 
  int n;
  unsigned int clilen;  
  struct sockaddr_in cli_addr;
  
  if(argc!=2) {
    printf("Usage: %s port_number\n", argv[0]);
    exit(1); 
  }

  
  //SIGPIPE is sent to the process if a write is made to a closed connection.
  //By default, SIGPIPE terminates the process. This makes the process to ignore the signal.
  signal(SIGPIPE, SIG_IGN);

       
  socket_descriptor = create_server_socket(atoi(argv[1]));


  while(1) {
    char c;
    
    printf("Waiting connection\n");  
    clilen = sizeof(cli_addr); 
    new_socket_descriptor = accept(socket_descriptor, (struct sockaddr *) &cli_addr, &clilen);  
    printf("Connection from %s\n", inet_ntoa(*((struct in_addr *) &(cli_addr.sin_addr) )));
  

    //serve request (communication with the client)
    //On each iterartion, read one character, convert it to uppercase and send it back to the cliente 
    while(1)
    {
      n = read(new_socket_descriptor, &c, 1);
      if(c=='\n' || n==0)//read until it detects a newline ('\n') or end of data stream
        break;
        
      c = toupper(c);
      
      usleep(100000);
      write(new_socket_descriptor, &c, 1);
    }


    close(new_socket_descriptor);
  }
  
  return 0;
}

int create_server_socket(int port)
{
  int s;
  struct sockaddr_in serv_addr;

  s = socket(PF_INET, SOCK_STREAM, 0);
  if(s < 0){
    perror("socket");
    exit(1);
  }
        
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  
  if(bind(s, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
    perror("bind");
    exit(1);
  }    
  
  listen(s, 5); 

  return s;
}

