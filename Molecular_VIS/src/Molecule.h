#pragma once
#include <glm/glm.hpp>
#include <vector>;


struct Atom {
	glm::vec3 position;
	float radius;
	
	Atom(glm::vec3 position, float radius) :
		position(position), radius(radius) {}
};

struct Molecule {
	std::vector<Atom> atoms;
	glm::vec3 max_pos;
	glm::vec3 min_pos;
	float max_radius;
};