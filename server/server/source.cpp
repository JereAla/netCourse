//if using VS15 release comment below--v
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Server.h"


int main()
{
	Server myServer(1111);

	for (int i = 0; i < 100; i++)
	{
		myServer.ListenNewConnection();
	}
	
	system("pause");
	return 0;
}