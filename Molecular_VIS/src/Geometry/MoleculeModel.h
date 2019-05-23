#pragma once
#include "Geometry.h"
#include "../Molecule.h"

class MoleculeModel:public Geometry
{
public:
	MoleculeModel(glm::mat4 modelMatrix, std::shared_ptr<Material> material, Molecule& molecule);
	virtual ~MoleculeModel();

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
	
	//Geometry calc
	GeometryData calcGeometry(std::vector<glm::vec3>& positions);
};

