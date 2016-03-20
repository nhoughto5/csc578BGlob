#include "ParticleSet.h"
#include <iostream>
#include <glm\glm.hpp>
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
	contactNozzle.resize(nCount);
	contactSecondTeir.resize(nCount);

	for ( unsigned int k = 0; k < nCount; ++k ) {
		m_X[k] = GetParticlePosition(k);
		//m_V[k] = glm::vec3(0, 0, 0);
		m_V[k] = newFountainVelocity();
		m_A[k] = glm::vec3(0,0,0);
		m_F[k] = glm::vec3(0,0,0);
		m_M[k] = 1.0f;
		timeToLive[k] = liveTime;
		timeToLive[k] = (0.25f* liveTime) + (float)(rand()) / ((float)(RAND_MAX / (liveTime - (0.25f* liveTime))));
		contactNozzle[k] = false;
		contactSecondTeir[k] = false;
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
	m_pParticles->F(i).x = 0.0f;
	m_pParticles->F(i).y = 0.0f;
	m_pParticles->F(i).z = 0.0f;
	m_pParticles->time(i) = (0.25f* timeToLive) + (float)(rand()) / ((float)(RAND_MAX / (timeToLive - (0.25f* timeToLive))));
	m_pParticles->contactNozzle[i] = false;
	m_pParticles->contactSecondTeir[i] = false;
}
glm::vec3 OriginSpringSimulator::interGlobularForce(int current) {
	glm::vec3 force{ 0.0f, 0.0f, 0.0f };
	glm::vec3 currentPosition = m_pParticles->GetParticlePosition(current);
	glm::vec3 currentVelocity = m_pParticles->V(current);
	glm::vec3 P, V;
	int m = 5, n = 3;
	float Cr, Cd, b1 = 0.00000010f, b2;
	float rMax = radius;
	float ri, rp, r0 = 0.8 * radius;
	Cr = r0; Cd = r0;
	ri = rp = r0;
	float D, dM, dN;
	float Sr, Sd;
	for (int i = 0; i < m_pParticles->ParticleCount(); ++i) {
		if (i != current) {
			D = glm::distance(currentPosition, m_pParticles->GetParticlePosition(i));
			if (D >= 0.01) {
				P = currentPosition - m_pParticles->GetParticlePosition(i);
				V = m_pParticles->V(i);
				if ((D*D) < (Cr*Cr*(ri + rp)*(ri + rp))) {
					Sr = 1.0f - (D * D) / ((Cr)*(Cr)*(ri + rp)*(ri + rp));
					Sr = 200.0f * Sr;
				}
				else Sr = 0.0f;
				if ((D*D) < (Cd*Cd*(ri + rp)*(ri + rp))) {
					Sd = 1.0f - (D * D) / ((Cd)*(Cd)*(ri + rp)*(ri + rp));
				}
				else Sd = 0.0f;
				b2 = b1 * glm::pow(r0, (n - m));
				dM = (Sr * ((b1 / glm::pow(D, m)) - (b2 / glm::pow(D, n))) - Sd*((glm::dot(V, P)) / (D*D)));
				force += P * dM;
			}
		}
	}
	return force;
}
//int count;
void OriginSpringSimulator::ComputeForces(float dt)
{
	//std::cout << "    ComputeForces: " << count++ << "\n";
	glm::vec3 gravity{ 0.0f, -0.5, 0.0f }, ground{ 0.0f, 0.0f, 0.0f };
	float restitution = 0.4f;
	float damping = 0.50f;
	unsigned int N = m_pParticles->N(); //Number of Particles
	glm::vec3 interForce;
	std::cout << "Globular Force ";
	for ( unsigned int i = 0; i < N; ++i ) {
		//m_pParticles->F(i) =   m_pParticles->M(i) * gravity - m_pParticles->V(i) * damping; //Falling
		interForce = interGlobularForce(i);
		std::cout << i << ": " << interForce.x << ", " << interForce.y << ", " << interForce.z << "  ";
		m_pParticles->F(i) = interForce + (m_pParticles->M(i) * gravity) - (m_pParticles->V(i) * damping);
		m_pParticles->A(i) = m_pParticles->F(i) / m_pParticles->M(i);

		//Hit Ground
		float closeToGround = 0.515f;
		if ((m_pParticles->GetParticlePosition(i).y - ground.y) <= closeToGround && ((m_pParticles->GetParticlePosition(i).x <= (float)planeSize - 1.0f)&&(m_pParticles->GetParticlePosition(i).z <= (float)planeSize -1.0f))) {
			m_pParticles->V(i).y = 0.0f;
		}
		m_pParticles->time(i) -= dt;
		if (m_pParticles->X(i).y <= bottom || m_pParticles->time(i) <= 0.0f || m_pParticles->X(i).y > 12.0f) {
			resetParticle(i);
		}
		float upperLimit = 0.5f, lowerLimit = 0.3f;
		//Contacts Fountain Nozzle
		float fountainNozzleWidth = 0.1f;
		if ((m_pParticles->contactNozzle[i] == false) && (m_pParticles->X(i).y <= 0.99f * fountainHead.y) && ((m_pParticles->X(i).x < fountainNozzleWidth) && (m_pParticles->X(i).z < fountainNozzleWidth))) {
			m_pParticles->contactNozzle[i] = true;
			m_pParticles->V(i).y = - restitution * m_pParticles->V(i).y;
			m_pParticles->V(i).x = (-upperLimit) + (float)(rand()) / ((float)(RAND_MAX / (upperLimit - (-upperLimit))));
			m_pParticles->V(i).z = (-upperLimit) + (float)(rand()) / ((float)(RAND_MAX / (upperLimit - (-upperLimit))));
		}

		//Contacts Second Teir
		float secondTeirWidth = 13.0f;
		upperLimit = 1.5f; lowerLimit = 0.3f;
		if ((m_pParticles->contactSecondTeir[i] == false) && (m_pParticles->X(i).y <= 0.55f * fountainHead.y) && (((m_pParticles->X(i).x < secondTeirWidth) && (m_pParticles->X(i).z < secondTeirWidth)))) {
			m_pParticles->contactSecondTeir[i] = true;
			m_pParticles->V(i).y = - restitution * m_pParticles->V(i).y;
		/*	m_pParticles->V(i).y = 0.0f;*/
			m_pParticles->V(i).x = (m_pParticles->V(i).x / glm::abs(m_pParticles->V(i).x))*((-upperLimit) + (float)(rand()) / ((float)(RAND_MAX / (upperLimit - (-upperLimit)))));
			m_pParticles->V(i).z = (m_pParticles->V(i).z / glm::abs(m_pParticles->V(i).z))*((-upperLimit) + (float)(rand()) / ((float)(RAND_MAX / (upperLimit - (-upperLimit)))));
		}
		
	}
	std::cout << "\n";
}
