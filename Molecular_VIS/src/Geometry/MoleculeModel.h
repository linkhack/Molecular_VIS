#pragma once
#include "Geometry.h"
#include "../Molecule/Molecule.h"

/*!
 * \brief Class representing the molecule as geometric model.
 * 
 * The atoms will be represented by balls with radius proportional to the van der Waals radius.
 */
class MoleculeModel:public Geometry
{
public:
	/*!
	 * Constructs the geometric model of a given molecule.
	 * 
	 * \param modelMatrix Modelmatrix, gives transformation from object space to world space.
	 * \param material The material that should be used for drawing.
	 * \param molecule The molecule for which a model should be constructed.
	 */
	MoleculeModel(glm::mat4 modelMatrix, std::shared_ptr<Material> material, Molecule& molecule);
	virtual ~MoleculeModel();

	/*!
	 * Draws the model.
	 * 
	 * \param matrix Defines a transformation from word space to word space. Gets left-multiplied with the model matrix.
	 */
	virtual void draw(glm::mat4 matrix = glm::mat4(1.0f));
private:
	bool isEmpty = true;
	//Buffers
	GLuint vao;

	GLuint vboPositions;
	GLuint vboNormals;
	GLuint vboUV;
	GLuint vboIndices;
	GLuint vboInstance;

	//nrOfVertices
	int nrOfVertices;
	int nrOfAtoms;

	//Matrices
	glm::mat4 modelMatrix;

	//Shader and Material stuff like color
	std::shared_ptr<Material> material;
	glm::vec3 color;
};

