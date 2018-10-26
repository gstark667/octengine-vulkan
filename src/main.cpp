#include "application.h"

#include <iostream>

#ifdef _WIN32
#define VALIDATION false
#define MAIN WinMain
#else
#define VALIDATION true
#define MAIN main
#endif


int MAIN()
{
    application_t app;
    app.enableValidationLayers = VALIDATION;
    app.windowWidth = 1920;
    app.windowHeight = 1080;
    app.fullscreen = true;
    app.windowWidth = 1280;
    app.windowHeight = 720;
    app.fullscreen = false;

    try {
        application_run(&app);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

