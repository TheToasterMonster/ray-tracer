#include "Sphere.h"

Sphere::Sphere(float radius, const glm::vec3& center)
{
	this->radius = radius;
	this->center = center;
}

Sphere::Sphere(float radius)
{
	this->radius = radius;
	this->center = glm::vec3(0.0f);
}
