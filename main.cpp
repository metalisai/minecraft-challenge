#include <GL/glew.h>
#include <stdio.h>
#include "application.h"

// day 2 start!
// day 3 start!

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
