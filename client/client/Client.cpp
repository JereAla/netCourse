#include "Client.h"

Client::Client(std::string _ip, int _port)
{
	WSADATA wsaData;
	WORD dllVersion = MAKEWORD(2, 1);
	if (WSAStartup(dllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock startup failed", "ERROR :)", MB_OK | MB_ICONERROR);
		exit(1);
	}

	addr.sin_addr.s_addr = inet_addr(_ip.c_str()); //address (host ip)
	addr.sin_port = htons(_port);
	addr.sin_family = AF_INET; //IPv4 socket
	Clientptr = this;
}
bool Client::Connect()
{
	connection = socket(AF_INET, SOCK_STREAM, NULL);

	if (connect(connection, (SOCKADDR*)&addr, addrlen) != 0)
	{
		MessageBoxA(NULL, "Failed to connect.", "ERROR :)", MB_OK | MB_ICONERROR);
		return false;
	}
	printf("Connected\n");

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)_ClientThread, NULL, NULL, NULL);
	return true;
}
bool Client::Disconnect()
{
	if (closesocket(connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK)
			return true;

		std::string errorMsg = "Failed to close the socket. Winsock Error: " + std::to_string(WSAGetLastError());
		MessageBoxA(NULL, errorMsg.c_str(), "Error :)", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}
bool Client::_ProcessPacket(Packet _packetType)
{
	switch (_packetType)
	{
	case P_ChatMessage:
	{
		std::string message;
		if (!_GetString(message))
			return false;

		std::cout << message << "\n";
		break;
	}
	default:
		printf("Unreconized packet: %d\n", _packetType);
		break;
	}
	return true;
}
void Client::_ClientThread()
{
	Packet packetType;
	while (true)
	{
		if (!Clientptr->_GetPacketType(packetType))
			break;

		if (!Clientptr->_ProcessPacket(packetType))			//if packet not handled
			break;							//break loop (this should be changed if packet gets lost)
	}
	printf("Lost connection to the server.\n");
	if (Clientptr->Disconnect())
	{
		printf("Socket to the server was closed successfuly\n");
	}
	else
	{
		printf("Socket close failure\n");
	}
}

bool Client::_SendAll(char* _data, int _totalBytes)
{
	int bytesSend = 0;
	while (bytesSend < _totalBytes)
	{
		int returnCheck = send(connection, _data + bytesSend, _totalBytes - bytesSend, NULL);
		if (returnCheck == SOCKET_ERROR)
			return false;

		bytesSend += returnCheck;
	}
	return true;
}
bool Client::_RecvAll(char* _data, int _totalBytes)
{
	int bytesReceived = 0;
	while (bytesReceived <_totalBytes)
	{
		int returnCheck = recv(connection, _data + bytesReceived, _totalBytes - bytesReceived, NULL);
		if (returnCheck == SOCKET_ERROR)
			return false;

		bytesReceived += returnCheck;
	}
	return true;
}
bool Client::_Sendint32_t(int32_t _int32_t)
{
	_int32_t = htonl(_int32_t);		//convert long from host byte order to network byte order
	if (!_SendAll((char*)&_int32_t, sizeof(int32_t)))
		return false;

	return true;
}
bool Client::_Getint32_t(int32_t& _int32_t)
{
	if (!_RecvAll((char*)&_int32_t, sizeof(int32_t)))
		return false;

	_int32_t = ntohl(_int32_t); //convert long from network byte order to host byte order
	return true;
}
bool Client::_SendPacketType(Packet _packetType)
{
	if (!_Sendint32_t(_packetType))
		return false;

	return true;
}
bool Client::_GetPacketType(Packet& _packetType)
{
	int32_t packettype;
	if (!_Getint32_t(packettype))
		return false;

	_packetType = (Packet)packettype;
	return true;
}
bool Client::SendString(std::string& _string)
{
	if (!_SendPacketType(P_ChatMessage))
		return false;

	int32_t bufferLength = _string.size();
	if (!_Sendint32_t(bufferLength))
		return false;

	if (!_SendAll((char*)_string.c_str(), bufferLength))
		return false;

	return true;
}
bool Client::_GetString(std::string& _string)
{
	int32_t bufferLength;
	if (!_Getint32_t(bufferLength))
		return false;

	char* buffer = new char[bufferLength + 1];
	buffer[bufferLength] = '\0';

	if (!_RecvAll(buffer, bufferLength))
	{
		delete[] buffer;
		return false;
	}
	_string = buffer;
	delete[] buffer;
	return true;
}