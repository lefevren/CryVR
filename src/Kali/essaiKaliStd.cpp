#include "stdafx.h"
#include "essaiKaliStd.h"
/*
int initSynth() {
	DLL = LoadLibrary("\\michel\\kali\\kali.dll");
	MessageKali = (int (_stdcall*)(unsigned char* texte)) GetProcAddress(DLL,"_MessageKaliStd@4");
	DWORD err = GetLastError();
	QueryIndexKali = (int (_stdcall*)(void)) GetProcAddress(DLL,"_QueryIndexKaliStd@0");
	StopKali = (int (_stdcall*)(void)) GetProcAddress(DLL,"_StopKaliStd@0");
	SetVoixKali = (void (_stdcall*)(unsigned char nVoix)) GetProcAddress(DLL,"_SetVoixKaliStd@4");
	GetVoixKali = (unsigned char (_stdcall*)(void)) GetProcAddress(DLL,"_GetVoixKaliStd@0");
	GetNbVoixKali = (unsigned char (_stdcall*)(void)) GetProcAddress(DLL,"_GetNbVoixKaliStd@0");
	SetVolumeKali = (void (_stdcall*)(int volume)) GetProcAddress(DLL,"_SetVolumeKaliStd@4");
	GetVolumeKali = (int (_stdcall*)(void)) GetProcAddress(DLL,"_GetVolumeKaliStd@0");
	SetDebitKali = (void (_stdcall*)(int debit)) GetProcAddress(DLL,"_SetDebitKaliStd@4");
	GetDebitKali = (int (_stdcall*)(void)) GetProcAddress(DLL,"_GetDebitKaliStd@0");
	SetHauteurKali = (void (_stdcall*)(int hauteur)) GetProcAddress(DLL,"_SetHauteurKaliStd@4");
	GetHauteurKali = (int (_stdcall*)(void)) GetProcAddress(DLL,"_GetHauteurKaliStd@0");
	SetModeLectureKali = (void (_stdcall*)(int modeLecture)) GetProcAddress(DLL,"_SetModeLectureKaliStd@4");
	SetComptaKali = (void (_stdcall*)(int compta)) GetProcAddress(DLL,"_SetComptaKaliStd@4");
	SetEpelleKali = (void (_stdcall*)(int epelle)) GetProcAddress(DLL,"_SetEpelleKaliStd@4");
	SetSyntaxeKali = (void (_stdcall*)(int syntaxe)) GetProcAddress(DLL,"_SetSyntaxeKaliStd@4");
	SetProsodieKali = (void (_stdcall*)(int prosodie)) GetProcAddress(DLL,"_SetProsodieKaliStd@4");
	SetCoefPauseKali = (void (_stdcall*)(int coefPause)) GetProcAddress(DLL,"_SetCoefPauseKaliStd@4");
	SetLocalisationKali = (void (_stdcall*)(int localisation)) GetProcAddress(DLL,"_SetLocalisationKaliStd@4");
	SauveInitKali = (void (_stdcall*)(void)) GetProcAddress(DLL,"_SauveInitKaliStd@0");
	ExitKali = (void (_stdcall*)(void)) GetProcAddress(DLL,"_ExitKaliStd@0");

	SetModeLectureKali(0);
	SetComptaKali(0);
	SetEpelleKali(0);
	SetSyntaxeKali(1);
	SetProsodieKali(8);
	SetCoefPauseKali(4);
	SetLocalisationKali(50);
	return 0;
}

void exitSynth() {
	ExitKali();	//stoppe Kali
	FreeLibrary(DLL);	//décharge la DLL Kali
}
*/