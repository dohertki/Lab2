
/***************************************************************
 * Kierin Doherty (dohertki)
 *
 * ftserver.c: 
 *
 * input: $ ftserver <Port #> 
 *
 * output: Server transfers files to a client. File must be in
 *         the same directory as the the server.
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
#include <dirent.h>     //opendir() and readdir()
#include <pthread.h>

int setPort(int argc_a, char **argv_a, int *port_a);

void listDirectory(char **lDir_buff);





int main(int argc, char *argv[]){

    char local_server[30];
    int port;
    int flag;
    int socket_fd;
	int newsock_fd;
    
    char *workingBuffer;

    flag = setPort(argc, argv, &port);
    listDirectory(&workingBuffer);
    printf("%d\n",  port);
    printf("dir name: %s\n", workingBuffer );

//*****************************************************************

    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

	bzero((char *) &serv_addr, sizeof(serv_addr)); //free memory  serv_addr
	serv_addr.sin_family =  AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        fprintf(stderr, "Error, no port provided\n");
        exit(1);
    }

    
	if (bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		perror("ERROR on binding");


    listen(socket_fd,5); // listening socket
    clilen = sizeof(cli_addr);
	
	newsock_fd = accept(socket_fd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsock_fd < 0) 
		perror("ERROR on accept");




    return 0;    
}




/******************************************************************
 * Function: 
 *
 * Use:  
 *      
 *
 * Input: 
 *       
 * Output: 
 *
 ******************************************************************/
int setPort(int argc_a, char **argv_a, int *port_a){
    int flag =1;
    printf("args %d\n", argc_a);
    if(argc_a == 2){
        *port_a = atoi(argv_a[1]);
    
    }else{
        printf("Clientchat usage:  ./ftserve [server-port#]\n");
        flag = 0;
    }
    
    return flag;

}

/******************************************************************
 * Function: listDirectory()
 *
 * Use:  
 *      
 *
 * Input: char **lDir_buff: Pointer to a char Pointer holding a buffer
 *            with directory contents.
 *       
 * Output: 
 *
 ******************************************************************/
void listDirectory(char **lDir_buff){
    //check for Null pointer here.
    *lDir_buff = malloc(sizeof(char)*100);
	
    DIR *dirPtr;
	struct dirent *mydirectory;
	dirPtr = opendir(".");
    if (dirPtr == NULL){
        //strerr
        return;
    }
    
    while(1){
        mydirectory = readdir(dirPtr);
        if(mydirectory == NULL)
            break;
        
     //   printf("@[0]: %c\n", mydirectory->d_name[0]);
        if(strncmp(mydirectory->d_name, ".", 1) == 0  ){
      //      printf("continue\n");
      //      printf("passed name: %s\n", mydirectory->d_name);
            continue;
        }


    printf("dir name: %s\n", mydirectory->d_name);
    
    strcat(*lDir_buff, mydirectory->d_name);    
    strcat(*lDir_buff, "\n");

    }
    printf("dir name: %s\n", *lDir_buff );
	return;
}
