#ifndef LAB01_INCLUDE_SIMPLE_GL_TRIANGLE_HPP
#define LAB01_INCLUDE_SIMPLE_GL_TRIANGLE_HPP

#pragma once

#include <atlas/utils/Geometry.hpp>

class Triangle : public atlas::utils::Geometry
{
public:
    Triangle();
    ~Triangle();

    void renderGeometry(atlas::math::Matrix4 projection, 
        atlas::math::Matrix4 view) override;

private:
    GLuint mVao;
    GLuint mBuffer;
};

#endif