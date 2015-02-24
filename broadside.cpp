#include "CApp.h"

#ifdef _WIN32
int wmain() {
#else
int main() {
#endif
    CApp theApp;
    return theApp.OnExecute();
}

