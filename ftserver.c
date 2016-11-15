

/***************************************************************
 * Kierin Doherty (dohertki)
 *
 * ftserver.c: 
 *
 * input: $ ftserver <Port #> 
 *
 * output: Server unencrypts message passed to it by the 
 *         the key passed via same socket and returns 
 *         a plaintext message
 *
 **************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>



int setAddress(int argc_a, char **argv_a, int *port_a, char *myserver_a);

int main(int argc, char *argv[]){

    char local_server[30];
    int port;
    int flag;

    flag = setAddress(argc, argv, &port, local_server);

    printf("%s, %d", local_server, port);

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        fprintf(stderr, "Error, no port provided\n");
        exit(1);
    }







    return 0;    
}





int setPort(int argc_a, char **argv_a, int *port_a, char *myserver_a){
    int flag =1;
    printf("args %d\n", argc_a);
    if(argc_a == 2){
        *port_a = atoi(argv_a[i]);
    
    }else{
        printf("Clientchat usage:  ./ftserve [server-port#]\n");
        flag = 0;
    }
    
    return flag;

}

