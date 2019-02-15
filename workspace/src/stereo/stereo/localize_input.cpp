#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
// Network related includes
// #undef UNICODE
// Need older API support e.g. IPV4. Else result in "Error E0040"
// E0040 expected an identifier ws2def.h
#define WIN32_LEAN_AND_MEAN 

#include <stdlib.h>
#include <stdio.h>

#include <windows.h> // DEFAULT_PORT
#include <ws2tcpip.h> // getaddrinfo, includes #include <winsock2.h>

#include <iostream>

#include "localize_input.h"

// Pragmas
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define INPUT_SERVER_PORT     27015 
#define INPUT_SERVER_IP     "127.0.0.1"
#define MAXLINE 1024 

using namespace std;

// Macros
#define TAG_LIN "LIN: "

// GLOBAL VARIABLES

static SOCKET ClientSocket = INVALID_SOCKET;
static SOCKET s;
static SOCKADDR_IN servaddr;


// Functions
int localize_input_request_server(char *req_cmd, char *res_buf, int res_len)
{
	// printf("\nIn request_server M: %d, Rx:%d\n", sizeof(req_cmd), res_len);
	// Request stereo server for the metadata.		
	if (sendto(s, req_cmd, sizeof(req_cmd), 0, NULL, 0) < 0) { goto ret_err; }

	// waiting and obtain metadata response 
	if (recvfrom(s, res_buf, res_len, 0, NULL, NULL) < 0) { goto ret_err; }
	//cv::cout << TAG_SNT "localize: Got REQ_IMAGES" << endl;
	return 0;

ret_err:
	printf(TAG_LIN "Localize: LastError :%d\n", WSAGetLastError());
	return -1;

}

int localize_input_network_init()
{
	WSADATA wsaData;
	int ret_val;

	// Initialize Winsock
	ret_val = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret_val != 0) {	goto ret_err; }

	s = socket(AF_INET, SOCK_DGRAM, 0); //Create socket
	if (s == INVALID_SOCKET) { goto ret_err; }

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(INPUT_SERVER_PORT); //Port to connect on
	servaddr.sin_addr.s_addr = inet_addr(INPUT_SERVER_IP);
	servaddr.sin_port = IPPROTO_UDP;

	// connect to server 
	ret_val = connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret_val < 0) { goto ret_err; }
	
	return 0;

ret_err:
	printf(TAG_LIN "Error in localize_input_network_init: %d \n", WSAGetLastError());
	return -1;
}
 
int localize_input_init(struct stereo_object *ptr_system_object)
{
	printf(TAG_LIN "In localize_input_init\n");
	
	// TODO: Ring buffer
	//
	// allocate memory to store the jpeg bytes and decompressed frame
	//
	ptr_system_object->ptr_frame_left =
		(unsigned char *)malloc(MAX_FRAME_SIZE);

	ptr_system_object->ptr_frame_right =
		(unsigned char *)malloc(MAX_FRAME_SIZE);

	ptr_system_object->ptr_jpeg_stream =
		(unsigned char *)malloc(MAX_FRAME_SIZE * 2);

	// 
	// Network init
	//
	localize_input_network_init();

	return 0;
}

int localize_input_deinit(struct stereo_object *ptr_system_object)
{
	printf(TAG_LIN "In localize_input_deinit\n");
	
	// closer network first
	closesocket(s);
	
	// deinit the network connection
	WSACleanup();

	// Free the allocated memory at last
	free(ptr_system_object->ptr_frame_left);
	free(ptr_system_object->ptr_frame_right);
	free(ptr_system_object->ptr_jpeg_stream);

	printf(TAG_LIN "In localize: localize_input_deinit ends\n");
	return 0;
}


int localize_input_request_metadata(char *cmd, char *buf, int len)
{
	return localize_input_request_server(cmd, buf, len);
}

int localize_input_request_images(char *cmd, char *buf, int len)
{
	return localize_input_request_server(cmd, buf, len);
}

int localize_input_process(struct stereo_object *ptr_system_object)
{
	int iResult;
	printf(TAG_LIN "In localize_input_process\n");
	return 0;
}