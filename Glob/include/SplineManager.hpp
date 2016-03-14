#ifndef SPLINE_MANAGER_HPP
#define SPLINE_MANAGER_HPP
#pragma once
#include <atlas/utils/Geometry.hpp>
#include "Spline.h"
class SplineManager : public atlas::utils::Geometry {
public:
	SplineManager(GLuint framesPerSpline_);
	~SplineManager();

	void renderGeometry(atlas::math::Matrix4 projection, atlas::math::Matrix4 view) override;
	void updateGeometry(atlas::utils::Time const& t) override;

	void addSplines();
	void showControlPoints();
	void showCage();
	void showSplinePoints();
	void showSpline();
	bool doneInterpolation();
	atlas::math::Point getSplinePosition();
private:
	atlas::math::Point interpolateOnSpline();
	void setUpVAOs();
	//==== Memebers ====//
	GLuint splinePointVertexArrayObject, controlPointVertexArrayObject, mControlBuffer, mSplineBuffer, currentSpline;
	std::vector<Spline> mSplines;
	std::vector<glm::vec3> allSplinePoints, allControlPoints;
	GLuint framesPerSpline, mResolution, mCurrentFrame;
	glm::mat4 mBasisMatrix;
	bool mShowControlPoints;
	bool mShowCage;
	bool mShowSplinePoints;
	bool mShowSpline;
	bool mIsInterpolationDone;

};
#endif