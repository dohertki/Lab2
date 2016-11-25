
/**************************************************************************
 * Kierin Doherty (dohertki)
 *
 * ftserver.c: 
 *
 * input: $ ftserver <Port #> 
 *
 * output: Server transfers files to a client. File must be in
 *         the same directory as the the server.
 *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
void testFunction(){
    printf("Test function is working\n");
    return;

}

void sendMessage(int sock_fd, char* buffer_s);

void msgScreener(int sock_fd){

    char header[128];
    char client_flag[32];
    int data_port;
    char filename[128];
    int words;
    bzero(header,128);
    //need to set up as char switch will no accept a char*
    // this should be in a new thread or process
    char flag = 'l';
    int n;
    char *ar_Buffer;
    //Read the header from the client 		   
    n = read(sock_fd,header,128); //read the header 
    if (n < 0){
        perror("ERROR reading from socket");
        close(sock_fd);
        exit(1);               
    }
    printf("Header %s", header);
    sscanf(header,"%s  %d  %d %s",client_flag, &data_port, &words, filename); //extract header info

    printf("Client flag: %s", client_flag); 
   
    switch(flag){
        case 'g':
            printf("get file\n");
            //connect to link Q FIXME
            //send file of link Q FIXME
            //close Q FIXME
            break;

        case 'l':
            printf("list file\n");
            //connect to link Q FIXME
            // call listDirectory()
            listDirectory(&ar_Buffer);
            // call sendMessage{
            sendMessage(sock_fd, ar_Buffer);
            //send directory on link Q
            //close Q
            break;

        default:
            printf("errror\n");
            
   }
    return;
}



void *arbiter(void *s_th){
    
    printf("Entered thread 2 line 79\n");
    int socket_th = (intptr_t)s_th;
    printf("Thread ptr: %d \n", socket_th);
    /*http://stackoverflow.com/questions/8487380
     /how-to-cast-an-integer-to-void-pointer                */
   msgScreener((intptr_t)s_th); 
   
    
    pthread_exit(NULL);

}












int main(int argc, char *argv[]){

    char local_server[30];
    int port;
    int flag;
    int n;
    int socket_fd;
	int newsock_fd;
    int newsock_t; //maybe an array?
    
    flag = setPort(argc, argv, &port);

//*****************************************************************
//
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


    printf("Socket returns ptr: %d\n", socket_fd);
    listen(socket_fd,5); // listening socket
    clilen = sizeof(cli_addr);
/***************************************************************************
****************************************************************************
********************** This is where the magic happens *********************
****************************************************************************
***************************************************************************/
    pthread_t thread[10];
    
    while(1){
        newsock_fd = accept(socket_fd, (struct sockaddr *) &cli_addr, &clilen);
	    if (newsock_fd < 0) 
            perror("ERROR on accept");
     //   https://computing.llnl.gov/tutorials/pthreads/    
        printf("Accept returns ptr: %d\n", newsock_fd);
        n = pthread_create(&thread[0], NULL, arbiter, (void*)(intptr_t)newsock_fd);
        printf("Thread returns: %d, socket ptr: %d\n", n,  newsock_fd);



    }
    printf("End\n");


    return 0;    
}




/***************************************************************************
 * Function: 
 *
 * Use:  
 *      
 *
 * Input: 
 *       
 * Output: 
 *
 ***************************************************************************/
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

/***************************************************************************
 * Function: listDirectory()
 *
 * Use: Funciton places a list of current working directory contents
 *      into a buffer.
 *      
 *
 * Input: char **lDir_buff: Pointer to a char Pointer holding a buffer
 *            with list directory contents.
 *       
 * Output: 
 *
 ***************************************************************************/
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


/******************************************************************
 * Function: sendMessage()
 *
 * Use:  
 *      
 *
 * Input: int sock_fd:
 *        char* buffer_s:
 *       
 * Output: 
 *
 ******************************************************************/

void sendMessage(int sock_fd, char* buffer_s){
    int n;

    printf("sizeof: %d\n", (int)sizeof(buffer_s)); 
    printf("strlen: %d\n", (int)strlen(buffer_s));

    n = send(sock_fd, buffer_s, strlen(buffer_s), 0); 
//    n = write(newsock_fd, workingBuffer, sizeof(workingBuffer)); //Write back the type of server
            if (n < 0){
                perror("ERROR writing to socket");
            }
    return;

}
