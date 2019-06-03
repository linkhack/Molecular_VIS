#include "MoleculeModel.h"
#include <glm/glm.hpp>


MoleculeModel::MoleculeModel(glm::mat4 modelMatrix, std::shared_ptr<Material> material, Molecule& molecule)
	:modelMatrix(modelMatrix), material(material)
{	
	GeometryData geometryData;
	for (const auto& atom : molecule.atoms)
	{
		ProceduralGeometry::createSphereGeometry(0.5f*atom.radius, 8u, 8u, atom.position, geometryData);
	}

	//Create Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//create vertex position array

	glGenBuffers(1, &vboPositions);	glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
	glBufferData(GL_ARRAY_BUFFER, geometryData.positions.size() * sizeof(glm::vec3), geometryData.positions.data(), GL_STATIC_DRAW);
	//Bind vertex positions to location 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//create normals array buffer
	glGenBuffers(1, &vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER, geometryData.normals.size() * sizeof(glm::vec3), geometryData.normals.data(), GL_STATIC_DRAW);
	//Bind vertex normals to location 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//create uv array buffer
	glGenBuffers(1, &vboUV);
	glBindBuffer(GL_ARRAY_BUFFER, vboUV);
	glBufferData(GL_ARRAY_BUFFER, geometryData.uv.size() * sizeof(glm::vec2), geometryData.uv.data(), GL_STATIC_DRAW);
	//Bind vertex uv to location 2
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);


	//create Index Array
	glGenBuffers(1, &vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometryData.indices.size() * sizeof(unsigned int), geometryData.indices.data(), GL_STATIC_DRAW);

	//Reset all bindings to 0
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//set color
	color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	nrOfVertices = geometryData.indices.size();

	isEmpty = false;
}

MoleculeModel::~MoleculeModel()
{

}

void MoleculeModel::draw(glm::mat4 matrix)
{

	//Combined Matrices, would allow hierarchical animation later
	glm::mat4 totalMatrix = matrix * modelMatrix;
	std::shared_ptr<Shader> shader = material->getShader();
	//set Model Uniforms
	material->setUniforms();
	shader->use();
	shader->setUniform("modelMatrix", totalMatrix);
	shader->setUniform("normalMatrix", glm::mat3(glm::inverse(glm::transpose(totalMatrix))));
	//shader->setUniform("materialColor", color);
	//Bind Buffers
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, nrOfVertices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
