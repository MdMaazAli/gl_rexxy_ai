#pragma once 
#include <winsock2.h>
#include <ws2tcpip.h>

SOCKET initSocket();
void closeSocket(SOCKET connectSocket);