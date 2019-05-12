#include "PDB_Tests.h"
#include <iostream>
#include <gemmi/cif.hpp>
#include <gemmi/mmcif.hpp>
#include <gemmi/mmread.hpp>
#include <gemmi/chemcomp.hpp>
#include <gemmi/model.hpp>
#include <glm/common.hpp>
#include "../Material/LambertMaterial.h"



PDB_Tests::PDB_Tests(const std::string& dir)
{
	//gemmi::cif::Document bla = gemmi::cif::read_file(dir);
	molStructure = gemmi::read_structure_file(dir, gemmi::CoorFormat::Mmcif);
}


PDB_Tests::~PDB_Tests()
{
}

std::vector<glm::vec3> PDB_Tests::doStuff()
{
	std::vector<glm::vec3> matrices;
	glm::vec3 sum;
	glm::vec3 max=-glm::vec3(10000,10000,10000);
	glm::vec3 min=-max;
	for (const gemmi::Model& model : molStructure.models) {
		for (const gemmi::Chain& chain : model.chains) {
			for (const gemmi::Residue& res : chain.residues) {
				for (const gemmi::Atom& atom : res.atoms) {
					glm::vec3 pos = glm::vec3(atom.pos.x, atom.pos.y, atom.pos.z);
					sum += pos;
					matrices.push_back(std::move(pos));
					max = glm::max(max, pos);
					min = glm::min(min, pos);
				}
			}
		}
	}
	std::transform(matrices.begin(), matrices.end(), matrices.begin(), [min, max](auto pos) {return (pos - max) / (max - min); });
	return std::move(matrices);
}

std::shared_ptr<Shader> PDB_Tests::getShader()
{
	return shader;
}

void PDB_Tests::buildAtomTable()
{
	
}
