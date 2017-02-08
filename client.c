#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // Unix Standard
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>  // System Socket Library
#include <netinet/in.h>

// defines the structure hostent
#include <netdb.h> 

#define BUF_SIZE 500 // max number of characters
#define NAME_SIZE 10 // max name size


// caled when a system call fails. displays information to stderr and aborts. 
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// command line arguments are passed using argc and argv
// arg c is the number of string pointed to by argv, which is 1 + num of args
int main(int argc, char *argv[])
{

    // check for 2 inuput parameters
    if (argc != 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    int num=-5; // How many chars we entered
    size_t bufferSize = 20; // Holds how large the allocated buffer is
    char* handle = NULL; 

    //char buffer[20];

    // get the user's handle, Get input from the user 
    printf("Enter a handle (max_length=10, CTRL-C to exit):");

    // flush std output
    //fflush(stdout);
    //Gets user input
    //fgets(h, NAME_SIZE, stdin);

    num = getline(&handle, &bufferSize, stdin); // Get a line from the user
    printf("size of num: %d \n", sizeof(num));

   
    int c = 0;
    while (handle[c] != '\0')
        c++;
    printf("c: %d \n", c);


    // check that handle is not too long 

    if (c > 11){
        fprintf(stderr,"handle must be less than 10 characters in length\n");
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        free(handle);
        exit(0);
    }
   
    printf("Here is the raw entered line: %s", handle);
    //snprintf(handle, bufferSize, "%s> ", handle);
    //handle[c]='>';
    int d = 1;
    //handle[d]=">";
    //handle[d]="\0";
    printf("\nHere is the prompt: handle %s", handle);
    strtok(handle, "\n");
    strcat(handle, "> Hi!!");
    printf("\nHere is the prompt: handle %s", handle);




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
    char buffer[256];



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

    // prompt the user to enter a message
    printf("Please enter the message: ");
    bzero(buffer,256);
    // reads from std input with fgets
    fgets(buffer,255,stdin);
    // writes the message to the socket. 
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);

    // reads the reply from the socket
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");

    // prints message to scree. 
    printf("%s\n",buffer);
    close(sockfd);

    free(handle); // Free the memory allocated by getline() or else memory leak
    return 0;
}