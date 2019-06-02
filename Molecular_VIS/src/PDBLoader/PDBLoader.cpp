#include "PDBLoader.h"
#include <iostream>
#include <random>
#include <gemmi/cif.hpp>
#include <gemmi/mmcif.hpp>
#include <gemmi/mmread.hpp>
#include <gemmi/chemcomp.hpp>
#include <gemmi/model.hpp>
#include <glm/common.hpp>
#include <gemmi/elem.hpp>
#include "../Material/LambertMaterial.h"



PDBLoader::PDBLoader(const std::string& dir)
{
	//gemmi::cif::Document bla = gemmi::cif::read_file(dir);
	molStructure = gemmi::read_structure_file(dir, gemmi::CoorFormat::Mmcif);
}


PDBLoader::~PDBLoader()
{
}

Molecule PDBLoader::getMolecule()
{
	Molecule result;
	glm::vec3 sum = glm::vec3(0.f);
	glm::vec3 max=-glm::vec3(100000.0,100000.,100000.);
	glm::vec3 min=-max;
	float max_radius=0.f;
	
	gemmi::Model& model = molStructure.models.front();
	for (const gemmi::Chain& chain : model.chains) {
		for (const gemmi::Residue& res : chain.residues) {
			for (const gemmi::Atom& atom : res.atoms) {
				glm::vec3 pos = glm::vec3(atom.pos.x, atom.pos.y, atom.pos.z);
				float radius = atom.element.VdW_r();
				max = glm::max(max, pos);
				min = glm::min(min, pos);
				max_radius = std::max(radius, max_radius);
				sum += pos;
				result.atoms.emplace_back(pos, radius);
			}
		}
	}

	sum /= result.atoms.size();
	std::for_each(result.atoms.begin(), result.atoms.end(), [sum](Atom& atom) {atom.position -= sum; });
	result.max_pos = max-sum;
	result.min_pos = min-sum;
	result.max_radius = max_radius;
	return std::move(result);
}