//if using VS15 release comment below--v
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Client.h"

#define PORT 1111
#define LocalHost "127.0.0.1"

//send int to tell lenght of sting
//send string

int main()
{
	Client myClient(LocalHost, PORT);

	if (!myClient.Connect())
	{
		printf("Failed to connect server...\n");
		system("pause");
		return 1;
	}
	
	std::string userInput;
	while (true)
	{
		std::getline(std::cin, userInput);
		if (!myClient.SendString(userInput))
			break;
		Sleep(10);
	}

	system("pause");
	return 0;
}