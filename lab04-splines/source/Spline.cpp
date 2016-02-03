#include "Spline.hpp"

#include <atlas/core/Macros.hpp>

Spline::Spline(int totalFrames) :
    mResolution(500),
    mTotalFrames(totalFrames),
    mCurrentFrame(0),
    mShowControlPoints(false),
    mShowCage(false),
    mShowSplinePoints(false),
    mShowSpline(true),
    mIsInterpolationDone(false)
{
    USING_ATLAS_MATH_NS;
    USING_ATLAS_GL_NS;

    mBasisMatrix = Matrix4(
         1.0f,  0.0f,  0.0f, 0.0f,
        -3.0f,  3.0f,  0.0f, 0.0f,
         3.0f, -6.0f,  3.0f, 0.0f,
        -1.0f,  3.0f, -3.0f, 1.0f);

    mControlPoints = std::vector<Point>
    {
        { -20, 5, 0 },
        { -19, 5, -15 },
        { 12.7f, -5, -1.4f },
        { 20, 8.2f, 4.4f }
    };

    std::vector<Point> splinePoints;

    float scale = 1.0f / mResolution;
    for (int res = 0; res < mResolution + 1; ++res)
    {
        splinePoints.push_back(evaluateSpline(scale * res));
    }

    generateArcLengthTable();

    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    glGenBuffers(1, &mControlBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mControlBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * mControlPoints.size(),
        mControlPoints.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mSplineBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mSplineBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * splinePoints.size(),
        splinePoints.data(), GL_STATIC_DRAW);

    std::vector<ShaderInfo> shaders
    {
        { GL_VERTEX_SHADER, "shaders/spline.vs.glsl" },
        { GL_FRAGMENT_SHADER, "shaders/spline.fs.glsl" }
    };

    mShaders.push_back(ShaderPointer(new Shader));
    mShaders[0]->compileShaders(shaders);
    mShaders[0]->linkShaders();

    GLuint var;
    var = mShaders[0]->getUniformVariable("uMVP");
    mUniforms.insert(UniformKey("uMVP", var));

    var = mShaders[0]->getUniformVariable("fColour");
    mUniforms.insert(UniformKey("fColour", var));

    mShaders[0]->disableShaders();
    glBindVertexArray(0);
}

Spline::~Spline()
{
    glDeleteVertexArrays(1, &mVao);
    glDeleteVertexArrays(1, &mControlBuffer);
    glDeleteVertexArrays(1, &mSplineBuffer);
}

void Spline::renderGeometry(atlas::math::Matrix4 projection,
    atlas::math::Matrix4 view)
{
    USING_ATLAS_MATH_NS;

    mShaders[0]->enableShaders();

    glBindVertexArray(mVao);

    Matrix4 mvp = projection * view * mModel;
    glUniformMatrix4fv(mUniforms["uMVP"], 1, GL_FALSE, &mvp[0][0]);

    // Draw the control points first.
    glUniform3f(mUniforms["fColour"], 1, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mControlBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    if (mShowControlPoints)
    {
        glPointSize(5.0f);
        glDrawArrays(GL_POINTS, 0, GLsizei(mControlPoints.size()));
        glPointSize(1.0f);
    }

    if (mShowCage)
    {
        glDrawArrays(GL_LINE_STRIP, 0, GLsizei(mControlPoints.size()));
    }

    // Now draw the spline.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mSplineBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glUniform3f(mUniforms["fColour"], 0, 1, 0);

    if (mShowSpline)
    {
        glLineWidth(5.0f);
        glDrawArrays(GL_LINE_STRIP, 0, mResolution + 1);
        glLineWidth(1.0f);
    }

    if (mShowSplinePoints)
    {
        glPointSize(8.0f);
        glDrawArrays(GL_POINTS, 1, mResolution);
        glPointSize(1.0f);
    }

    glDisableVertexAttribArray(0);

    mShaders[0]->disableShaders();
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

void Spline::showControlPoints()
{
    mShowControlPoints = !mShowControlPoints;
}

void Spline::showCage()
{
    mShowCage = !mShowCage;
}

void Spline::showSplinePoints()
{
    mShowSplinePoints = !mShowSplinePoints;
}

void Spline::showSpline()
{
    mShowSpline = !mShowSpline;
}

bool Spline::doneInterpolation()
{
    return mIsInterpolationDone;
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

atlas::math::Point Spline::evaluateSpline(float t)
{
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