#include "ParticleSet.h"

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

	for ( unsigned int i = 0; i < N; ++i )
		m_pParticles->X(i) = m_pParticles->X(i) + m_pParticles->V(i)*dt;

	for ( unsigned int i = 0; i < N; ++i ) {
		m_pParticles->A(i) = m_pParticles->F(i) / m_pParticles->M(i);
		m_pParticles->V(i) = m_pParticles->V(i) + m_pParticles->A(i)*dt;
	}

}




OriginSpringSimulator::OriginSpringSimulator(float fK, float fB)
{
	m_fK = fK;
	m_fB = fB;
}

void OriginSpringSimulator::InitializeSimulator()
{
	unsigned int N = m_pParticles->N();
	m_vRestPos.resize(N);
	for ( unsigned int i = 0; i < N; ++i ) {
		m_vRestPos[i] = 0.5f * m_pParticles->X(i);
	}
}


void OriginSpringSimulator::ComputeForces()
{
	unsigned int N = m_pParticles->N();
	for ( unsigned int i = 0; i < N; ++i ) {
		glm::vec3 vDelta = (m_pParticles->X(i) - m_vRestPos[i]);
		m_pParticles->F(i) = - (m_fK*vDelta) - (m_fB*m_pParticles->V(i));
	}
}
