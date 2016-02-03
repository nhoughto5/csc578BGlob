#ifndef LAB04_INCLUDE_SPLINE_SCENE_HPP
#define LAB04_INCLUDE_SPLINE_SCENE_HPP

#pragma once

#include <atlas/utils/Scene.hpp>

#include "MayaCamera.hpp"
#include "Grid.hpp"
#include "Ball.hpp"
#include "Spline.hpp"

class SplineScene : public atlas::utils::Scene
{
public:
    SplineScene();
    ~SplineScene();

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

    float mLastTime;
    float mFPS;
    float mTick;

    float mAnimTime;
    float mAnimLength;

    MayaCamera mCamera;
    Grid mGrid;
    Spline mSpline;
    Ball mBall;
};

#endif