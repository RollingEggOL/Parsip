#pragma once
#ifndef CSKELETONTRIANGLE_H
#define CSKELETONTRIANGLE_H

#include "CSkeleton.h"

using namespace PS;

namespace PS{
namespace BLOBTREE{

//Implementation of Skeleton types like point, line, disk, circle, cylinder, hollowCylinder
class  CSkeletonTriangle: public CSkeleton
{
private:
    vec3f m_triangle[3];
public:
    CSkeletonTriangle()
    {
        m_triangle[0] = vec3f(-1.0f, 0.0f, 0.0f);
        m_triangle[1] = vec3f(1.0f, 0.0f, 0.0f);
        m_triangle[2] = vec3f(0.0f, 1.0f, 0.0f);
    }

    CSkeletonTriangle(vec3f a, vec3f b, vec3f c)
    {
        m_triangle[0] = a;
        m_triangle[1] = b;
        m_triangle[2] = c;
    }

    void setParamFrom(CSkeleton* input)
    {
        CSkeletonTriangle* triN = dynamic_cast<CSkeletonTriangle*>(input);
        this->m_triangle[0] = triN->m_triangle[0];
        this->m_triangle[1] = triN->m_triangle[1];
        this->m_triangle[2] = triN->m_triangle[2];
    }

    vec3f getTriangleCorner(int i) const
    {
        return m_triangle[i];
    }
    void setTriangleCorner(int i, vec3f p) {m_triangle[i] = p;}


    float distance(vec3f p)
    {
        vec3f closestPoint;
        vec3f baryCoords;
        return sqrt(squareDistance(p, closestPoint, baryCoords));
    }

    float squareDistance(vec3f p)
    {
        vec3f closestPoint;
        vec3f baryCoords;
        return squareDistance(p, closestPoint, baryCoords);
    }

    float squareDistance(vec3f p, vec3f& outClosestPoint, vec3f& outBaryCoords)
    {
        vec3f dif = m_triangle[0] - p;
        vec3f edge0 = m_triangle[1] - m_triangle[0];
        vec3f edge1 = m_triangle[2] - m_triangle[0];
        float a00 = edge0.length2();
        float a01 = edge0.dot(edge1);
        float a11 = edge1.length2();

        float b0 = dif.dot(edge0);
        float b1 = dif.dot(edge1);
        float c = dif.length2();
        float det = Absolutef(a00 * a11 - a01*a01);
        float s = a01*b1 - a11*b0;
        float t = a01*b0 - a00*b1;
        float sqrDistance;

        //Conditions
        if (s + t <= det)
        {
            if (s < (float)0)
            {
                if (t < (float)0)  // region 4
                {
                    if (b0 < (float)0)
                    {
                        t = (float)0;
                        if (-b0 >= a00)
                        {
                            s = (float)1;
                            sqrDistance = a00 + ((float)2)*b0 + c;
                        }
                        else
                        {
                            s = -b0/a00;
                            sqrDistance = b0*s + c;
                        }
                    }
                    else
                    {
                        s = (float)0;
                        if (b1 >= (float)0)
                        {
                            t = (float)0;
                            sqrDistance = c;
                        }
                        else if (-b1 >= a11)
                        {
                            t = (float)1;
                            sqrDistance = a11 + ((float)2)*b1 + c;
                        }
                        else
                        {
                            t = -b1/a11;
                            sqrDistance = b1*t + c;
                        }
                    }
                }
                else  // region 3
                {
                    s = (float)0;
                    if (b1 >= (float)0)
                    {
                        t = (float)0;
                        sqrDistance = c;
                    }
                    else if (-b1 >= a11)
                    {
                        t = (float)1;
                        sqrDistance = a11 + ((float)2)*b1 + c;
                    }
                    else
                    {
                        t = -b1/a11;
                        sqrDistance = b1*t + c;
                    }
                }
            }
            else if (t < (float)0)  // region 5
            {
                t = (float)0;
                if (b0 >= (float)0)
                {
                    s = (float)0;
                    sqrDistance = c;
                }
                else if (-b0 >= a00)
                {
                    s = (float)1;
                    sqrDistance = a00 + ((float)2)*b0 + c;
                }
                else
                {
                    s = -b0/a00;
                    sqrDistance = b0*s + c;
                }
            }
            else  // region 0
            {
                // minimum at interior point
                float invDet = ((float)1)/det;
                s *= invDet;
                t *= invDet;
                sqrDistance = s*(a00*s + a01*t + ((float)2)*b0) +
                        t*(a01*s + a11*t + ((float)2)*b1) + c;
            }
        }
        else
        {
            float tmp0, tmp1, numer, denom;

            if (s < (float)0)  // region 2
            {
                tmp0 = a01 + b0;
                tmp1 = a11 + b1;
                if (tmp1 > tmp0)
                {
                    numer = tmp1 - tmp0;
                    denom = a00 - ((float)2)*a01 + a11;
                    if (numer >= denom)
                    {
                        s = (float)1;
                        t = (float)0;
                        sqrDistance = a00 + ((float)2)*b0 + c;
                    }
                    else
                    {
                        s = numer/denom;
                        t = (float)1 - s;
                        sqrDistance = s*(a00*s + a01*t + ((float)2)*b0) +
                                t*(a01*s + a11*t + ((float)2)*b1) + c;
                    }
                }
                else
                {
                    s = (float)0;
                    if (tmp1 <= (float)0)
                    {
                        t = (float)1;
                        sqrDistance = a11 + ((float)2)*b1 + c;
                    }
                    else if (b1 >= (float)0)
                    {
                        t = (float)0;
                        sqrDistance = c;
                    }
                    else
                    {
                        t = -b1/a11;
                        sqrDistance = b1*t + c;
                    }
                }
            }
            else if (t < (float)0)  // region 6
            {
                tmp0 = a01 + b1;
                tmp1 = a00 + b0;
                if (tmp1 > tmp0)
                {
                    numer = tmp1 - tmp0;
                    denom = a00 - ((float)2)*a01 + a11;
                    if (numer >= denom)
                    {
                        t = (float)1;
                        s = (float)0;
                        sqrDistance = a11 + ((float)2)*b1 + c;
                    }
                    else
                    {
                        t = numer/denom;
                        s = (float)1 - t;
                        sqrDistance = s*(a00*s + a01*t + ((float)2)*b0) +
                                t*(a01*s + a11*t + ((float)2)*b1) + c;
                    }
                }
                else
                {
                    t = (float)0;
                    if (tmp1 <= (float)0)
                    {
                        s = (float)1;
                        sqrDistance = a00 + ((float)2)*b0 + c;
                    }
                    else if (b0 >= (float)0)
                    {
                        s = (float)0;
                        sqrDistance = c;
                    }
                    else
                    {
                        s = -b0/a00;
                        sqrDistance = b0*s + c;
                    }
                }
            }
            else  // region 1
            {
                numer = a11 + b1 - a01 - b0;
                if (numer <= (float)0)
                {
                    s = (float)0;
                    t = (float)1;
                    sqrDistance = a11 + ((float)2)*b1 + c;
                }
                else
                {
                    denom = a00 - ((float)2)*a01 + a11;
                    if (numer >= denom)
                    {
                        s = (float)1;
                        t = (float)0;
                        sqrDistance = a00 + ((float)2)*b0 + c;
                    }
                    else
                    {
                        s = numer/denom;
                        t = (float)1 - s;
                        sqrDistance = s*(a00*s + a01*t + ((float)2)*b0) +
                                t*(a01*s + a11*t + ((float)2)*b1) + c;
                    }
                }
            }
        }

        // Account for numerical round-off error.
        if (sqrDistance < (float)0)
        {
            sqrDistance = (float)0;
        }

        //mClosestPoint0 = *mPoint;
        outClosestPoint = m_triangle[0] + s*edge0 + t*edge1;
        outBaryCoords.y = s;
        outBaryCoords.z = t;
        outBaryCoords.x = (float)1 - s - t;
        return sqrDistance;
    }


