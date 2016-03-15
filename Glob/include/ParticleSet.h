#pragma once

#include <glm/glm.hpp>
#include <vector>

class ParticleSet
{
public:
	ParticleSet(void);
	~ParticleSet(void);

	virtual unsigned int ParticleCount() const = 0;
	virtual glm::vec3 GetParticlePosition(int i) const = 0;
	virtual void SetParticlePosition( int i, const glm::vec3 & vPosition) = 0;

	void InitializeParticles();
	void UpdateParticles();

	unsigned int N() const { return (unsigned int)m_X.size(); }

	const glm::vec3 & X(int i) const { return m_X[i]; }
	const glm::vec3 & V(int i) const { return m_V[i]; }
	const glm::vec3 & A(int i) const { return m_A[i]; }
	const glm::vec3 & F(int i) const { return m_F[i]; }
	float M(int i) const { return m_M[i]; }

	glm::vec3 & X(int i) { return m_X[i]; }
	glm::vec3 & V(int i) { return m_V[i]; }
	glm::vec3 & A(int i) { return m_A[i]; }
	glm::vec3 & F(int i) { return m_F[i]; }
	float & M(int i) { return m_M[i]; }

	void DrawParticles();

protected:
	std::vector<glm::vec3> m_X;		// position
	std::vector<glm::vec3> m_V;		// velocity
	std::vector<glm::vec3> m_A;		// acceleration
	std::vector<glm::vec3> m_F;		// force
	std::vector<float> m_M;				// mass
};



class Simulator
{
public:
	Simulator();
	~Simulator();

	void SetParticles(ParticleSet * pParticles);
	virtual void InitializeSimulator() = 0;

	void StepSimulation(float dt);
	virtual void ComputeForces() = 0;

protected:
	ParticleSet * m_pParticles;
};



class OriginSpringSimulator : public Simulator
{
public:
	OriginSpringSimulator(float fK = 1.0f, float fB = 0.1f);

	virtual void InitializeSimulator();
	virtual void ComputeForces();

protected:
	std::vector<glm::vec3> m_vRestPos;
	float m_fK;		// spring stiffness
	float m_fB;		// damping constant
};