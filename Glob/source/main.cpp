#include <atlas/utils/Application.hpp>
#include <atlas/gl/ErrorCheck.hpp>
#include "GlobScene.hpp"

int main()
{
    APPLICATION.createWindow(400, 400, "integration");
    APPLICATION.addScene(new GlobScene);
    APPLICATION.runApplication();

    return 0;
}