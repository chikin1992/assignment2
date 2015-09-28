
#include <stdio.h>     
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <string.h>     
#include <stdlib.h>     
#include <fcntl.h>      
#include <unistd.h>    

#define sendingBuffer 512 
#define receivingBuffer 512 
#define PORT 2000 

char sentStringSize[sendingBuffer];  
int file; 
int counterForReceiving,fileSizeForReceive; 
ssize_t bytesSending, bytesReceive; 
char receivingString[receivingBuffer];   
size_t stringSendingSize;  
int fileReceive(int ,char*); 
int main(int argc, char* argv[]) 
{ 
   int clientSocketFileDescriptor;  
   struct sockaddr_in  serverAddress;  
   char sentStringSize[sendingBuffer];       
     memset(&serverAddress, 0, sizeof(serverAddress));

	 clientSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    
	 serverAddress.sin_family = AF_INET;    

	 if ( inet_pton(AF_INET, argv[2], &(serverAddress.sin_addr)) < 1 ) 
	  {    printf("IP address not found\n"); 
	       exit(1);  
	  }   
  
	serverAddress.sin_port =  htons(PORT);     
	 if( connect(clientSocketFileDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0 )  
	 {    
	   printf("Error in connection");    
	   exit(1); 
	 }   
	   printf("start the connection via %s through %d port\n",argv[2],PORT);   
	   fileReceive(clientSocketFileDescriptor, argv[1]);
  
		 if(close(clientSocketFileDescriptor) < 0)  
		  {
		  	  printf("socket cant close");   
			 exit(1);  
		  }    
		  return 0; 
} 
int fileReceive(int sock, char* fileName) 
{   
  sprintf(sentStringSize,"%s\n",fileName);  
  stringSendingSize = strlen(sentStringSize); 
 
   

  if( (bytesSending = send(sock, fileName, stringSendingSize, 0)) < 0 ) 
  {   
    printf("Error in sending");    
    return -1;  
  }  
 
    if ( (file = open(fileName, O_WRONLY|O_CREAT, 700)) < 0 )  
   {    
    printf("Error in open file");    
    return -1;  
   } 

    counterForReceiving = 0; 
	fileSizeForReceive = 0;    
         
	while ( (bytesReceive = recv(sock, receivingString, receivingBuffer, 0)) > 0 )  
	{    
	   counterForReceiving++;    
	   fileSizeForReceive += bytesReceive;       
	    if (write(file, receivingString, bytesReceive) < 0 )   
		 {     
		  printf("Error write the file");     
		  return -1;    
		 }  
   }  
   close(file); 
   printf("Client : i received the file which in: %d bytes for %d time\n", fileSizeForReceive, counterForReceiving); 
   return fileSizeForReceive; 
}
