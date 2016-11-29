
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


int initConnection(char* hostname, int portno) {
	int sockfd;
	struct sockaddr_in host;
	struct hostent* host_ip_addr;

	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Creating socket");
	}

	/* Configure */
	host_ip_addr = gethostbyname(hostname);
	memset(&host, '\0', sizeof(host));
	host.sin_family = AF_INET;
	host.sin_port = htons(portno);
	memcpy(&host.sin_addr, host_ip_addr->h_addr, host_ip_addr->h_length);

	/* Initiate connection */
	if (connect(sockfd, (struct sockaddr *) &host, sizeof(host)) < 0) {
		perror("Contacting host");
	}

	return sockfd;
}


// https://www.ibm.com/support/knowledgecenter/SSB23S_1.1.0.12/gtpc2/cpp_gethostbyname.html

// https://www.ibm.com/support/knowledgecenter/SSB23S_1.1.0.12/gtpc2/cpp_gethostbyname.html
void msgScreener(int sock_fd){

    char header[] = "Connected to ftserver\n Don't eat green mushrooms";
//    char filename[128];
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
        case 'g':
            printf("get file %s\n", fldPtr->filename);
            //connect to link Q FIXME
            //send file of link Q FIXME
            
            m_sockfd = clientX();
           // m_sockfd = initConnection(fldPtr->hostname, 30327); //FIXME
           //close Q FIXME
            i = sendfile(m_sockfd, fldPtr->filename);
            readMessage(m_sockfd);
            break;

        case 'l':
            printf("180: In case 'l',calling thread\n");
            m_sockfd = clientX();
            printf("clienX returns ptr %d\n", m_sockfd);
            //connect to link Q FIXME
    // call listDirectory()
            listDirectory(&ar_Buffer);
            sendMessage(m_sockfd, ar_Buffer);
            //send directory on link Q
            readMessage(m_sockfd);
          //  close(m_sockfd);
      //      pthread_exit(NULL);
            break;

        default:
            printf("errror\n");
            
   }
   
            printf("core dumper\n");
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

      
    int sockfd;
    int n;
    int port; //FIXME 
   
    int socket_fd; 
    int newsock_t; //maybe an array?
     
    struct sockaddr_in  cli_addr;
    socklen_t clilen;
	int newsock_fd;

    pthread_t thread[10];
    
    fldPtr = &field;
    
    strcpy(fldPtr->filename, "gulliver.txt");
    
    if(setPort(argc, argv, &port)) //FIXME 
        return 1;

     
    socket_fd = serverX(port);  
      

    listen(socket_fd,5); // listening socket
    clilen = sizeof(cli_addr);
    
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
    close(socket_fd);

  
  
  
    return 0;    
}


/***************************************************************************
 * Function: ()
 *
 * Use:  
 *      
 *
 * Input: 
 *       
 * Output: 
 *
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
        exit(1);    //FIXME
    }

    printf("Socket returns ptr: %d\n", socket_fd);
    return socket_fd; 
}
/***************************************************************************
 * Function: clientX()
 *
 * Use:  
 *      
 *
 * Input: 
 *       
 * Output: 
 *
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
   printf("64\n");

    
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     //
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    printf("318: port of c client %s\n", fldPtr->data_port);
    

    

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
//FIXME set pointer to null if fail.

    return q_sockfd; 

}
/***************************************************************************
 * Function: sendFile()
 *
 * Use:  
 *      
 *
 * Input: 
 *       
 * Output: 
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

    printf("%s\n", read_buffer);
    printf("File size: %ld\n", file_size);
    printf("Bytes written to client %d\n", n);
    
    fclose(in);

    return 0;

}


/***************************************************************************
 * Function: setPort()
 *
 * Use: Program checks command line argument and places it into 'port' 
 *      variable.
 *      
 *
 * Input: int argc_a:
 *        char **argv_a:
 *        int *port_a:
 *       
 * Output: Function returns a value of '0' on success and '1' on failure.
 *
 ***************************************************************************/
int setPort(int argc_a, char **argv_a, int *port_a){
    int flag = 0;
    printf("args %d\n", argc_a);
    if(argc_a == 2){
        *port_a = atoi(argv_a[1]);
    
    }else{
        printf("Missing arguments\n usage:  ./ftserver [server-port#]\n");
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


   // printf("dir name: %s\n", mydirectory->d_name);
    
    strcat(*lDir_buff, mydirectory->d_name);    
    strcat(*lDir_buff, "\n");

    }
   // printf("dir name: %s\n", *lDir_buff );
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

    printf("Send Msg sizeof: %d\n", (int)sizeof(buffer_s)); 
    printf("Send Msg strlen: %d\n", (int)strlen(buffer_s));

    n = send(sock_fd, buffer_s, strlen(buffer_s), 0); 
    if (n < 0){
        perror("ERROR writing to socket");
        
    }
    printf("Bytes written to client %d\n", n);
    
    return;

}

/******************************************************************************
 * Function: readMessage()
 *
 * Use: Function reads incomming messages from socket connection and prints 
 *      them to the display.
 *
 * Input: int sockfd_r: Pointer to a socket.
 *       
 * Output: The function returns void, incoming messages are sent display(stdout)
 *
 *****************************************************************************/
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

















