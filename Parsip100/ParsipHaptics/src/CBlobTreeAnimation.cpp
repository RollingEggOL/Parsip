#include "GL/glew.h"
#include "CBlobTreeAnimation.h"
#include "CEaseInEaseOut.h"



namespace PS{

namespace BLOBTREEANIMATION{

CAnimManager* CAnimManager::sm_pAnimManager = NULL;

void CAnimObject::gotoStart()
{
	if(!path->isValid()) return;

	vec3f displace = path->position(0.0f) - model->getOctree().center();
	if(model->getLock().acquire())
	{
		model->getTransform().addTranslate(displace);
		model->getLock().release();
	}
}

void CAnimObject::gotoEnd()
{
	if(!path->isValid()) return;

	vec3f displace = path->position(1.0f) - model->getOctree().center();
	if(model->getLock().acquire())
	{
		model->getTransform().addTranslate(displace);
		model->getLock().release();
	}

}

void CAnimObject::advance(float animTime)
{
	float s = CEaseInEaseOut::sineEase(animTime);
	float t = path->parameterViaTable(s);

	vec3f displace = path->position(t) - model->getOctree().center();

	//Frenet Frame to realign object with path
	vec3f pathTangent = path->tangent(t);
	vec3f prevAxis;
	float prevAngle;
	quat modelQuat = model->getTransform().getRotation();
	modelQuat.getAxisAngle(prevAxis, prevAngle);

	vec3f rotAxis = prevAxis.cross(pathTangent);
	rotAxis.normalize();
	float rotAngle = prevAxis.getAngleRad(pathTangent);
	
	if((rotAxis.length() > EPSILON)&&(Absolutef(rotAngle) > EPSILON))
	{
		quat q;
		q.fromAngleAxis(rotAngle, rotAxis);
		//modelQuat *= q;

		if(model->getLock().acquire())
		{
			model->getTransform().addRotate(q);
			model->getLock().release();
		}
	}

	
	if(bTranslate && (path->getArcTable().size() > 0))
	{
		if(model->getLock().acquire())
		{
			model->getTransform().addTranslate(displace);
			model->getLock().release();
		}
	}

	if(bScale)
	{
		vec3f s1 = startVal.xyz();
		vec3f s2 = endVal.xyz();
		vec3f val;
		val.lerp(s1, s2, t);
		if((!val.isZero()) && model->getLock().acquire())
		{
			model->getTransform().setScale(val);
			model->getLock().release();
		}
	}
}

void CAnimObject::drawPathCtrlPoints()
{
	DVec<vec3f> vCtrlPoints = path->getControlPoints();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPointSize(5.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);
	for (size_t i=0; i<vCtrlPoints.size(); i++)
	{			
		if(i != idxSelCtrlPoint)
			glVertex3fv(vCtrlPoints[i].ptr());
	}
	glEnd();

	if(vCtrlPoints.isItemIndex(idxSelCtrlPoint))	
	{
		glPointSize(7.0f);
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POINTS);
			glVertex3fv(vCtrlPoints[idxSelCtrlPoint].ptr());
		glEnd();
	}

	glPopAttrib();
}

void CAnimObject::drawPathCurve()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glLineWidth(1.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		path->drawCurve(GL_LINE_STRIP);
	glPopAttrib();
}


void CAnimManager::removeAll()
{
	CAnimObject* obj;
	for (size_t i=0; i<m_lstObjects.size(); i++)
	{
		obj = m_lstObjects[i];
		SAFE_DELETE(obj);
	}
	m_lstObjects.clear();
}

void CAnimManager::addModel( CBlobTree* lpModel )
{
	CAnimObject* obj = new CAnimObject(lpModel);
	m_lstObjects.push_back(obj);
}

bool CAnimManager::removeModel( CBlobTree* lpModel )
{
	for(size_t i=0; i < m_lstObjects.size(); i++)
	{
		if(m_lstObjects[i]->model == lpModel)
			return remove(i);			
	}

	return false;
}

bool CAnimManager::remove( int index )
{
	if(m_lstObjects.isItemIndex(index))
	{
		m_lstObjects.remove(index);
		return true;
	}
	return false;
}

CAnimManager* CAnimManager::GetAnimManager()
{
	if(sm_pAnimManager == NULL)
	{
		sm_pAnimManager = new CAnimManager();
	}

	return sm_pAnimManager;
}

void CAnimManager::advanceAnimation(float animTime)
{
	for (size_t i=0; i<m_lstObjects.size(); i++)
	{
		m_lstObjects[i]->advance(animTime);
	}
}

CAnimObject* CAnimManager::getObject( CBlobTree* root )
{
	if(root == NULL) return NULL;

	for(size_t i=0; i < m_lstObjects.size(); i++)
	{
		if(m_lstObjects[i]->model == root)
			return m_lstObjects[i];
	}
	return NULL;
}

CAnimObject* CAnimManager::getObject( int index )
{
	if(m_lstObjects.isItemIndex(index))
		return m_lstObjects[index];
	else
		return NULL;
}

int CAnimManager::queryHitPathOctree(const PS::MATH::CRay& ray, float t0, float t1)
{
	COctree oct;
	for(size_t i=0; i<m_lstObjects.size(); i++)
	{
		oct = m_lstObjects[i]->path->getOctree();
		if(oct.isValid())
		{
			if(oct.intersect(ray, t0, t1))
			{
				return i;
			}
		}
	}
	return -1;
}

bool CAnimManager::queryHitPathCtrlPoint( const PS::MATH::CRay& ray, float t0, float t1, int& idxPath, int& idxCtrlPoint )
{
	idxPath = queryHitPathOctree(ray, t0, t1);
	if(idxPath >= 0)
	{
		COctree oct;
		DVec<vec3f> vCtrlPoints;
		vCtrlPoints = m_lstObjects[idxPath]->path->getControlPoints();
		vec3f delta(m_selRadius, m_selRadius, m_selRadius);

		for(size_t i=0; i<vCtrlPoints.size(); i++)
		{
			vec3f c = vCtrlPoints[i];
			oct.set(c - delta, c + delta);
			if(oct.intersect(ray, t0, t1))
			{
				idxCtrlPoint = i;
				return true;
			}
		}
	}

	return false;
}

void CAnimManager::queryHitResetAll()
{
	for(size_t i=0; i<m_lstObjects.size(); i++)
	{
		m_lstObjects[i]->idxSelCtrlPoint = -1;
	}
	m_idxSelObject = -1;
}

void CAnimManager::setActiveObject( int index )
{
	if(m_lstObjects.isItemIndex(index))
		m_idxSelObject = index;
	else
		m_idxSelObject = -1;
}

CAnimObject* CAnimManager::getActiveObject()
{
	if(m_lstObjects.isItemIndex(m_idxSelObject))
		return m_lstObjects[m_idxSelObject];
	else
		return NULL;
}

bool CAnimManager::hasSelectedCtrlPoint()
{
	CAnimObject* obj = getActiveObject();
	if(obj)
		return obj->path->vCtrlPoints.isItemIndex(obj->idxSelCtrlPoint);
	return false;
}

}
}