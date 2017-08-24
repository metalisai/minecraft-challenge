#include <GL/glew.h>
#include <stdio.h>
#include "application.h"

// day 2 start!
// day 3 start!
// day 4 start!
// day 5 start!
// day 6 start!
// day 7 start! Is this finally over?
// day 8 start - cheat day

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
