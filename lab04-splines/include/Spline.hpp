#ifndef LAB04_INCLUDE_SPLINE_HPP
#define LAB04_INCLUDE_SPLINE_HPP

#pragma once

#include <atlas/utils/Geometry.hpp>

class Spline : public atlas::utils::Geometry
{
public:
    Spline(int totalFrames);
    ~Spline();

    void renderGeometry(atlas::math::Matrix4 projection,
        atlas::math::Matrix4 view) override;

    void updateGeometry(atlas::utils::Time const& t) override;

    void showControlPoints();
    void showCage();
    void showSplinePoints();
    void showSpline();
    bool doneInterpolation();

    atlas::math::Point getSplinePosition();

private:
    atlas::math::Point interpolateOnSpline();

    atlas::math::Point evaluateSpline(float t);
    void generateArcLengthTable();
    int tableLookUp(float distance);
    float chooseEpsilon();

    atlas::math::Matrix4 mBasisMatrix;
    std::vector<atlas::math::Point> mControlPoints;

    std::vector<float> mTable;

    atlas::math::Point mSplinePosition;

    GLuint mVao;
    GLuint mControlBuffer;
    GLuint mSplineBuffer;

    int mResolution;
    int mTotalFrames;
    int mCurrentFrame;

    bool mShowControlPoints;
    bool mShowCage;
    bool mShowSplinePoints;
    bool mShowSpline;
    bool mIsInterpolationDone;
};

#endif