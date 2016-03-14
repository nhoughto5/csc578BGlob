#pragma once
#include <vector>
#include "ImplicitFunction.h"
#include "ParticleSet.h"


class Blob : public ImplicitFunction
{
public:
	Blob() { m_vPosition = glm::vec3{0.0f, 0.0f, 0.0f}; m_fRadius = 1; }
	Blob(const glm::vec3 & pos, float fRadius )  { m_vPosition = pos; m_fRadius = fRadius; }

	/* ImplicitFunction interface */
	virtual float Value(float x, float y, float z) const;
	virtual void Gradient(float x, float y, float z, float & gx, float & gy, float & gz) const;
	virtual void GetSeedPoints(float * pBuf, int & nBufIndex, int nBufSize) const;

	glm::vec3 & Position() { return m_vPosition; }
	const glm::vec3 & Position() const { return m_vPosition; }

	glm::vec3 m_vPosition;
	float m_fRadius;
};



class BlobSet : public ImplicitFunction, public ParticleSet
{
public:
	BlobSet(void);
	~BlobSet(void);

	void AddBlob( const Blob & b ) { m_vBlobs.push_back(b); }

	/* ImplicitFunction interface */

	virtual float Value(float x, float y, float z) const;
	virtual void Gradient(float x, float y, float z, float & gx, float & gy, float & gz) const;
	virtual void GetSeedPoints(float * pBuf, int & nBufIndex, int nBufSize) const;

	/* ParticleSet interface */
	virtual unsigned int ParticleCount() const { return (unsigned int)m_vBlobs.size(); }
	virtual glm::vec3 GetParticlePosition(int i) const { return m_vBlobs[i].Position(); }
	virtual void SetParticlePosition( int i, const glm::vec3 & vPosition) { m_vBlobs[i].Position() = vPosition; }
	
protected:
	std::vector<Blob> m_vBlobs;
};