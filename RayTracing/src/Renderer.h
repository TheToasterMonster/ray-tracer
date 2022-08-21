#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render();

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
	uint32_t ConvertToRGBA(const glm::vec4& color);
	uint32_t CalcPixelForSphere(const glm::vec2& coord, const Sphere& sphere);
	uint32_t PerPixel(const glm::vec2& coord);

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;	
	uint32_t* m_ImageData = nullptr;
	std::vector<Sphere> spheres{
		Sphere(0.5f, glm::vec3(0.0f, 0.0f, 0.0f))
	};
	glm::vec3 lightSource{ 3.0f, 3.0f, 3.0f };
};