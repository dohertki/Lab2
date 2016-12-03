
/**************************************************************************
 * Kierin Doherty (dohertki)
 * CS372 Project 2
 * 27 NOV 2016
 *
 * ftserver.c: Program is a file transfer server that works in conjunction
 *             with ftclient.py
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
#include <arpa/inet.h>
#define BUFFSIZE 1024

int setPort(int argc_a, char **argv_a, int *port_a);
void readMessage(int sockfd_r);
void listDirectory(char **lDir_buff);
int clientX();
void sendMessage(int sock_fd, char* buffer_s);
int sendfile(int sockfd_s, char* filename_s);

int serverX(int port);

struct clientParam{
    int socket_fd;    
    char header[128];
    char client_flag[32];
    char data_port[256];
    char filename[128];
    char hostname[256];
};

struct clientParam field, *fldPtr;



void msgScreener(int sock_fd){

    char header[] = "Connected to ftserver\n Don't eat green mushrooms";
    int i;
    char flag;
    fldPtr = &field;
    int m_sockfd;
    int n;
    char *ar_Buffer;
    
    
    bzero(fldPtr->header,128);
    
    
    i = read(sock_fd,fldPtr->header,128); //read the header 
    if (i < 0){
        perror("ERROR reading from socket");
        close(sock_fd);
        exit(1);               
    }
    
    sendMessage(sock_fd, header);

    printf("Header %s\n", fldPtr-> header);
    sscanf(fldPtr->header,"%s %s  %s %s",fldPtr->client_flag, fldPtr->data_port,\
            fldPtr->hostname, fldPtr->filename); //extract header info
        
    printf("Msg  hostname: %s", fldPtr->hostname);
    
    
    
    if( strcmp( fldPtr->client_flag, "-l") == 0)
        flag = 'l';
    else if(strcmp(fldPtr->client_flag, "-g") == 0)
        flag = 'g';
    else{
        printf("Header missing -1 of -g\n");
    }
        
    printf("Client flag: %s\n", fldPtr->client_flag); 
   
    switch(flag){       
        case 'g':          //Get file download
            printf("get file %s\n", fldPtr->filename);
            
            m_sockfd = clientX();
            i = sendfile(m_sockfd, fldPtr->filename);
            break;

        case 'l':      //Get directory information
            printf("180: In case 'l',calling thread\n");
            m_sockfd = clientX();
            printf("clienX returns ptr %d\n", m_sockfd);
            listDirectory(&ar_Buffer);
            sendMessage(m_sockfd, ar_Buffer);
            break;

        default:
            printf("errror\n");
            
   }
   close(m_sockfd);
      return;
}

void *arbiter(void *s_th){
    
    printf("Entered thread 2 line 79\n");
    int socket_th = (intptr_t)s_th;
    printf("Thread ptr: %d \n", socket_th);
    //*NOTE 
    /*http://stackoverflow.com/questions/8487380
     /how-to-cast-an-integer-to-void-pointer      */
       
    
    msgScreener((intptr_t)s_th); 
  
 //   pthread_exit((void*) s_th); 

}


int main(int argc, char *argv[]){

      
    int sockfd;
    int n;
    int port;    
    int socket_fd; 
    int newsock_t; //maybe use an array?
    void *status; 
    struct sockaddr_in  cli_addr;
    socklen_t clilen;
	int newsock_fd;
//   *NOTE
//   Threading code is based on a tutorial @ https://computing.llnl.gov
//   /tutorials/pthreads/    
 
    pthread_t thread[10];
    pthread_attr_t attr;    
    fldPtr = &field;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
       
    if(setPort(argc, argv, &port))  
        return 1;

     
    socket_fd = serverX(port);  
      

    listen(socket_fd,5); 
    clilen = sizeof(cli_addr);
    int t = 0;
    while(1){
        
        newsock_fd = accept(socket_fd, (struct sockaddr *) &cli_addr, &clilen);
	    if (newsock_fd < 0) 
            perror("ERROR on accept");
        printf("New Connection\n"); 
        n = pthread_create(&thread[t], &attr, arbiter, (void*)(intptr_t)newsock_fd);
   
   
     //   pthread_attr_destroy(&attr);
     //   n = pthread_join(thread[t],&status);
   
    }
  
    printf("End\n");
    close(socket_fd);

  
  
  
    return 0;    
}


