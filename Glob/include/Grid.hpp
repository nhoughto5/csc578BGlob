#ifndef LAB03_INCLUDE_GRID_HPP
#define LAB03_INCLUDE_GRID_HPP

#pragma once

#include <atlas/utils/Geometry.hpp>

class Grid : public atlas::utils::Geometry
{
public:
    Grid();
    ~Grid();

    void renderGeometry(atlas::math::Matrix4 projection, 
        atlas::math::Matrix4 view) override;

private:
    GLuint mBuffer;
    GLuint mVao;
    int mNumVertex;
};

#endif