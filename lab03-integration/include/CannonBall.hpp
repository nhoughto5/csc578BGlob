#ifndef LAB03_INCLUDE_CANNON_BALL_HPP
#define LAB03_INCLUDE_CANNON_BALL_HPP

#pragma once

#include <atlas/utils/Geometry.hpp>
#include <atlas/primitives/Sphere.hpp>

class CannonBall : public atlas::utils::Geometry
{
public:
    CannonBall();
    ~CannonBall();

    void updateGeometry(atlas::utils::Time const& t) override;
    void renderGeometry(atlas::math::Matrix4 projection,
        atlas::math::Matrix4 view) override;
    void stopAnimation(atlas::utils::Time const& t);
    void resetGeometry() override;

    enum class Integrator : int
    {
        EULER = 0,
        RK4
    };

    void setIntegrator(Integrator integrator);

private:
    void eulerIntegrator(atlas::utils::Time const& t);

    atlas::primitives::Sphere mRefBall;
    atlas::primitives::Sphere mModelBall;

    atlas::math::Vector mRefPosition;
    atlas::math::Vector mModelVelocity;
    atlas::math::Vector mModelPosition;
    atlas::math::Vector mForce;
    
    float mMass;

    atlas::math::Matrix4 mRefMatrix;
    Integrator mIntegrator;
};

#endif