/***************************************************************************
 * Function: serverX()
 *
 * Use:  Function sets up a socket to be used by the server. Function take
 *       an integer as an argument for the server port number.
 *      
 *
 * Input: int port: The server port number. 
 *       
 * Output: Function returns a pointer to the socket that it created.
 *
 * *NOTE(1): code is referenced from client code from http://beej.us/guide/
 *  bgnet/output/html/singlepage/bgnet.html#connect
 * *NOTE(2) code refenced from  https://www.ibm.com/support/knowledgecenter
 *  /SSB23S_1.1.0.12/gtpc2/cpp_gethostbyname.html
 ***************************************************************************/

int serverX(int port){
    char local_server[30];
    int n;
    int socket_fd;
    
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

    
	if (bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("ERROR on binding\n");
        exit(1);    
    }

    printf("Socket returns ptr: %d\n", socket_fd);
    return socket_fd; 
}
/***************************************************************************
 * Function: clientX()
 *
 * Use:  Function acts a  a client. Function sets up socket and connects to
 *       host and port number found in clientParam struct members 'hostname'
 *       and 'data_port'.
 *
 * Input: Function use global variable i clientParam struct 'user_entry'.
 *       
 * Output: Function returns a pointer to a connectec socket.
 *
 * *NOTE(1): code is referenced from client code from http://beej.us/guide/
 *  bgnet/output/html/singlepage/bgnet.html#connect
***************************************************************************/
int clientX(){
    
    struct sockaddr_in q_addr;
    struct hostent *host_e;
    int n, q_sockfd;
    char ar_Buffer[] = "GET / HTTP/1.1\r\n\r\n";
    
    printf("hostname: %s\n", fldPtr->hostname);

       
    printf("port: %s\n", fldPtr->data_port);
  
     int status;
    struct addrinfo hints, *res, *p;
    struct addrinfo *servinfo;  // will point to the results
    char ipstr[INET6_ADDRSTRLEN]; 
    /*see NOTE*/
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     //
    hints.ai_socktype = SOCK_STREAM; 
    if ((status = getaddrinfo(fldPtr->hostname, fldPtr->data_port , &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return 2;
}
            

    if(host_e == NULL)
        printf("Unable to resolve host name %s\n", fldPtr->hostname);
	       
           
    bzero((char *) &q_addr, sizeof(q_addr)); //free memory  serv_addr

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;


    
    q_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(q_sockfd == -1){
        perror("Data socket error\n");
    }
    if( connect(q_sockfd, res->ai_addr,res->ai_addrlen) == 0)
        perror("Data socket connect error\n");


    freeaddrinfo(res); 


    return q_sockfd; 

}
/***************************************************************************
 * Function: sendFile()
 *
 * Use: Function accepts a socket pointer and a filename. If file is in 
 *      present working directory it will be sent on the connection at the
 *      socket. 
 *
 * Input: int sockfd_s: Pointer to a socket connection
 *        char* filename_s: A file name to a file in current directory
 *       
 * Output: Function returns a value of 1 if file cant be opened or 0 otherwise
 *
 ***************************************************************************/
int sendfile(int sockfd_s, char* filename_s){

    char read_buffer[BUFFSIZE];
    FILE *in;
    int size;
    int n;
    long file_size = 0;
    long msg_size = 0 ;
    
    printf("Attempting to open file: %s\n", filename_s); 
    
    in = fopen(filename_s, "rb");
    if( in == NULL){
        printf("Error opening file\n");
        return 1;
    }
    
    printf("file opened: %s\n", filename_s);
    
    while(1){
        
        size = fread(read_buffer, 1 ,sizeof(read_buffer) ,in);
        file_size += size;
        if(size == 0)
            break;
            
        n = send(sockfd_s, read_buffer, sizeof(read_buffer), 0); 
        msg_size = msg_size + n;
        if (n < 0){
            perror("ERROR writing to socket");
            break;
        }
    }

    printf("File sent: %s\n", fldPtr->filename);
    printf("To Host: %s @ port: %s\n", fldPtr->hostname, fldPtr->data_port); 
    printf("File size: %ld\n", file_size);
    printf("Bytes written to client %d\n", n); 

    return 0;

}

/***************************************************************************
 * Function: setPort()
 *
 * Use: Program checks command line argument and places it into 'port' 
 *      variable.
 *      
 *
 * Input: int argc_a: number of arguments on commmand line
 *        char **argv_a: an array of strings holding command line arguments
 *        int *port_a: a pointer to location of the value of port.
 *       
 * Output: Function returns a value of '0' on success and '1' on failure.
 *
 ***************************************************************************/
int setPort(int argc_a, char **argv_a, int *port_a){
    int flag = 0;
    
    if(argc_a == 2 && atoi(argv_a[1]) > 1024 && atoi(argv_a[1]) < 65536){
        *port_a = atoi(argv_a[1]);
    
    }else{
        printf("Port number needed (1025-65500)\nFormat useage:  ./ftserver [server-port#]\n");
        flag = 1;

    }
    
    return flag;

}

/***************************************************************************
 * Function: listDirectory()
 *
 * Use: Funciton places a list of current working directory contents
 *      into a buffer.
int sendfile(int sockfd_s, char* filename_s){
 *      
 *
 * Input: char **lDir_buff: Pointer to a char Pointer holding a buffer
 *            with list directory contents.
 *       
 * Output: Function returns void
 *
 ***************************************************************************/
void listDirectory(char **lDir_buff){
    //check for Null pointer here.
    *lDir_buff = malloc(sizeof(char)*100);
	
    DIR *dirPtr;
	struct dirent *mydirectory;
	dirPtr = opendir(".");
    if (dirPtr == NULL){
        return;
    }
    
    while(1){
        mydirectory = readdir(dirPtr);
        if(mydirectory == NULL)
            break;
        
        if(strncmp(mydirectory->d_name, ".", 1) == 0  ){
            continue;
        }


    
    strcat(*lDir_buff, mydirectory->d_name);    
    strcat(*lDir_buff, "\n");

    }  
	return;
}


/**************************************************************************
 * Function: sendMessage()
 *
 * Use: Function sends a stored character string to to a socket 
 *      pointed to by a pointer passed into one of the parameters
 *      
 *
 * Input: int sock_fd:  Pointer to a connected socket
 *        char* buffer_s: Pointer to a string that will be sent
 *        on connection
 *       
 * Output: Function is void, but displays result to screen.
 *
 *************************************************************************/

void sendMessage(int sock_fd, char* buffer_s){
    int n;

    printf("Send Msg sizeof: %d\n", (int)sizeof(buffer_s)); 
    printf("Send Msg strlen: %d\n", (int)strlen(buffer_s));

    n = send(sock_fd, buffer_s, strlen(buffer_s), 0); 
    if (n < 0){
        perror("ERROR writing to socket");
        
    }
    printf("Bytes written to client %d\n", n);
    
    return;

}

/***************************************************************************
 * Function: readMessage()
 *
 * Use: Function reads incomming messages from socket connection and prints 
 *      them to the display.
 *
 * Input: int sockfd_r: Pointer to a socket.
 *       
 * Output: The function returns void, incoming messages are sent to display
 *
 **************************************************************************/
void readMessage(int sockfd_r){
    int n;
    char small_buffer[1024];

    bzero(small_buffer,1024);  //clear buffer.
    n = read(sockfd_r,small_buffer,1024); //read message back from server.
    if (n < 0) 
        perror("ERROR reading from socket");//        error("Error connecting");
  
    printf("%s\n", small_buffer); 
    
    return;

}



