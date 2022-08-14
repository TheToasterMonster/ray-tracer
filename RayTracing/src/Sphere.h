#pragma once
#include <glm/glm.hpp>

class Sphere
{
public:
	float radius;
	glm::vec3 center;

public:
	Sphere(float radius, const glm::vec3& center);
	Sphere(float radius);
};

