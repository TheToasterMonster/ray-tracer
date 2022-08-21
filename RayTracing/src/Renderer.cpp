#include "Renderer.h"

#include "Walnut/Random.h"
#include "imgui.h"
#include <iostream>

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render()
{
	if (ImGui::IsKeyPressed(ImGuiKey_W))
	{
		for (Sphere& sphere : spheres)
		{
			sphere.center.y += 1.0f;
		}
	}
	if (ImGui::IsKeyPressed(ImGuiKey_A))
	{
		for (Sphere& sphere : spheres)
		{
			sphere.center.x -= 1.0f;
		}
	}
	if (ImGui::IsKeyPressed(ImGuiKey_S))
	{
		for (Sphere& sphere : spheres)
		{
			sphere.center.y -= 1.0f;
		}
	}
	if (ImGui::IsKeyPressed(ImGuiKey_D))
	{
		for (Sphere& sphere : spheres)
		{
			sphere.center.x += 1.0f;
		}
	}

	if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
	{
		for (Sphere& sphere : spheres)
		{
			lightSource.y += 1.0f;
		}
	}
	if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
	{
		for (Sphere& sphere : spheres)
		{
			lightSource.x -= 1.0f;
		}
	}
	if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
	{
		for (Sphere& sphere : spheres)
		{
			lightSource.y -= 1.0f;
		}
	}
	if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
	{
		for (Sphere& sphere : spheres)
		{
			lightSource.x += 1.0f;
		}
	}

	float aspectRatio = m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; // -1 -> 1
			coord.x *= aspectRatio;
			m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}


uint32_t Renderer::ConvertToRGBA(const glm::vec4& color)
{
	return ((uint32_t)(color.r * 255))
		 | ((uint32_t)(color.g * 255) << 8)
		 | ((uint32_t)(color.b * 255) << 16)
		 | ((uint32_t)(color.a * 255) << 24);
}

uint32_t Renderer::CalcPixelForSphere(const glm::vec2& coord, const Sphere& sphere)
{
	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	rayDirection = glm::normalize(rayDirection);

	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// where
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit distance

	// (a+bt)^2 - 2h(a + bt) + h^2
	// a^2 + (2ab)t + (b^2)(t^2) - 2ah - 2bh(t) + h^2
	// (b^2)t^2 + (2ab - 2bh)t + (a^2 - 2ah + h^2)
	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f *
			(glm::dot(rayOrigin, rayDirection)
				- glm::dot(rayDirection, sphere.center));
	float c = glm::dot(rayOrigin, rayOrigin)
			+ glm::dot(sphere.center, sphere.center)
			- glm::dot(rayOrigin, sphere.center)
			- sphere.radius * sphere.radius;

	// Quadratic forumula discriminant:
	// b^2 - 4ac
	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
		return 0xff << 24;

	// Quadratic formula:
	// (-b +- sqrt(discriminant)) / 2a
	float discriminantSqrt = sqrt(discriminant);
	float t[2] = {
		(-b - discriminantSqrt) / (2.0f * a),
		(-b + discriminantSqrt) / (2.0f * a)
	};

	glm::vec3 hitPoint = rayOrigin + rayDirection * t[0];
	glm::vec3 normal = glm::normalize(hitPoint - sphere.center);

	//glm::vec3 lightSource(1.0f, 1.0f, 1.0f);
	glm::vec3 lightDirection = hitPoint - lightSource;
	lightDirection = glm::normalize(lightDirection);

	//glm::vec3 color(1.0f, 0.0f, 1.0f);
	//glm::vec3 color = normal * 0.5f + 0.5f;
	float lightIntensity = glm::max(glm::dot(-lightDirection, normal), 0.0f);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.0f) * lightIntensity;
	return ConvertToRGBA(glm::vec4(color, 1.0f));
}

uint32_t Renderer::PerPixel(const glm::vec2& coord)
{
	uint32_t color = 0xff << 24;
	for (const Sphere& sphere : spheres)
	{
		color = std::max(color, CalcPixelForSphere(coord, sphere));
	}
	return color;
}
