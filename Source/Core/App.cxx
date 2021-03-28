#include "App.hpp"

void App::run()
{
    while (!window.shouldClose())
    {
        window.pollEvents();
    }
}