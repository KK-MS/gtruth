#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

// Threads: to run streaming server
#include <process.h>

// Network related includes
//#undef UNICODE

// Need older API support e.g. IPV4. Else result in "Error E0040"
// E0040 expected an identifier ws2def.h
#define WIN32_LEAN_AND_MEAN 



#include <windows.h> // DEFAULT_PORT
#include <ws2tcpip.h> // getaddrinfo, includes #include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define INPUT_SERVER_PORT     27015 
#define INPUT_SERVER_IP     "127.0.0.1"
#define MAXLINE 1024 

using namespace std;

// GLOBAL VARIABLES

static SOCKET ClientSocket = INVALID_SOCKET;
static SOCKET s;
static SOCKADDR_IN servaddr;

char buffer[100];
const char *message = "Hello Server";

static HANDLE hLocalizeRxThread;

void localize_rx_thread(void *param)
{
	int iResult;

	// Send to server
	printf("\nIn Localize client thread\n");
	
	do {
		
		iResult = sendto(s, message, 10, 0, (struct sockaddr*)NULL, sizeof(servaddr));
	//	printf("localize sendto stereo:%d \n", iResult);
		//iResult = sendto(s, message, 88, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
		if (iResult < 0) {
			printf("\n Error localize_network_input_init : sendto %d \n", WSAGetLastError());
			goto end_client_thread;
		}

		// waiting for response 
	//	printf("localize to recvfrom\n");
		int len = sizeof(sockaddr_in); // need to specify lenght of address, else server will show as socket is not available 10057
									   //iResult = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, &len);
		iResult = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
		if (iResult < 0) {
			printf("\n Error localize_network_input_init : recvfrom %d \n", WSAGetLastError());
			goto end_client_thread;
		}
	//	printf("localize recvfrom stereo %d\n", iResult);

	} while (iResult > 0);

end_client_thread:
	printf("Localize client thread closing...\n");
	closesocket(s);
	WSACleanup();

	return;
}

int localize_network_input_init()
{
	WSADATA wsaData;

	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	s = socket(AF_INET, SOCK_DGRAM, 0); //Create socket
	if (s == INVALID_SOCKET) {
		printf("Error localize_network_input_init socket %d\n", WSAGetLastError());
		return false; //Couldn't create the socket
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(INPUT_SERVER_PORT); //Port to connect on
	servaddr.sin_addr.s_addr = inet_addr(INPUT_SERVER_IP);
	servaddr.sin_port = IPPROTO_UDP;

	//INADDR_ANY; //accept any source address? 
	// if we give server IP/ local address, then server sendto is resulting in error 
	// that client socket is not availble inet_addr(INPUT_SERVER_IP); //Target IP

	// connect to server 
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		printf("\n Error localize_network_input_init : Connect %d \n", WSAGetLastError());
		return 0; //exit(0);
	}

	printf("Starting localize_rx_thread\n");
	
	hLocalizeRxThread = (HANDLE)_beginthread(localize_rx_thread, 0, NULL);

	return 0;
}
 


int localize_input_init()
{
	printf("In localize_input_init\n");
	localize_network_input_init();

	return 0;
}

int localize_input_deinit()
{
	printf("In localize_input_deinit\n");
	
	closesocket(s);
	
	// deinit the network connection
	WSACleanup();
	
	printf("In WaitForSingleObject hLocalizeRxThread\n");
	// WAIT for Server loop to end
	WaitForSingleObject(hLocalizeRxThread, INFINITE);

	printf("localize_input_deinit ends\n");
	return 0;
}

int localize_input_process()
{
	int iResult;
	printf("In localize_input_process\n");
	// waiting for response 
	iResult = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
	if (iResult < 0) {
		printf("\n Error localize_network_input_init : recvfrom %d \n", WSAGetLastError());
		return 0;
	}
	return 0;
}