// MainLibrary.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "MainLibrary.h"
#include "PlainTextFileIO.h"
#include "WindowsHook.h"

//const char *fileName = 

MainLibrary::MainLibrary()
{
	//m_Hook = new WindowsHook();
	//m_WritingFile = new PlainTextFileIO();
}

bool MainLibrary::InitTextFile()
{

	return false;
}

bool MainLibrary::InstallHook(HWND hWnd)
{

	return false;
}
