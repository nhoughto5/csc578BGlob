#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <atlas\utils\Geometry.hpp>
class ParticleSet
{
public:
	ParticleSet(void);
	~ParticleSet(void);

	virtual unsigned int ParticleCount() const = 0;
	virtual glm::vec3 GetParticlePosition(int i) const = 0;
	virtual glm::vec3 GetStartVelocity(int i) const = 0;
	virtual void SetParticlePosition( int i, const glm::vec3 & vPosition) = 0;
	virtual glm::vec3 newFountainVelocity() = 0;
	void InitializeParticles(float liveTime);
	void UpdateParticles();

	unsigned int N() const { return (unsigned int)m_X.size(); }

	const glm::vec3 & X(int i) const { return m_X[i]; }
	const glm::vec3 & V(int i) const { return m_V[i]; }
	const glm::vec3 & A(int i) const { return m_A[i]; }
	const glm::vec3 & F(int i) const { return m_F[i]; }
	float M(int i) const { return m_M[i]; }
	float time(int i) const { return timeToLive[i]; }
	//const bool & contactedNozzle(int i) const { return contactNozzle[i]; }
	glm::vec3 & X(int i) { return m_X[i]; }
	glm::vec3 & V(int i) { return m_V[i]; }
	glm::vec3 & A(int i) { return m_A[i]; }
	glm::vec3 & F(int i) { return m_F[i]; }
	float & M(int i) { return m_M[i]; }
	float & time(int i) { return timeToLive[i]; }
	//bool & contactedNozzle(int i) { return contactNozzle[i]; }
	std::vector<bool> contactNozzle;
	std::vector<bool> contactSecondTeir;
	std::vector<bool> contactedBase;

protected:
	std::vector<glm::vec3> m_X;		// position
	std::vector<glm::vec3> m_V;		// velocity
	std::vector<glm::vec3> m_A;		// acceleration
	std::vector<glm::vec3> m_F;		// force
	std::vector<float> m_M;			// mass
	std::vector<float> timeToLive;
	
};



class Simulator
{
public:
	Simulator();
	~Simulator();

	void SetParticles(ParticleSet * pParticles, float liveTime);
	virtual void InitializeSimulator() = 0;
	void StepSimulation(float dt);
	virtual void ComputeForces(float dt) = 0;
	virtual glm::vec3 interGlobularForce(int i) = 0;
	virtual void resetParticle(int i) = 0;
	virtual void resetAllParticles() = 0;
	void setForReset();
protected:
	ParticleSet * m_pParticles;
	bool isReset;
};



class OriginSpringSimulator : public Simulator
{
public:
	OriginSpringSimulator(float fK = 1.0f, float fB = 0.1f, float radius = 0.5f, GLuint planeSize_ = 20, float bottom_ = -5.0f, glm::vec3 fountainHead_ = glm::vec3{0.0f, 1.7f, 0.0f}, float timeToLive_ = 5.0f);

	virtual void InitializeSimulator();
	virtual void ComputeForces(float dt);
	virtual glm::vec3 interGlobularForce(int i);
	virtual void resetParticle(int i);
	virtual void resetAllParticles();
protected:
	std::vector<glm::vec3> m_vRestPos;
	float m_fK;		// spring stiffness
	float m_fB;		// damping constant
	float radius;
	GLuint planeSize;
	float bottom;
	glm::vec3 fountainHead;
	float timeToLive;
	
};