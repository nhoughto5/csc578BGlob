#include "BlobSet.h"



float Blob::Value(float x, float y, float z) const
{
	// [TODO] can get rid of sqrt here by dividing by radius^2

	float dx = (x-m_vPosition[0]);
	float dy = (y-m_vPosition[1]);
	float dz = (z-m_vPosition[2]);
	float fDist = sqrt( dx*dx + dy*dy + dz*dz );
	float fR = fDist / m_fRadius;

	float fD = 1-fR*fR;
	if ( fD > 0 )
		return fD*fD*fD;
	else
		return 0;
}


void Blob::Gradient(float x, float y, float z, float & gx, float & gy, float & gz) const
{
	float dx = (x-m_vPosition[0]);
	float dy = (y-m_vPosition[1]);
	float dz = (z-m_vPosition[2]);
	float fDist = sqrt( dx*dx + dy*dy + dz*dz );
	float fR = fDist / m_fRadius;

	float fD = 1-fR*fR;
	if ( fD > 0 ) {
		float fV = -6 * (fD*fD) / (m_fRadius*m_fRadius);
		gx = fV*dx;
		gy = fV*dy;
		gz = fV*dz;
	} else {
		gx = gy = gz = 0;
	}
}


void Blob::GetSeedPoints(float * pBuf, int & nBufIndex, int nBufSize) const
{
	for ( int k = 0; k < 3 && nBufIndex < nBufSize; ++k )
		pBuf[nBufIndex++] = m_vPosition[k];
}



BlobSet::BlobSet(void)
{
}

BlobSet::~BlobSet(void)
{
}


float BlobSet::Value(float x, float y, float z) const
{
	float fSum = 0;
	for ( unsigned int k = 0; k < m_vBlobs.size(); ++k )
		fSum += m_vBlobs[k].Value(x,y,z);
	return fSum;
}


void BlobSet::Gradient(float x, float y, float z, float & gx, float & gy, float & gz) const
{
	gx = 0; gy = 0; gz = 0;
	for ( unsigned int k = 0; k < m_vBlobs.size(); ++k ) {
		float bgx, bgy, bgz;
		m_vBlobs[k].Gradient(x,y,z, bgx,bgy,bgz);
		gx += bgx;
		gy += bgy;
		gz += bgz;
	}
}

void BlobSet::GetSeedPoints(float * pBuf, int & nBufIndex, int nBufSize) const
{
	for ( unsigned int k = 0; k < m_vBlobs.size(); ++k )
		m_vBlobs[k].GetSeedPoints(pBuf, nBufIndex, nBufSize);
}