    vec3f normal(vec3f p)
    {
        vec3f closestPoint;
        vec3f baryCoords;
        squareDistance(p, closestPoint, baryCoords);
        vec3f n = closestPoint - p;
        n.normalize();
        return n;
    }

    float getDistanceAndNormal(vec3f p, vec3f& normal)
    {
        vec3f closestPoint;
        vec3f baryCoords;
        float d = squareDistance(p, closestPoint, baryCoords);
        normal = closestPoint - p;
        normal.normalize();
        return d;

    }

    string getName()
    {
        return "TRIANGLE";
    }

    BBOX bound() const
    {
        vec3f slo = m_triangle[0].vectorMin(m_triangle[1]).vectorMin(m_triangle[2]);
        vec3f shi = m_triangle[0].vectorMax(m_triangle[1]).vectorMax(m_triangle[2]);
        vec3f offset(ISO_VALUE);
        BBOX box(slo - offset, shi + offset);
        return box;
    }

    vec3f getPolySeedPoint()
    {
        vec3f centroid = (m_triangle[0] + m_triangle[1] + m_triangle[2])*(1.0f/3.0f);
        return centroid;
    }

    void translate(vec3f d)
    {
        m_triangle[0] += d;
        m_triangle[1] += d;
        m_triangle[2] += d;
    }

    BlobNodeType getType()		{return bntPrimTriangle;}

    bool saveScript(CSketchConfig* lpSketchScript, int id)
    {
        //Write parameters for RicciBlend
        DAnsiStr strNodeName = printToAStr("BLOBNODE %d", id);
        lpSketchScript->writeVec3f(strNodeName, "corner0", this->getTriangleCorner(0));
        lpSketchScript->writeVec3f(strNodeName, "corner1", this->getTriangleCorner(1));
        lpSketchScript->writeVec3f(strNodeName, "corner2", this->getTriangleCorner(2));

        return true;
    }

    bool loadScript(CSketchConfig* lpSketchScript, int id)
    {
        //Write parameters for RicciBlend
        DAnsiStr strNodeName = printToAStr("BLOBNODE %d", id);
        m_triangle[0] = lpSketchScript->readVec3f(strNodeName, "corner0");
        m_triangle[1] = lpSketchScript->readVec3f(strNodeName, "corner1");
        m_triangle[2] = lpSketchScript->readVec3f(strNodeName, "corner2");
        return true;
    }


};

float ComputeTriangleSquareDist(vec3f vertices[3], vec3f p, vec3f& outClosestPoint, vec3f& outBaryCoords);

}
}

#endif
