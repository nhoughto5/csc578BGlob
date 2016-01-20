#ifndef LAB02_INCLUDE_SPHERE_HPP
#define LAB02_INCLUDE_SPHERE_HPP

#pragma once

#include <atlas/utils/Geometry.hpp>

class ColorCube : public atlas::utils::Geometry
{
public:
    ColorCube();
    ~ColorCube();

    void updateGeometry(atlas::utils::Time const& t) override;
    void renderGeometry(atlas::math::Matrix4 projection,
        atlas::math::Matrix4 view) override;
    void resetGeometry() override;


private:
    GLuint mVao;
    GLuint mVertexBuffer;
    GLuint mColorBuffer;

    bool mIsStopped;

    atlas::math::Vector mVelocity;
    atlas::math::Vector mPosition;
};

#endif