#include <atlas/utils/Application.hpp>
#include <atlas/gl/ErrorCheck.hpp>
#include "GlobScene.hpp"

int main()
{
	APPLICATION.createWindow(900, 500, "Globular Physics");
	APPLICATION.addScene(new GlobScene);
	APPLICATION.runApplication();
	return 0;
}