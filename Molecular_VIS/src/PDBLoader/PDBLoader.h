#pragma once
#include <string>
#include <vector>
#include <gemmi/cif.hpp>
#include <gemmi/mmcif.hpp>
#include <gemmi/model.hpp>
#include <gemmi/chemcomp.hpp>
#include "../Geometry/Geometry.h"
#include <glm/glm.hpp>
#include "../Molecule/Molecule.h"

class PDBLoader
{
private:
	gemmi::Structure molStructure;
public:
	PDBLoader(const std::string& dir);
	~PDBLoader();

	Molecule getMolecule();
	void buildAtomTable();
};

