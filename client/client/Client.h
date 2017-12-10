#ifndef Client_H
#define Client_H
//#pragma once
#pragma comment(lib,"ws2_32.lib")

#include <WinSock2.h>
#include <iostream>
#include <stdint.h> //use int32_t
#include <string>


enum Packet
{
	P_ChatMessage,
	P_Test
};


class Client
{
public:
	Client(std::string _ip, int _port);
	//~Client();
	bool Connect();
	bool Disconnect();

	bool SendString(std::string& _string);

private:
	bool _ProcessPacket(Packet _packetType);
	static void _ClientThread();

	bool _SendAll(char* _data, int _totalBytes);
	bool _RecvAll(char* _data, int _totalBytes);
	bool _Sendint32_t(int32_t _int32_t);
	bool _Getint32_t(int32_t& _int32_t);
	bool _SendPacketType(Packet _packetType);
	bool _GetPacketType(Packet& _packetType);

	bool _GetString(std::string& _string);

	SOCKET connection;
	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
};

static Client* Clientptr;
#endif