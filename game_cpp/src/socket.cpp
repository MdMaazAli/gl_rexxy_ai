#include "Socket.h"
#include <iostream>
#include <cstring>
using namespace std;

SOCKET initSocket(){
    WSADATA wsa;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsa);

    if(iResult != 0){
        cout<<"WSAStartup failed"<<endl;
        return INVALID_SOCKET;
    }

    cout<<"Winsock working"<<endl;

    // intialising address hints and result pointer poiting to addrinfo
    struct addrinfo *result = NULL,hints;
    memset(&hints,0,sizeof(hints));
    // address family & socket type
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    cout<<"enter the ip_addr:"<<endl;
    string ip_addr = "";
    cin>>ip_addr;
    const char* ip_address = ip_addr.c_str();

    iResult = getaddrinfo(ip_address,"5000",&hints,&result);
    if(iResult != 0){
        cout<<"getaddrinfo failed:"<<iResult<<endl;
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    // initialising a socket and pointing towards its atrributes
    SOCKET connectSocket=INVALID_SOCKET;
    if(ip_addr!=""){
        connectSocket = socket(
            result->ai_family,
            result->ai_socktype,
            result->ai_protocol   
        );
        
        // Blocking for RL sync
        u_long mode = 0; // 1 = Non-blocking, 0 = Blocking
        if (ioctlsocket(connectSocket, FIONBIO, &mode) != 0) {
            cout << "ioctlsocket failed with error: " << WSAGetLastError() << endl;
        }
        
        if(connectSocket == INVALID_SOCKET){
            cout<<"failed to create a socket"<<endl;
            freeaddrinfo(result);
            WSACleanup();
            return INVALID_SOCKET;
        }
        
        iResult = connect(connectSocket,result->ai_addr,(int)result->ai_addrlen);
        if(iResult == SOCKET_ERROR){
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK) { // If it's NOT just "would block," then it's a real failure
                cout << "connection failed with error: " << err << endl;
                closesocket(connectSocket);
                freeaddrinfo(result);
                WSACleanup();
                return INVALID_SOCKET;
            }
            // If it IS WSAEWOULDBLOCK, we continue! The connection will complete in the background.
            cout << "Connection in progress..." << endl;
        }
        else{
            cout<<"connected"<<endl;
        }
        freeaddrinfo(result);
    }
        
    return connectSocket;
}

void closeSocket(SOCKET connectSocket){
    closesocket(connectSocket);
    WSACleanup();
}