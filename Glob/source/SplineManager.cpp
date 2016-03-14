#include "SplineManager.hpp"
#include "ShaderPaths.hpp"
#include <iostream>
SplineManager::SplineManager(GLuint framesPerSpline_) :
	framesPerSpline(framesPerSpline_),
	mResolution(500),
	mCurrentFrame(0),
	mShowControlPoints(false),
	mShowCage(false),
	mShowSplinePoints(false),
	mShowSpline(false),
	mIsInterpolationDone(false),
	currentSpline(0)
{
	//Bezier
	mBasisMatrix = Matrix4(
		1.0f, 0.0f, 0.0f, 0.0f,
		-3.0f, 3.0f, 0.0f, 0.0f,
		3.0f, -6.0f, 3.0f, 0.0f,
		-1.0f, 3.0f, -3.0f, 1.0f);
	addSplines();

	float scale = 1.0f / mResolution;
	for (GLuint i = 0; i < mSplines.size(); ++i) {
		for (GLuint res = 0; res < mResolution + 1; ++res) {
			allSplinePoints.push_back(mSplines[i].evaluateSpline(scale * res));
			allSplinePoints.push_back(mSplines[i].getColour());
		}
		mSplines[i].generateArcLengthTable();
	}

	glGenBuffers(1, &mControlBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mControlBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * allControlPoints.size(), allControlPoints.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &mSplineBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mSplineBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * allSplinePoints.size(), allSplinePoints.data(), GL_STATIC_DRAW);

	setUpVAOs();

	std::string shaderDir = generated::ShaderPaths::getShaderDirectory();
	std::vector<ShaderInfo> shaders
	{
		{ GL_VERTEX_SHADER, shaderDir + "spline.vs.glsl" },
		{ GL_FRAGMENT_SHADER, shaderDir + "spline.fs.glsl" }
	};

	mShaders.push_back(ShaderPointer(new Shader));
	mShaders[0]->compileShaders(shaders);
	mShaders[0]->linkShaders();

	GLuint var;
	var = mShaders[0]->getUniformVariable("uMVP");
	mUniforms.insert(UniformKey("uMVP", var));

	mShaders[0]->disableShaders();
}
SplineManager::~SplineManager() {

}
void SplineManager::setUpVAOs() {
	glGenVertexArrays(1, &controlPointVertexArrayObject);
	glGenVertexArrays(1, &splinePointVertexArrayObject);

	glBindVertexArray(splinePointVertexArrayObject);
	glEnableVertexAttribArray(0); //SplinePoint positions
	glEnableVertexAttribArray(1); //Spline point Colours
	glBindBuffer(GL_ARRAY_BUFFER, mSplineBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

	glBindVertexArray(controlPointVertexArrayObject);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mControlBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}
void SplineManager::renderGeometry(atlas::math::Matrix4 projection, atlas::math::Matrix4 view) {
	USING_ATLAS_MATH_NS;
	mShaders[0]->enableShaders();

	glBindVertexArray(splinePointVertexArrayObject);
	Matrix4 mvp = projection * view * mModel;
	glUniformMatrix4fv(mUniforms["uMVP"], 1, GL_FALSE, &mvp[0][0]);

	glLineWidth(5.0f);
	glDrawArrays(GL_LINE_STRIP, 0, ((size_t)(mResolution + 1)) * mSplines.size());
	glLineWidth(1.0f);
	mShaders[0]->disableShaders();
}

atlas::math::Point SplineManager::getSplinePosition() {
	return mSplines[currentSpline].getSplinePosition();
}
void SplineManager::updateGeometry(atlas::utils::Time const& t) {
	mSplines[currentSpline].updateGeometry(t);
	if (mSplines[currentSpline].doneInterpolation()) {
		++currentSpline;
	}
	if (currentSpline == mSplines.size()) mIsInterpolationDone = true;
}

//This is where splines are created and added to a vector
void SplineManager::addSplines() {

	std::vector<Point> tempControlPoints = std::vector<Point>
	{
		{ -10.0f, 5.0f, 0.0f }, //A0
		{ -10.0f, 5.0f, 4.0f }, //A1
		{ -4.0f, 5.0f, 10.0f }, //A2
		{ 0.0f, 5.0f, 10.0f } //A3
	};
	glm::vec3 splineColour{ 1.0f, 0.0f, 0.0f };


	std::vector<Point> tempControlPoints2 = std::vector<Point>{
		tempControlPoints[3], //B0
		{ 4.0f, 5.0f, 10.0f }, //B1
		{ 10.0f, 5.0f, 4.0f }, //B2
		{ 10.0f, 5.0f, 0.0f }, //B3
	};
	glm::vec3 splineColour2 = glm::vec3{ 0.0f, 1.0f, 0.0f };

	mSplines.push_back(Spline(framesPerSpline, tempControlPoints, mBasisMatrix, mResolution, splineColour));
	mSplines.push_back(Spline(framesPerSpline, tempControlPoints2, mBasisMatrix, mResolution, splineColour2));
}
void SplineManager::showControlPoints() {

}
void SplineManager::showCage() {

}
void SplineManager::showSplinePoints() {

}
void SplineManager::showSpline() {

}
bool SplineManager::doneInterpolation() {
	bool X = true;
	//If any of the splines are still interpolating send false
	//Otherwise send true
	for (int i = 0; i < mSplines.size(); ++i) {
		if (mSplines[i].doneInterpolation() == false) {
			X = false;
		}
	}
	return X;
}