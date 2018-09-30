#include "application.h"

#include <iostream>


int main()
{
    application_t app;
    app.enableValidationLayers = true;
    app.windowWidth = 1280;
    app.windowHeight = 720;
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

