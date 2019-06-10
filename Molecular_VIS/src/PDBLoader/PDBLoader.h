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

/*!
 * \brief Wrapper to simplify pdbx/mmcif format import.
 
 */
class PDBLoader
{
private:
	gemmi::Structure molStructure;
public:
	/*!
	 * Loads molecule data from a pdbx/mmcif file.
	 * 
	 * \param dir Path to file.
	 */
	PDBLoader(const std::string& dir);
	~PDBLoader();

	/*!
	 * Gets the molecule as Molecule.
	 * 
	 * \return The molecule as Molecule.
	 */
	Molecule getMolecule();
};

