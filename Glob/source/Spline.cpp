#include "Spline.h"
#include <atlas\core\Macros.hpp>
Spline::Spline(GLuint totalFrames_, std::vector<atlas::math::Point> controlPoints_, glm::mat4 mBasisMatrix_, int mResolution_, glm::vec3 colour_) :
	mControlPoints(controlPoints_),
	mBasisMatrix(mBasisMatrix_),
	mResolution(mResolution_),
	colour(colour_),
	mIsInterpolationDone(false),
	mTotalFrames(totalFrames_),
	mCurrentFrame(0)
{

}
Spline::~Spline() {

}
std::vector<atlas::math::Point> Spline::getControlPoints() {
	return mControlPoints;
}
glm::vec3 Spline::getColour() {
	return colour;
}
void Spline::updateGeometry(atlas::utils::Time const& t)
{
	UNUSED(t);
	mSplinePosition = interpolateOnSpline();

	mCurrentFrame++;
	if (mCurrentFrame == mTotalFrames)
	{
		mIsInterpolationDone = true;
		return;
	}
}

atlas::math::Point Spline::getSplinePosition()
{
	return mSplinePosition;
}
atlas::math::Point Spline::interpolateOnSpline()
{
	int n = int(mTable.size());
	float totalDistance = mTable[n - 1];
	float step = totalDistance / mTotalFrames;
	float currDistance = step * mCurrentFrame;

	int index = tableLookUp(currDistance);

	float t = (1.0f / mResolution) * (index % mResolution);
	return evaluateSpline(t);
}
atlas::math::Point Spline::evaluateSpline(float t) {
	USING_ATLAS_MATH_NS;

	Vector4 xControls =
	{
		mControlPoints[0].x, mControlPoints[1].x,
		mControlPoints[2].x, mControlPoints[3].x
	};

	Vector4 yControls =
	{
		mControlPoints[0].y, mControlPoints[1].y,
		mControlPoints[2].y, mControlPoints[3].y
	};

	Vector4 zControls =
	{
		mControlPoints[0].z, mControlPoints[1].z,
		mControlPoints[2].z, mControlPoints[3].z
	};

	Vector4 xCoeff = xControls * mBasisMatrix;
	Vector4 yCoeff = yControls * mBasisMatrix;
	Vector4 zCoeff = zControls * mBasisMatrix;

	float xcr, ycr, zcr;
	xcr = xCoeff[0] + t * xCoeff[1] + t * t * xCoeff[2] + t * t * t * xCoeff[3];
	ycr = yCoeff[0] + t * yCoeff[1] + t * t * yCoeff[2] + t * t * t * yCoeff[3];
	zcr = zCoeff[0] + t * zCoeff[1] + t * t * zCoeff[2] + t * t * t * zCoeff[3];

	return Point(xcr, ycr, zcr);
}
void Spline::generateArcLengthTable()
{
	USING_ATLAS_MATH_NS;

	if (!mTable.empty())
	{
		mTable.clear();
	}

	float scale = 1.0f / mResolution;
	mTable.push_back(0.0f);

	for (int i = 1; i < mResolution + 1; ++i)
	{
		Point p0 = evaluateSpline((i - 1) * scale);
		Point p1 = evaluateSpline(i * scale);

		Point dist = p0 - p1;
		mTable.push_back(mTable[i - 1] + glm::length(dist));
	}
}
int Spline::tableLookUp(float distance)
{
	// Find the entry in our table that corresponds to the given distance.
	float epsilon = chooseEpsilon();
	for (int i = 0; i < int(mTable.size()); ++i)
	{
		if (glm::abs(mTable[i] - distance) < epsilon)
		{
			return i;
		}
	}

	return -1;
}
float Spline::chooseEpsilon()
{
	// Find the largest difference and use that to look up distances
	// in our table.
	float epsilon = 0.0f;
	float diff;
	for (int i = 0; i < mTable.size() - 1; ++i)
	{
		diff = glm::abs(mTable[i] - mTable[i + 1]);
		if (diff > epsilon)
		{
			epsilon = diff;
		}
	}
	return epsilon;
}
bool Spline::doneInterpolation()
{
	return mIsInterpolationDone;
}