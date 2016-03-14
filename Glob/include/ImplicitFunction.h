#pragma once

class ImplicitFunction
{
public:
	ImplicitFunction(float T = 0.5f);
	~ImplicitFunction(void);

	virtual float Value(float x, float y, float z) const = 0;
	virtual void Gradient(float x, float y, float z, float & gx, float & gy, float & gz) const = 0;

	virtual void GetSeedPoints(float * pBuf, int & nBufIndex, int nBufSize) const = 0;

	float ValueT(float x, float y, float z) const {
		float fV = Value(x,y,z);
		return fV - m_T;
	}

	float m_T;
};
