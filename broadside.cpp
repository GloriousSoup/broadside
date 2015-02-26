#include "CApp.h"

#ifdef _WIN32
//int wmain() {
#include <Windows.h>
int WINAPI WinMain( 
HINSTANCE hInstance, 
HINSTANCE hPrevInstance, 
LPSTR lpCmdLine, 
int nCmdShow) {
#else
int main() {
#endif
    CApp theApp;
    return theApp.OnExecute();
}

