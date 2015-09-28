
#include <stdio.h>
#include <sys/socket.h> 
 #include <arpa/inet.h>
 #include <string.h> 
 #include <stdlib.h> 
 #include <fcntl.h> 
 #include <unistd.h>
 #include <signal.h>
 #include <sys/wait.h> 
 
 #define PORT 2000 
 #define LISTEN_ENQ 5  
 #define receivingBuffer 512 
 #define sendingBuffer 512 
  
  void obtainFileByName(int, char*); 
  int fileSent(int , char*); 
  void childSignal(int);  
  char stringReceive[receivingBuffer];  
  ssize_t bytesReceive; 
int counterForSending=0,fileSizeSent = 0; ; 
ssize_t bytesRead, bytesSent;         
char stringSent[sendingBuffer]; 
char * message = "File Not Found\n";   
int file; 
  int main(int argc, char* argv[]) 
  { 
   int serverSocketFileDescriptor, fileDescriptorConnection;  
   struct sockaddr_in serverAddress, clientAddress;  
   socklen_t clientAddressLength;  
   pid_t childProcessID; 
   char fileName [receivingBuffer]; 
   char readableIP [INET_ADDRSTRLEN];  
   memset(&serverAddress, 0, sizeof(serverAddress)); 
   memset(&clientAddress, 0, sizeof(clientAddress)); 
   serverAddress.sin_family = AF_INET;  
   serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
   serverAddress.sin_port = htons(PORT);    
   if ( (serverSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
   {    
   perror("socket error");    
   exit(EXIT_FAILURE);  
   } 
   if( bind(serverSocketFileDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0 )
   {    
   perror("bind error");    
   exit(EXIT_FAILURE);  
   }   
    printf("Listening on port number %d ...\n", ntohs(serverAddress.sin_port)); 
	 if( listen(serverSocketFileDescriptor, LISTEN_ENQ) < 0 )  
	 {   
	  perror("listen error");   
	   exit(EXIT_FAILURE); 
	 } 

	     signal (SIGCHLD, childSignal);    
		  for( ; ; )
		   {   
		    clientAddressLength = sizeof(clientAddress);    
		    printf ("Waiting for a client to connect...\n\n");      
			if ( (fileDescriptorConnection = accept(serverSocketFileDescriptor, (struct sockaddr*) &clientAddress,&clientAddressLength)) < 0 )    
			{      
			perror("accept error");     
			 break; 
		        }        
		  
			 inet_ntop(AF_INET, &(clientAddress.sin_addr), readableIP, INET_ADDRSTRLEN);    
			 printf("Client connected from %s:%d\n",readableIP, ntohs(clientAddress.sin_port) );    
			  
			 if ( (childProcessID = fork()) == 0 )    
			 {      
			 close (serverSocketFileDescriptor);        
		    
			  obtainFileByName(fileDescriptorConnection, fileName);      
			  fileSent(fileDescriptorConnection, fileName);      
			  printf("Closing connection\n");     
			    
			    close(fileDescriptorConnection);    
				 exit(0);     
			  }   
			   close(fileDescriptorConnection);    
			}     
			close(serverSocketFileDescriptor); 
			 return 0; 
} 

void obtainFileByName(int sock, char* fileName) 
{    
       
	  if ( (bytesReceive = recv(sock,  stringReceive, receivingBuffer, 0)) < 0) 
	  {    
	   printf("Nothing receive");   
	   return; 
	  }   
	   sscanf ( stringReceive, "%s\n", fileName);   
} 
int fileSent(int sock, char *fileName) 
{  
  if( (file = open(fileName, O_RDONLY)) < 0)  
   {    
   perror(fileName);    
   if( (bytesSent = send(sock, message ,strlen(message), 0)) < 0 )  
     {     
	 printf("send error");     
	   return -1;    
	 }  
	}  
	else  
	{   
	 printf("Sending file: %s\n", fileName);   
	  while( (bytesRead = read(file, stringSent, receivingBuffer)) > 0 )    
	  {     
	   if( (bytesSent = send(sock, stringSent, bytesRead, 0))< bytesRead )     
	    {        
		printf("Nothing send");        
		return -1;      
		}     
		 counterForSending++;     
		 fileSizeSent += bytesSent;    
	  }   
	   close(file);  
	} 
	printf("Done with this client. Sent %d bytes in %d send(s)\n\n", fileSizeSent, counterForSending); 
	return counterForSending; 
} 


void childSignal(int signo)
 { 
  pid_t    pid; 
  int      stat;   
  while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)   
  printf("child %d terminated\n", pid); 
   return; 
 }
