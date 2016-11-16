

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
#include <dirent.h>     //opendir() and readdir()


int setPort(int argc_a, char **argv_a, int *port_a);

void listDirectory();





int main(int argc, char *argv[]){

    char local_server[30];
    int port;
    int flag;
    int socket_fd;


    flag = setPort(argc, argv, &port);
    listDirectory();
    printf("%d",  port);

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        fprintf(stderr, "Error, no port provided\n");
        exit(1);
    }







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
 * Input: 
 *       
 * Output: 
 *
 ******************************************************************/
void listDirectory(){

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
    }
    
	return;
}
