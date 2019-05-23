#pragma once
#include <string>
#include <vector>
#include <gemmi/cif.hpp>
#include <gemmi/mmcif.hpp>
#include <gemmi/model.hpp>
#include <gemmi/chemcomp.hpp>
#include "../Geometry/Geometry.h"
#include <glm/glm.hpp>
#include "../Molecule.h"

class PDB_Tests
{
private:
	gemmi::Structure molStructure;
	gemmi::ChemComp ccMol;
	std::shared_ptr<Shader> shader;
public:
	PDB_Tests(const std::string& dir);
	~PDB_Tests();

	Molecule doStuff();
	std::shared_ptr<Shader> getShader();
	void buildAtomTable();
};

