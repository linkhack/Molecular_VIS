#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <string>
#include "gemmi/model.hpp"



struct Atom
{
	glm::vec4 position_radius;
};

class Molecule
{
private:
	/*
		Has position between 0 1, VdW radius is scaled accordingly
	*/
	std::unique_ptr<std::vector<Atom>> atoms;

	float scaleFactor;
public:
	Molecule();
	
	/**
	* Builds molecule from file, should be pdb/mmCIF format
	**/
	Molecule(const std::string file);

	/**
	* Builds molecule from gemmi structure
	**/
	Molecule(const gemmi::Structure gemmi_structure);

	/**
	* Builds Molecule from Array, and old Scale_factor
	**/
	~Molecule();
};

