#include "CApp.h"

#ifdef ANDROID
#include "android/native_activity.h"
void android_main( android_app * ) {
	CApp theApp;
	theApp.OnExecute();
}
#else

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
#endif


