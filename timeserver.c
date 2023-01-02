/*
	C socket server example
*/

#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <sys/time.h>
#include <time.h>

#define PORT_NUMBER 60140

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , read_size; //Socket desc is the server side of the socket, whether the client_sock is for the server side 
	struct sockaddr_in server , client; // Defining the struct for the socket address for the server and client.
	char client_message[2000]; // Defining the message that will be transmitted through the socket between server and client.
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0); // Creating the socket with the parameters.
	if (socket_desc == -1) // Outputting an error message in the case that the socket might not get created.
	{
		printf("Could not create socket");
	}
	puts("Socket created"); // Outputting the information of succeded process of creating the socket.

	int true = 1;
	// Part below is to prevent the failure that is tried to get connected again in 1 minute after closing the server.
	 if (setsockopt(socket_desc,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int)) == -1) {
            perror("Setsockopt");
        }    
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT_NUMBER);
	
	//Bind the server using the method.
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done"); // Outputting result of successfully binded operation.
	
	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	//accept connection from an incoming client
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock < 0)
	{
		// print the error message that the acceptance is refused.
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");
	
	//Receive a message from client
	// Client message is received with two excessive unnecessary chars at the end, so it gets faulty when 
	// we try to compare the client message with the command string we would operate in the server side.
	// So we subtracted the last two chars in the char array, in order to compare the strings correctly.
	while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
	{
        client_message[read_size-2] = '\0';

	  // Using time.h library, we defined the necessary variables.
        struct timeval tv;
        time_t t;
        struct tm *info;
        char buffer[64];
 	  // Fetching current time information 
        gettimeofday(&tv, NULL);
        t = tv.tv_sec;
        info = localtime(&t);
	// Comparing the command sent by the client, if it is equal to the command, the local time information gets extracted
	// by the desired information of the date and time, using strftime method and a buffer to write it to the client side.
	// Strftime is used in order to convert date and time objects into their string equivalents.


        if(!strcmp("GET_TIME",client_message)){
			strftime (buffer, sizeof buffer, "%H:%M:%S\n", info); // Only hour, minute and second information is sent.
			write(client_sock , buffer , strlen(buffer));
			
            
            
        }else if(!strcmp("GET_DATE",client_message)){
			strftime (buffer, sizeof buffer, "%d.%m.%Y\n", info); // Only day, month and year information is sent. 
			write(client_sock , buffer , strlen(buffer));
            

        }else if(!strcmp("GET_TIME_ZONE",client_message)){
			strftime (buffer, sizeof buffer, "%z\n", info); // Only the time zone current time is present in is sent.
			write(client_sock , buffer , strlen(buffer));
            
        }else if(!strcmp("GET_TIME_DATE",client_message)){
			strftime (buffer, sizeof buffer, "%H:%M:%S, %d.%m.%Y\n", info); // Emerged information that is the concatenated
			// version of the GET_TIME and GET_DATE outputs.
			write(client_sock , buffer , strlen(buffer));
            
            
        }else if(!strcmp("GET_DAY_OF_WEEK",client_message)){
			strftime (buffer, sizeof buffer, "%A\n", info); // Only the day of the week, lexically.
			write(client_sock , buffer , strlen(buffer));
            
        }else if(!strcmp("CLOSE_SERVER",client_message)){
			write(client_sock , "GOOD BYE\n" , strlen("GOOD BYE\n"));
			close(client_sock);
			close(socket_desc);
			// Closing the server and ending the socket connection for the both of the sides.
			break;
            
        }else{
			write(client_sock , "INCORRECT REQUEST\n" , strlen("INCORRECT REQUEST\n")); 
			// When an unconvient input is gotten.
		}
	}
	// Below is for the failures about receing the message.
	
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
	
	return 0;
}
