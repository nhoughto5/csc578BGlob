#include <atlas/utils/Application.hpp>
#include "SimpleGLScene.hpp"

int main()
{
    // Create a new window (only one per application for now).
    APPLICATION.createWindow(400, 400, "Simple GL");

    // Now give it our scene.
    APPLICATION.addScene(new SimpleGLScene);
    
    // Finally, run the application.
    APPLICATION.runApplication();

    return 0;
}