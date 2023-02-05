#pragma once

class WindowsHook;
class BaseFileIO;

class MainLibrary
{
protected:
	WindowsHook* m_Hook;
	BaseFileIO* m_WritingFile;

public:
	MainLibrary();

private:
	bool InitTextFile();
	bool InstallHook(HWND hWnd);

};

