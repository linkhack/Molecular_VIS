#pragma once
#include <glm/glm.hpp>
#include <vector>;

/*!
 * \brief Struct for atom data
 * 
 */
struct Atom {
	/*!
	 * position of the atom.
	 * 
	 */
	glm::vec3 position;

	/*!
	 * Van der Waals radius of the atom
	 * 
	 */
	float radius;
	
	/*!
	 * Constructor
	 * 
	 * \param position Position of atom.
	 * \param radius Van der Waals radius of the atom.
	 */
	Atom(glm::vec3 position, float radius) :
		position(position), radius(radius) {}
};

/*!
 * \brief Struct for molecule data.
 * 
 */
struct Molecule {
	/*!
	 * Vector of all atoms of the molecule
	 * 
	 */
	std::vector<Atom> atoms;
	/// \name Bounding-Box
	//@{
	/**
	 * Bounding box for the molecule. All atoms are between min_pos and max_pos
	 * 
	 **/
	glm::vec3 max_pos;
	glm::vec3 min_pos;
	//@}
	/*!
	 * Maximum of van der Waals radii of the atoms of the molecule.
	 * 
	 */
	float max_radius;
};