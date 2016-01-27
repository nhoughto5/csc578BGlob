#include <atlas/utils/Application.hpp>
#include <atlas/gl/ErrorCheck.hpp>
#include "IntegrationScene.hpp"

int main()
{
    APPLICATION.createWindow(400, 400, "integration");
    APPLICATION.addScene(new IntegrationScene);
    APPLICATION.runApplication();

    return 0;
}