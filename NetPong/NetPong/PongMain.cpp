#include <WinSock2.h>
#include <stdio.h>
#pragma comment (lib,"ws2_32.lib" )
#define BUFLEN 512
#define PORT 8888


int main()
{
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;
	
	slen = sizeof(si_other);
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		printf("failed: %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET){
		printf("Couldn't create socket: %d", WSAGetLastError());
	}
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);
	
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR){
		printf("Bind failure: %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	
	while (1)
	{
		printf("waiting for data...");
		memset(buf, 0, BUFLEN);
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == SOCKET_ERROR){
			printf("recv failure: %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		if (sendto(s, buf, recv_len, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR){
			printf("sendto failure: %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		printf("here");
	}
	closesocket(s);
	WSACleanup();
	return 0;
}