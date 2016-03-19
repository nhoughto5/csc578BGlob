#include "ParticleSet.h"
#include <iostream>
//#include <GL/freeglut.h>

ParticleSet::ParticleSet(void)
{
}

ParticleSet::~ParticleSet(void)
{
}


void ParticleSet::InitializeParticles(float liveTime)
{
	unsigned int nCount = ParticleCount();
	m_X.resize(nCount);
	m_V.resize(nCount);
	m_A.resize(nCount);
	m_F.resize(nCount);
	m_M.resize(nCount);
	timeToLive.resize(nCount);

	for ( unsigned int k = 0; k < nCount; ++k ) {
		m_X[k] = GetParticlePosition(k);
		//m_V[k] = glm::vec3(0, 0, 0);
		m_V[k] = newFountainVelocity();
		m_A[k] = glm::vec3(0,0,0);
		m_F[k] = glm::vec3(0,0,0);
		m_M[k] = 1.0f;
		timeToLive[k] = liveTime;
		timeToLive[k] = (0.25f* liveTime) + (float)(rand()) / ((float)(RAND_MAX / (liveTime - (0.25f* liveTime))));
	}
}

void ParticleSet::UpdateParticles()
{
	unsigned int nCount = ParticleCount();
	for ( unsigned int k = 0; k < nCount; ++k )
		SetParticlePosition(k, X(k) );
}

Simulator::Simulator()
{
}

Simulator::~Simulator()
{
}

void Simulator::SetParticles(ParticleSet * pParticles, float liveTime)
{
	m_pParticles = pParticles;
	m_pParticles->InitializeParticles(liveTime);
	InitializeSimulator();
}


void Simulator::StepSimulation(float dt)
{
	ComputeForces(dt);

	unsigned int N = m_pParticles->N();

	for ( unsigned int i = 0; i < N; ++i )
		m_pParticles->X(i) = m_pParticles->X(i) + m_pParticles->V(i)*dt;

	for ( unsigned int i = 0; i < N; ++i ) {
		m_pParticles->A(i) = m_pParticles->F(i) / m_pParticles->M(i);
		m_pParticles->V(i) = m_pParticles->V(i) + m_pParticles->A(i)*dt;
	}

	//for (unsigned int i = 0; i < N; ++i) {
	//	m_pParticles->X(i) = m_pParticles->X(i) + m_pParticles->V(i)*dt;
	//	m_pParticles->V(i) = m_pParticles->V(i) + m_pParticles->A(i)*dt;
	//}

}

OriginSpringSimulator::OriginSpringSimulator(float fK, float fB, float radius_, GLuint planeSize_, float bottom_, glm::vec3 fountainHead_, float timeToLive_)
{
	m_fK = fK; //The amount of energy lost from blob movement in space
	m_fB = fB; //Amount of energy lost from contact with another blob
	radius = radius_;
	planeSize = planeSize_;
	bottom = bottom_;
	fountainHead = fountainHead_;
	timeToLive = timeToLive_;
}

void OriginSpringSimulator::InitializeSimulator()
{
	//Set the rest position between all blobs
	unsigned int N = m_pParticles->N();
	m_vRestPos.resize(N);
	for ( unsigned int i = 0; i < N; ++i ) {
		/*m_vRestPos[i] = 0.5f * m_pParticles->X(i);*/
		m_vRestPos[i].x = 0.5f * m_pParticles->X(i).x;
		m_vRestPos[i].y = m_pParticles->X(i).y;
		m_vRestPos[i].z = 0.5f * m_pParticles->X(i).z;
	}
}
void OriginSpringSimulator::resetParticle(int i) {
	glm::vec3 tempVel = m_pParticles->newFountainVelocity();
	m_pParticles->X(i).x = fountainHead.x;
	m_pParticles->X(i).y = fountainHead.y;
	m_pParticles->X(i).z = fountainHead.z;
	m_pParticles->V(i).x = tempVel.x;
	m_pParticles->V(i).y = tempVel.y;
	m_pParticles->V(i).z = tempVel.z;
	m_pParticles->time(i) = (0.25f* timeToLive) + (float)(rand()) / ((float)(RAND_MAX / (timeToLive - (0.25f* timeToLive))));;
}
void OriginSpringSimulator::ComputeForces(float dt)
{
	glm::vec3 gravity{ 0.0f, -2.8f, 0.0f }, ground{ 0.0f, 0.0f, 0.0f };
	float restitution = 0.8f;
	float damping = 0.50f;
	unsigned int N = m_pParticles->N(); //Number of Particles
	for ( unsigned int i = 0; i < N; ++i ) {
		glm::vec3 vDelta = (m_pParticles->X(i) - m_vRestPos[i]); //Distance from rest position
		//m_pParticles->F(i) = -(m_fK*vDelta) - (m_fB*m_pParticles->V(i)); //Attraction
		//m_pParticles->F(i) = -(m_fK*vDelta) - (m_fB*m_pParticles->V(i)) + m_pParticles->M(i) * gravity; //Falling + Attraction
		m_pParticles->F(i) =   m_pParticles->M(i) * gravity - m_pParticles->V(i) * damping; //Falling
		m_pParticles->A(i) = m_pParticles->F(i) / m_pParticles->M(i);
		glm::vec3 pos = m_pParticles->GetParticlePosition(i);
		float distance = pos.y - ground.y;

		//Hit Ground
		float closeToGround = 0.515f;
		//std::cout << "PlaneSize: " << planeSize << "  pos:" << pos.x << ", " << pos.y << ", " << pos.z << "\n";
		if (distance <= closeToGround && ((pos.x <= (float)planeSize)&&(pos.z <= (float)planeSize))) {
			m_pParticles->V(i).y = 0.0f;
		}
		m_pParticles->time(i) -= dt;
		if (m_pParticles->X(i).y <= bottom || m_pParticles->time(i) <= 0.0f) {
			resetParticle(i);
		}
		
	}
}
