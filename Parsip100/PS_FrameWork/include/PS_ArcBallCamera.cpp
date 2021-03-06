//#include "stdafx.h"
#include "PS_ArcBallCamera.h"

namespace PS
{


CArcBallCamera::CArcBallCamera()
{
	m_omega = horizontalAngle;
	m_phi = verticalAngle;
	m_rho = zoom;
	m_origin = vec3f(0.0f, 0.0f, 0.0f);
	m_center = vec3f(0.0f, 0.0f, 0.0f);
}

//Constructor with valid values
CArcBallCamera::CArcBallCamera(float o, float p, float r)
{
	setHorizontalAngle(o);
	setVerticalAngle(p);
	setZoom(r);
	setOrigin(vec3f(0.0f, 0.0f, 0.0f));
	setCenter(vec3f(0.0f, 0.0f, 0.0f));
}

//Set our horizontal angle can be any value (m_omega)
void CArcBallCamera::setHorizontalAngle(float o)
{
	m_omega = o;
}

//Set our vertical angle. This is clamped between 0 and 180
void CArcBallCamera::setVerticalAngle(float p)
{
	Clampf(p, verticalAngleMin, verticalAngleMax);
	m_phi = p;
}

//Zoom or CCamera distance from scene is clamped.
void CArcBallCamera::setZoom(float r)
{
	Clampf(r, zoomMin, zoomMax);
	m_rho = r;
}

//Set Origin position of Camera
void CArcBallCamera::setOrigin(vec3f org)
{
	m_origin = org;
}

void CArcBallCamera::setCenter(vec3f c)
{
	m_center = c;
}

//convert spherical coordinates to Eulerian values
vec3f CArcBallCamera::getCoordinates()
{
	vec3f p = m_origin;

	p.x += float(m_rho * sin(m_phi) * cos(m_omega));
	p.z += float(m_rho * sin(m_phi) * sin(m_omega));
	p.y += float(m_rho * cos(m_phi));
	return p;
}

//Return Current CCamera Direction
vec3f CArcBallCamera::getDirection()
{	
	vec3f dir = m_origin - getCoordinates();

	dir.normalize();
	return dir;
}

//Calculate an Up vector
vec3f CArcBallCamera::getUp()
{	
	float o = (m_omega + PiOver2);
	float ph = Absolutef(m_phi - PiOver2);

	vec3f p;
	p.x = (m_rho * cos(o) * sin(ph));
	p.z = (m_rho * sin(o) * sin(ph));
	p.y = (m_rho * cos(ph));
	p.normalize();
	return p;
}

vec3f CArcBallCamera::getStrafe()
{
	vec3f dir;
	dir = getCoordinates();
	dir.normalize();
	dir.cross(getUp());
	return dir;
}

void CArcBallCamera::goHome()
{
	m_omega = horizontalAngle;
	m_phi = verticalAngle;
	m_rho = zoom;
	m_origin = vec3f(0.0f, 0.0f, 0.0f);
	m_center = vec3f(0.0f, 0.0f, 0.0f);
}

}