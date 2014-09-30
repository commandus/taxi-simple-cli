// taxi-simple-svc.cpp : Defines the entry point for the console application.
//
#include <stdio.h>

int doServer(int, char**);

#ifdef WIN32

#include <tchar.h>
#include "winsock.h"
#include "argvcharsetconv.h"

int _tmain(int argc, _TCHAR* argv[])
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable WinSock DLL */
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		return 1;
	}

	char **argv_utf8 = argvConv(argc, argv);
	doServer(argc, argv_utf8);
	argvFree(argc, argv_utf8);

	WSACleanup();
	return 0;
}
#else
int main(int argc, char* argv[])
{
        doServer(argc, argv);
        return 0;
}
#endif
