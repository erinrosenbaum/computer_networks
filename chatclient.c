/*
Program: chatclient.c
Assignment #1 
cs_372 Winter_2017
Erin Rosenbaum
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // Unix Standard
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>  // System Socket Library
#include <netinet/in.h>

// defines the structure hostent
#include <netdb.h> 

//*************************** error function **********************************
// caled when a system call fails. displays information to stderr and aborts. 
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// command line arguments are passed using argc and argv
// arg c is the number of string pointed to by argv, which is 1 + num of args

//*************************** start of main ***********************************
int main(int argc, char *argv[])
{

    // check for 2 inuput parameters
    if (argc != 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    int num=-5; // How many chars we entered
    size_t handle_bufferSize = 1000; // Holds how large the allocated buffer is
    char* handle = NULL; 
    size_t msg_buffer_size = 1000;
    int max_length = 500;
    int c = 0;

    //char buffer[20];

    // get the user's handle, Get input from the user 
    printf("Enter a handle (max_length=10, CTRL-C to exit):");

    // flush std output
    fflush(stdout);
    //Gets user input
    //fgets(h, NAME_SIZE, stdin);

    // writes from stdin to handle, not to exceed buffer size
    num = getline(&handle, &handle_bufferSize, stdin); // Get a line from the user
    
    // keep track of the size of the input
    while (handle[c] != '\0')
        c++;

    // check that handle is not too long 
    if (c > 11){
        fprintf(stderr,"handle must be less than 10 characters in length\n");
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        free(handle);
        exit(0);
    }
    strtok(handle, "\n");
    strcat(handle, "> ");
    //printf("\nHere is the prompt: handle %s", handle);

//********************************** sockets *************************

     // sockfd is a file descriptors, array subscripts stored in a table. 0, 1, or 2. 
     // this is a return value, and portno is the port the server is running on.
     // n is return value for read and write calls
    int sockfd, portno, n;

     // serv_addr contains the address of the server, and is of the type sockaddr_in,
     // which is shown right below. 

/*
// a structure to contain an internet address defined in the include file in.h 
struct sockaddr_in {
        short   sin_family; // should be AF_INET 
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8]; // not used, must be zero 
};
struct in_addr {
   unsigned long s_addr;
};
*/
    struct sockaddr_in serv_addr;

    // server is a pointer to a struct of the type hostent, defined in netdb.h, and is shown:
/*
struct  hostent
{
  char    *h_name;        // official name of host 
  char    **h_aliases;    // alias list 
  int     h_addrtype;     // host address type 
  int     h_length;       // length of address 
  char    **h_addr_list;  // list of addresses from name server 
  #define h_addr  h_addr_list[0]  // address, for backward compatiblity 
};
*/
    struct hostent *server;

     // server characters from the socket into this buffer
    char buffer[1000];
    char message[1000];

    // converts the 2nd argument passed in into a string
    portno = atoi(argv[2]);

    // socket system call creates a new socket. 
    // AF_INET is address domain, used for internet connection. 
    // SOCK_STREAM is type of socket. 

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // return if value less than zero. -1 indicates failure
    if (sockfd < 0) 
        error("ERROR opening socket");

    // gets server name from command line
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // sets all values in buffer to zero. First argument is 
    // pointer to buffer, 2nd is size of buffer. 
    // sets up socket to send and receive data
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // setup of symbolic constants should never change. 
    // sets the fields in serv_adr. Use bcopy
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    // htons coverts into network byte order. converts a port number
    // in host byte order to a port number in network byte order. 
    serv_addr.sin_port = htons(portno);

    // connect function takes 3 arguments: sockfd (file descriptor), address of host,
    // and size of address. If connect returns < 1, give error message. 
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
while(1){
    // prompt the user to enter a message using handle 
    printf("%s",handle);
    // bzero sets all values in the buffer to zero
    bzero(buffer, msg_buffer_size);
    bzero(message, msg_buffer_size);

    // reads into buffer from std input with fgets. 
    fgets(buffer,max_length,stdin);
    // writes the message to the socket. 

    // adds handle to the beginning of message and copy to message
    strtok(buffer, "\n");

    if(strcmp(buffer, "\\quit") == 0){
        close(sockfd);
        free(handle); // Free the memory allocated by getline() or else memory leak
        exit(0);    
    }   

    strcat(message, handle);
    strcat(message, buffer);

    n = write(sockfd,message,strlen(message));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(message,msg_buffer_size);
    bzero(buffer,msg_buffer_size);
    // reads the reply from the socket
    // reads a size from the socket not greater than max into buffer
    n = read(sockfd,buffer,max_length);
    if (n < 0) {
        error("ERROR reading from socket");
        printf("ERROR reading from socket");
        close(sockfd);
        free(handle); // Free the memory allocated by getline() or else memory leak
        break;
    }

    // get rid of new line character 
    strtok(buffer, "\n");

    // prints message to screen. 
    printf("%s\n",buffer);

}
    close(sockfd);
    free(handle); // Free the memory allocated by getline() or else memory leak
    return 0;
}