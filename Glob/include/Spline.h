#ifndef SPLINE_H
#define SPLINE_H

#include <atlas/utils/Geometry.hpp>
USING_ATLAS_MATH_NS;
USING_ATLAS_GL_NS;
class Spline{
public:
	Spline(GLuint totalFrames_, std::vector<atlas::math::Point> controlPoints_, glm::mat4 mBasisMatrix_, int mResolution_, glm::vec3 colour_);
	~Spline();
	std::vector<atlas::math::Point> getControlPoints();
	glm::vec3 getColour();

	atlas::math::Point evaluateSpline(float t);
	void generateArcLengthTable();
	atlas::math::Point getSplinePosition();
	void updateGeometry(atlas::utils::Time const& t);
	bool doneInterpolation();
private:
	
	int tableLookUp(float distance);
	float chooseEpsilon();
	
	atlas::math::Point interpolateOnSpline();
	GLuint mTotalFrames, mCurrentFrame;
	std::vector<atlas::math::Point> mControlPoints;
	atlas::math::Point mSplinePosition;
	std::vector<float> mTable;
	glm::vec3 colour;
	glm::mat4 mBasisMatrix;
	int mResolution;
	bool mIsInterpolationDone;
};
#endif // !SPLINE_H
