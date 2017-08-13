#include <GL/glew.h>
#include <stdio.h>
#include "application.h"

int main(int argc, char *argv[])
{
    Application app;
    app.start();

    while (app.isRunning())
    {
        app.doEvents();
        app.doFrame();
    }

    return 0;
}
