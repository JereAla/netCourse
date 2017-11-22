#include <stdio.h>
#include <WinSock2.h>
#include <thread>
#include <mutex>
#include <list>
#include <string>
#pragma comment(lib,"ws2_32.lib")

#define SERVER "192.168.11.3"
#define BUFLEN 512
#define PORT 8888

std::list<std::string> msgbuf;
std::mutex msgbuf_mtx;

void ReceiveThread(int s){
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buf[BUFLEN];

	while (1)
	{
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen) == SOCKET_ERROR){
			printf("recv failure: &d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		msgbuf_mtx.lock();
		msgbuf.push_back(buf);
		msgbuf_mtx.unlock();
	}
}

int main(void)
{
	struct sockaddr_in si_other, si_this;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		printf("failure: %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	if (s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) == SOCKET_ERROR){
		printf("socket failure: %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

	memset((char*)&si_this, 0, sizeof(si_this));
	si_this.sin_family = AF_INET;
	si_this.sin_port = htons(PORT + 1);
	si_this.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(s, (struct sockaddr*)&si_this, sizeof(si_this)) == SOCKET_ERROR){
		printf("bind failure: %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	std::thread recthr(ReceiveThread, s);

	while (1)
	{
		printf("enter message: ");
		gets_s(message);
		if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR){
			printf("sendto failure: %d", WSAGetLastError());
			Sleep(1000);
			exit(EXIT_FAILURE);
		}

		Sleep(100);
		msgbuf_mtx.lock();
		while (!msgbuf.empty()){
			std::string s = msgbuf.front();
			msgbuf.pop_front();
			printf("%s\n", s.c_str());
		}
		msgbuf_mtx.unlock();
	}
	closesocket(s);
	WSACleanup();

	return 0;
}