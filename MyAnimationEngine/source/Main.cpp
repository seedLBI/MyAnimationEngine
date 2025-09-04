#ifndef _DEBUG﻿
    #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include "Application/Application.h"

int main() {

    Application app;
    app.Run();
    return 0;

}
