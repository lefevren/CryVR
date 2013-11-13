/* UdpListener class - for licensing and copyright see license.txt */

#include "StdAfx.h"
#include "winsock2.h"
#include "Android/UdpListener.h"

UdpListener::UdpListener()
	{
		socketWorking = false;
		//m_bEnabled = false;
		problem = "Nothing done";
	}

void UdpListener::EndSocket() {
		socketWorking = false;
		closesocket(mySocket);
		WSACleanup();
	}

bool UdpListener::StartSocket(int port_) {
		STRLEN = 1024;
		socketWorking = false;

		if( WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR )
		{
			problem = "Socket Initialization: Error with WSAStartup\n";
			WSACleanup();

		} else {


			mySocket = socket(AF_INET, SOCK_DGRAM, 0);
			if (mySocket == INVALID_SOCKET)
			{
				problem = "Socket Initialization: Error creating socket";
				WSACleanup();
			} else {

				port = port_;

				//bind
				myAddress.sin_family = AF_INET;
				myAddress.sin_addr.s_addr = inet_addr( "0.0.0.0" );
				myAddress.sin_port = htons(port);
				
				// set up as non-blocking
				u_long arg = 1;
				ioctlsocket(mySocket, FIONBIO, &arg);
  
				if(bind(mySocket, (SOCKADDR*) &myAddress, sizeof(myAddress)) == SOCKET_ERROR)
				{
					problem = "ServerSocket: Failed to connect\n";
					WSACleanup();
				} else {
					// all went well, send Success signal, and set details
					socketWorking = true;
					problem = "no problem";
					return true;
				}

			}
		}

		return false;
	}

	
int UdpListener::ReceiveLine() {
		int size = -1;
		if (socketWorking) {
			server_length = sizeof(struct sockaddr_in);
			size = recvfrom(mySocket, recMessage, STRLEN, 0, (SOCKADDR*) &myAddress, &server_length);
			/*
			if (size == SOCKET_ERROR) {
				  // get last error
				switch(WSAGetLastError()) {
				  case WSANOTINITIALISED:
						return "WSANOTINITIALISED";
				  case WSAENETDOWN:
						return "WSAENETDOWN";
				  case WSAEFAULT:
					  return "WSAEFAULT";
				  case WSAENOTCONN:
					  return "WSAENOTCONN";
				  case WSAEINTR:
					  return "WSAEINTR";
				  case WSAEINPROGRESS:
					  return "WSAEINPROGRESS";
				  case WSAENETRESET:
					  return "WSAENETRESET";
				  case WSAENOTSOCK:
					  return "WSAENOTSOCK";
				  case WSAEOPNOTSUPP:
					  return "WSAEOPNOTSUPP";
				  case WSAESHUTDOWN:
					  return "WSAESHUTDOWN";
				  case WSAEWOULDBLOCK:
					  return "WSAEWOULDBLOCK";
				  case WSAEMSGSIZE:
					  return "WSAEMSGSIZE";
				  case WSAEINVAL:
					  return "WSAEINVAL";
				  case WSAECONNABORTED:
					  return "WSAECONNABORTED";
				  case WSAETIMEDOUT:
					  return "WSAETIMEDOUT";
				  case WSAECONNRESET:
					  return "WSAECONNRESET";
				  default:
					  return "UNKNOWN SOCKET ERROR";
				}
			}*/
			if (size != SOCKET_ERROR) {
				recMessage[size] = '\0';
			} 
		}
		return size;
	}



int UdpListener::GetTokenCount(){
	string message = recMessage;
	int i=0;
	int nombre = -1;
	while(i<STRLEN){
		if(message[i]=='[') nombre ++;
		i++;
	}

	return nombre;
}

string UdpListener::GetToken(int index){
	//GetVec3 @ index 
	//from a formatted message
	//Formatted string type :
	//[] => separator
	//[Vec3 => Vec3 value
	//[Vec2 => Vec2 value etc.
	//[CryVR][Vec3:x0,y0,z0][Float:3.5684][String,"string2"][Vec2:x3,y3][Quat:x4,y4,z4,w4][Cvar,e_timeofday,5.856]

				
	string message = recMessage;
	int indice = message.find("[CryVR]",0);
	if (indice != 0) return "-1";
	for(int i=0;i<=index;i++){
		indice = message.find("[",indice+1);
	}
	int indice_fin = message.find("]",indice);
	return message.substr(indice,indice_fin-indice);
}

Vec3 UdpListener::TokenToVec3(string token){
	string data = token.substr(1,token.length()-2); // A tester
	if(data.substr(0,4)!="Vec3") return Vec3(0,0,0);
	data = data.substr(5,data.length()-5);
	
	int indice = data.find(",");
	float x = (float)atof(data.substr(0,indice));
	
	int n_indice = data.find(",",indice+1);
	float y = (float)atof(data.substr(indice+1,n_indice));
	indice = n_indice;


	n_indice = data.find(",",indice+1);
	float z = (float)atof(data.substr(indice+1,n_indice));
	indice = n_indice;

	return Vec3(x,y,z);
}


Quat UdpListener::TokenToQuat(string token){
	//Sup first and last char
	string data = token.substr(1,token.length()-2); // A tester
	if(data.substr(0,4)!="Quat") return Quat(0,0,0,0);
	data = data.substr(5,data.length()-5);
	
	int indice = data.find(",");
	float w = (float)atof(data.substr(0,indice));
	
	int n_indice = data.find(",",indice+1);
	float x = (float)atof(data.substr(indice+1,n_indice));
	indice = n_indice;


	n_indice = data.find(",",indice+1);
	float y = (float)atof(data.substr(indice+1,n_indice));
	indice = n_indice;
	

	n_indice = data.find(",",indice+1);
	float z = (float)atof(data.substr(indice+1,n_indice));
		
	return Quat(w,x,y,z);
}



int UdpListener::TokenToInt(string token){
	string data = token.substr(1,token.length()-2); // A tester
	if(data.substr(0,3)!="Int") return -12345678;
	data = data.substr(4,data.length()-4);
	
	int indice = data.find(",");
	int x = atoi(data.substr(0,indice));
	return x;
}


float UdpListener::TokenToFloat(string token){
	string data = token.substr(1,token.length()-2); // A tester
	if(data.substr(0,5)!="Float") return -12345678;
	data = data.substr(6,data.length()-6);
	int indice = data.find(",");
	float x = (float)atof(data.substr(0,indice));
	return x;
}

Vec3 UdpListener::TokenToVec2(string token){
	//Sup first and last char
	string data = token.substr(1,token.length()-2); // A tester
	if(data.substr(0,4)!="Vec3") return Vec3(0,0,0);
	data = data.substr(5,data.length()-5);
	
	int indice = data.find(",");
	float x = (float)atof(data.substr(0,indice));
	
	int n_indice = data.find(",",indice+1);
	float y = (float)atof(data.substr(indice+1,n_indice));
	indice = n_indice;

	

	return Vec3(x,y,0);
};
