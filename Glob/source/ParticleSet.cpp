#include "ParticleSet.h"
#include <iostream>
//#include <GL/freeglut.h>

ParticleSet::ParticleSet(void)
{
}

ParticleSet::~ParticleSet(void)
{
}


void ParticleSet::InitializeParticles()
{
	unsigned int nCount = ParticleCount();
	m_X.resize(nCount);
	m_V.resize(nCount);
	m_A.resize(nCount);
	m_F.resize(nCount);
	m_M.resize(nCount);

	for ( unsigned int k = 0; k < nCount; ++k ) {
		m_X[k] = GetParticlePosition(k);
		m_V[k] = glm::vec3(0,0,0);
		m_A[k] = glm::vec3(0,0,0);
		m_F[k] = glm::vec3(0,0,0);
		m_M[k] = 1.0f;
	}
}

void ParticleSet::UpdateParticles()
{
	unsigned int nCount = ParticleCount();
	for ( unsigned int k = 0; k < nCount; ++k )
		SetParticlePosition(k, X(k) );
}


//void ParticleSet::DrawParticles()
//{
//	glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT | GL_LINE_BIT);
//
//	unsigned int nCount = ParticleCount();
//
//	glPointSize(5.0f);
//	glColor3f(0,0,1);
//	glBegin(GL_POINTS);
//	for ( unsigned int k = 0; k < nCount; ++k ) {
//		glVertex3f( X(k).x, X(k).y, X(k).z);
//	}
//	glEnd();
//
//	glLineWidth(2.0f);
//	glColor3f(1,0,0);
//	glBegin(GL_LINES);
//	for ( unsigned int k = 0; k < nCount; ++k ) {
//		glVertex3f(X(k).x, X(k).y, X(k).z);
//		glVertex3f( X(k).x + V(k).x, X(k).y + V(k).y, X(k).z + V(k).z);
//	}
//	glEnd();
//
//	glPopAttrib();
//}



Simulator::Simulator()
{
}

Simulator::~Simulator()
{
}

void Simulator::SetParticles(ParticleSet * pParticles)
{
	m_pParticles = pParticles;
	m_pParticles->InitializeParticles();
	InitializeSimulator();
}


void Simulator::StepSimulation(float dt)
{
	ComputeForces();

	unsigned int N = m_pParticles->N();

	//for ( unsigned int i = 0; i < N; ++i )
	//	m_pParticles->X(i) = m_pParticles->X(i) + m_pParticles->V(i)*dt;

	//for ( unsigned int i = 0; i < N; ++i ) {
	//	m_pParticles->A(i) = m_pParticles->F(i) / m_pParticles->M(i);
	//	m_pParticles->V(i) = m_pParticles->V(i) + m_pParticles->A(i)*dt;
	//}

	for (unsigned int i = 0; i < N; ++i) {
		m_pParticles->X(i) = m_pParticles->X(i) + m_pParticles->V(i)*dt;
		m_pParticles->V(i) = m_pParticles->V(i) + m_pParticles->A(i)*dt;
	}

}

OriginSpringSimulator::OriginSpringSimulator(float fK, float fB, float radius_)
{
	m_fK = fK; //The amount of energy lost from blob movement in space
	m_fB = fB; //Amount of energy lost from contact with another blob
	radius = radius_;
}

void OriginSpringSimulator::InitializeSimulator()
{
	//Set the rest position between all blobs
	unsigned int N = m_pParticles->N();
	m_vRestPos.resize(N);
	for ( unsigned int i = 0; i < N; ++i ) {
		m_vRestPos[i] = 0.5f * m_pParticles->X(i);
	}
}
void OriginSpringSimulator::ComputeForces()
{
	glm::vec3 gravity{ 0.0f, -2.8f, 0.0f }, ground{ 0.0f, 0.0f, 0.0f };
	float damping = 2.0f;
	unsigned int N = m_pParticles->N(); //Number of Particles
	for ( unsigned int i = 0; i < N; ++i ) {
		glm::vec3 vDelta = (m_pParticles->X(i) - m_vRestPos[i]); //Distance from rest position
		//m_pParticles->F(i) = -(m_fK*vDelta) - (m_fB*m_pParticles->V(i)) + m_pParticles->M(i) * gravity; //Falling
		m_pParticles->F(i) =   m_pParticles->M(i) * gravity - m_pParticles->V(i) * damping; //Falling
		glm::vec3 tempAccel = m_pParticles->A(i) = m_pParticles->F(i) / m_pParticles->M(i);
		glm::vec3 temp = m_pParticles->GetParticlePosition(i);
		float distance = glm::abs(temp.y - ground.y);
		
		if (distance < 4.0f) {
			int s = 4;
		}
		if ( distance <= radius) {
			//Stop Condition
			if (glm::abs(tempAccel.y) < 0.000005f) {
				m_pParticles->V(i) = glm::vec3{0.0f, 0.0f, 0.0f}; 
				m_pParticles->A(i) = glm::vec3{ 0.0f, 0.0f, 0.0f };
				m_pParticles->F(i) = glm::vec3{ 0.0f, 0.0f, 0.0f };
				std::cout << "Less Vel: " << tempAccel.x << ", " << tempAccel.y << ", " << tempAccel.z << "    Distance: " << distance << "\n";
			}
			//Bounce
			else {
				glm::vec3 newVel = -0.8f * m_pParticles->V(i);
				m_pParticles->V(i) = newVel;
				std::cout << "More Vel: " << m_pParticles->A(i).x << ", " << m_pParticles->A(i).y << ", " << m_pParticles->A(i).z << "    Distance: " << distance << "\n";
			}
		}
	}
}
