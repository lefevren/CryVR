/* UdpListener header - for licensing and copyright see license.txt */


#include "StdAfx.h"
#include "winsock2.h"


class UdpListener {

private : 
	int server_length;
    int port;
	int STRLEN;
    char recMessage[1024];
    
	
	WSADATA wsaData;
    SOCKET mySocket;
    sockaddr_in myAddress;

	bool socketWorking;
	string problem;

	
	

public : 

	UdpListener();
	void EndSocket();
	bool StartSocket(int port);
	int ReceiveLine();

	bool IsWorking(){return socketWorking;}
	char* GetRecMessage(){return recMessage;}
	int GetMessageLength(){return STRLEN;}

	//AndroidDevice node
	int GetTokenCount();
	string GetToken(int i); //i is the index of the vector. message could contain more than 1 Vec3
	int GetId(string token);
	Vec3 TokenToVec3(string token);
	Vec3 TokenToVec2(string token);
	Quat TokenToQuat(string token);
	bool TokenToBool(string token);
	int TokenToInt(string token);
	float TokenToFloat(string token);
	
};