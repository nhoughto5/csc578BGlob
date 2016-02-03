#include <atlas/utils/Application.hpp>
#include <atlas/gl/ErrorCheck.hpp>
#include "SplineScene.hpp"

int main()
{
    APPLICATION.createWindow(400, 400, "integration");
    APPLICATION.addScene(new SplineScene);
    APPLICATION.runApplication();

    return 0;
}