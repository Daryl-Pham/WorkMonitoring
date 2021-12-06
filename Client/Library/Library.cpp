#include "stdafx.h"
#include "Library.h"
#include <WinUser.h>
#include <iostream>
#include <fstream>

#include <wtsapi32.h>
#include <UserEnv.h>

using namespace std;

void Library::WriteFileTest()
{

    ofstream myfile;
    myfile.open("example.txt");
    myfile << "Writing this to a file.\n";
    myfile.close();

}