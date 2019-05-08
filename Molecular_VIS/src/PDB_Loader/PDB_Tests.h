#pragma once
#include <string>
#include <gemmi/cif.hpp>
#include <gemmi/mmcif.hpp>
#include <gemmi/model.hpp>
#include <gemmi/chemcomp.hpp>
#include "../Geometry/Geometry.h"


struct AtomVertex {
	gemmi::Element el = gemmi::El::X;
	std::string name;
};

struct BondEdge
{
	gemmi::BondType type;
};

class PDB_Tests
{
private:
	gemmi::Structure molStructure;
	gemmi::ChemComp ccMol;
	std::shared_ptr<Shader> shader;
public:
	PDB_Tests(const std::string& dir);
	~PDB_Tests();

	std::vector<glm::vec3> doStuff();
	std::shared_ptr<Shader> getShader();
	void buildAtomTable();
};

