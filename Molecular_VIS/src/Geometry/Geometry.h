#pragma 
#include <vector>
#include <memory>
#include <glm/ext.hpp>
#include <GL/glew.h>

#include "../Material/Material.h"


struct Vertex {
	//Vertex data
	glm::vec3 positions;
	glm::vec3 normals;
	glm::vec2 uv;
};

struct GeometryData {
	//Vertex data
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uv;
	//Indices
	std::vector<unsigned int> indices;
};

class Geometry
{
protected:
	glm::mat4 modelMatrix;
public:
	virtual void draw(glm::mat4 matrix = glm::mat4(1.0f))=0;
	virtual ~Geometry();
};

class ProceduralGeometry : public Geometry
{
public:
	ProceduralGeometry(glm::mat4 modelMatrix, GeometryData& geometryData, std::shared_ptr<Material> material);
	ProceduralGeometry(glm::mat4 modelMatrix, GeometryData& geometryData, std::shared_ptr<Shader> shader);
	virtual ~ProceduralGeometry();

	virtual void draw(glm::mat4 matrix = glm::mat4(1.0f));

	//Construction helper
	static GeometryData createCubeGeometry(float width, float height, float depth);
	static GeometryData createSphereGeometry(float radius, unsigned int longitudeSegments, unsigned int latitudeSegments);
	static GeometryData createCylinderGeometry(float radius, float height, unsigned int segments);
	static GeometryData createFullScreenQuad();

private:
	bool isEmpty = true;
	//Buffers
	GLuint vao;

	GLuint vboPositions;
	GLuint vboNormals;
	GLuint vboUV;
	GLuint vboIndices;

	//nrOfVertices
	int nrOfVertices;

	//Matrices
	glm::mat4 modelMatrix;

	//Shader and Material stuff like color
	std::shared_ptr<Material> material;
	glm::vec3 color;
};
