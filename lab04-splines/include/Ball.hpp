#ifndef LAB04_INCLUDE_BALL_HPP
#define LAB04_INCLUDE_BALL_HPP

#pragma once

#include <atlas/utils/Geometry.hpp>
#include <atlas/primitives/Sphere.hpp>

class Ball : public atlas::utils::Geometry
{
public:
    Ball();
    ~Ball();

    void renderGeometry(atlas::math::Matrix4 projection,
        atlas::math::Matrix4 view) override;
    void transformGeometry(atlas::math::Matrix4 const& t) override;

private:

    atlas::primitives::Sphere mBall;
};

#endif