#ifndef LAB02_INCLUDE_EULER_SCENE_HPP
#define LAB02_INCLUDE_EULER_SCENE_HPP

#pragma once

#include <atlas/utils/Scene.hpp>

#include "MayaCamera.hpp"
#include "Grid.hpp"
#include "ColorCube.hpp"

class EulerScene : public atlas::utils::Scene
{
public:
    EulerScene();
    ~EulerScene();

    void mousePressEvent(int button, int action, int modifiers, 
        double xPos, double yPos) override;
    void mouseMoveEvent(double xPos, double yPos) override;
    void keyPressEvent(int key, int scancode, int action, int mods) override;

    void screenResizeEvent(int width, int height) override;
    void renderScene() override;
    void updateScene(double time) override;

private:
    bool mIsDragging;
    bool mIsPlaying;
    double mLastTime;
    MayaCamera mCamera;
    Grid mGrid;
    ColorCube mCube;
};

#endif