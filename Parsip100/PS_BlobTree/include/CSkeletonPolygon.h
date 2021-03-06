#pragma once
#ifndef CSKELETONPOLYGON_H
#define CSKELETONPOLYGON_H

#include "CSkeleton.h"
#include <vector>

namespace PS{
namespace BLOBTREE{

using namespace std;

//Implementation of Skeleton types like point, line, disk, circle, cylinder, hollowCylinder
class  CSkeletonPolygon: public CSkeleton
{
private:
    std::vector<vec3f> m_lstPoints;
public:
    CSkeletonPolygon() { }
    CSkeletonPolygon(vector<vec3f>& lstPoints)
    {
        setPolygon(lstPoints);
    }

    ~CSkeletonPolygon()
    {
        m_lstPoints.resize(0);
    }

    void setParamFrom(CSkeleton* input)
    {
        CSkeletonPolygon* rhs = dynamic_cast<CSkeletonPolygon*>(input);
        m_lstPoints.assign(rhs->m_lstPoints.begin(), rhs->m_lstPoints.end());
    }

    void setPolygon(vector<vec3f>& lstPoints)
    {
        m_lstPoints.assign(lstPoints.begin(), lstPoints.end());
    }

    void addPoint(vec3f v)
    {
        m_lstPoints.push_back(v);
    }

    size_t countPoints() const { return m_lstPoints.size();}

    void clearAll()
    {
        m_lstPoints.resize(0);
    }

    float distance(vec3f p)
    {
        if(m_lstPoints.size() > 2)
        {
            vec3f inPlane = NearestPointInPlane(p, m_lstPoints[0], m_lstPoints[1], m_lstPoints[2]);
            return inPlane.distance(p);
        }
        else
            return 0.0f;
    }

    float squareDistance(vec3f p)
    {
        float dist = distance(p);
        return dist*dist;
    }

    vec3f normal(vec3f p)
    {
        vec3f inPlane = NearestPointInPlane(p, m_lstPoints[0], m_lstPoints[1], m_lstPoints[2]);
        vec3f n = p - inPlane;
        n.normalize();
        return n;
    }

    float getDistanceAndNormal(vec3f p, vec3f& normal)
    {
        vec3f inPlane = NearestPointInPlane(p, m_lstPoints[0], m_lstPoints[1], m_lstPoints[2]);
        normal = p - inPlane;
        normal.normalize();
        return inPlane.distance(p);
    }

    string getName()
    {
        return "POLYGON";
    }

    bool getExtremes(vec3f& lower, vec3f& upper)
    {
        if(m_lstPoints.size() == 0) return false;

        vec3f minPoint = m_lstPoints[0];
        vec3f maxPoint = m_lstPoints[0];
        vec3f p;
        for(size_t i=1; i < m_lstPoints.size(); i++)
        {
            p = m_lstPoints[i];
            if(p.x > maxPoint.x) maxPoint.x = p.x;
            if(p.y > maxPoint.y) maxPoint.y = p.y;
            if(p.z > maxPoint.z) maxPoint.z = p.z;
            if(p.x < minPoint.x) minPoint.x = p.x;
            if(p.y < minPoint.y) minPoint.y = p.y;
            if(p.z < minPoint.z) minPoint.z = p.z;
        }

        lower = minPoint;
        upper = maxPoint;
        return true;
    }

    VOL::CVolume* getBoundingVolume(float range)
    {
        if(m_lstPoints.size() == 0) return NULL;

        vec3f minPoint = m_lstPoints[0];
        vec3f maxPoint = m_lstPoints[0];
        vec3f p;
        for(size_t i=1; i < m_lstPoints.size(); i++)
        {
            p = m_lstPoints[i];
            if(p.x > maxPoint.x) maxPoint.x = p.x;
            if(p.y > maxPoint.y) maxPoint.y = p.y;
            if(p.z > maxPoint.z) maxPoint.z = p.z;
            if(p.x < minPoint.x) minPoint.x = p.x;
            if(p.y < minPoint.y) minPoint.y = p.y;
            if(p.z < minPoint.z) minPoint.z = p.z;
        }
        VOL::CVolumeBox * s = new VOL::CVolumeBox(minPoint, maxPoint);
        s->scaleSelf(vec3f(range, range, range));
        return s;
    }

    vec3f getPolySeedPoint()
    {
        if(m_lstPoints.size() > 0)
            return m_lstPoints[0];
        else
            return vec3f(0.0f, 0.0f, 0.0f);
    }

    void translate(vec3f d)
    {
        for(size_t i=0; i < m_lstPoints.size(); i++)
            m_lstPoints[i] += d;
    }

    BlobNodeType getType()		{return bntPrimPolygon;}

    bool saveScript(CSketchConfig *lpSketchScript, int id)
    {
        return false;

    }

    bool loadScript(CSketchConfig *lpSketchScript, int id)
    {
        return false;

    }



};

}
}
#endif
