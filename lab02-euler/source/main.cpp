#include <atlas/utils/Application.hpp>
#include <atlas/gl/ErrorCheck.hpp>
#include "EulerScene.hpp"

int main()
{
    APPLICATION.createWindow(400, 400, "gl-sphere");
    APPLICATION.addScene(new EulerScene);
    APPLICATION.runApplication();

    return 0;
}