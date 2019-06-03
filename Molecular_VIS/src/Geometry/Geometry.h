#pragma once
#include <vector>
#include <memory>
#include <glm/ext.hpp>
#include <GL/glew.h>

#include "../Material/Material.h"

/*!
 * \brief Struct for Vertex Data
 * 
 */
struct Vertex {
	//Vertex data
	glm::vec3 positions; /*!Vertex position. */
	glm::vec3 normals; /*! Vertex normal. */
	glm::vec2 uv; /*! Vertex uv coordinate. */
};

/*!
 * \brief Stuct holding the vertices and indeces of the Geometry.
 *
 */
struct GeometryData {
	//Vertex data

	/*! Vector of vertex Position. One entry for each vertex. */
	std::vector<glm::vec3> positions;

	/*! Vector of vertex normals. One entry for each vertex*/
	std::vector<glm::vec3> normals;

	/*! Vector of vertex uv-coordinates. One entry for each vertex*/
	std::vector<glm::vec2> uv;

	//Indices
	/*! Vector of indices defining the connectivity of the vertices*/
	std::vector<unsigned int> indices; 
};

/*!
 * \brief Base class for Geometry classes.
 * 
 */
class Geometry
{
protected:
	/*! contains model matrix. Transforms geometry from object to world space.*/
	glm::mat4 modelMatrix; 
public:
	/*!
	 * Abstract function defining how to draw
	 * 
	 * \param matrix Defines a transformation from word space to word space. Gets left-multiplied with the model matrix.
	 */
	virtual void draw(glm::mat4 matrix = glm::mat4(1.0f))=0;
	virtual ~Geometry();
};

/*!
 * \brief Class for procedurally genereted geometries, like spheres and cubes.
 * 
 */
class ProceduralGeometry : public Geometry
{
public:
	/*!
	 * Creates a procedural geometry object
	 * 
	 * \param modelMatrix The model matrix of the geometry. Transforms geometry from object to world space.
	 * \param geometryData Data of the vertices and connectivity. This is used to construct the indexed buffers.
	 * \param material Material of the geometry, defining the brdf properties of the geometry.
	 */
	ProceduralGeometry(glm::mat4 modelMatrix, GeometryData& geometryData, std::shared_ptr<Material> material);
	/*!
	 * Creates a procedural geometry object with standard material values for a given shader.
	 * 
	 * \param modelMatrix The model matrix of the geometry. Transforms geometry from object to world space.
	 * \param geometryData Data of the vertices and connectivity. This is used to construct the indexed buffers.
	 * \param shader Shader that is used to draw. Is used to construct the default material.
	 */
	ProceduralGeometry(glm::mat4 modelMatrix, GeometryData& geometryData, std::shared_ptr<Shader> shader);
	virtual ~ProceduralGeometry();

	/*!
	 * Function defining how to draw the Geometry.
	 *
	 * \param matrix Defines a transformation from word space to word space. Gets left-multiplied with the model matrix.
	 */
	virtual void draw(glm::mat4 matrix = glm::mat4(1.0f));



	//Construction helper
	/*!
	 * Creates GeometryData for a cube.
	 * 
	 * \param width, height, depth dimensions of the cube
	 * \return GeometryData for a cube.
	 */
	static GeometryData createCubeGeometry(float width, float height, float depth);
	/*!
	 * Creates GeometryData for a sphere.
	 * 
	 * \param radius Radius of sphere
	 * \param longitudeSegments Number of segments along longitude.
	 * \param latitudeSegments Number of segments along latitude.
	 * \return GeometryData for a sphere.
	 */
	static GeometryData createSphereGeometry(float radius, unsigned int longitudeSegments, unsigned int latitudeSegments);
	/*!
	 * Append a Sphere to the given GeometryData at a given position. Can be used to construct a collection of spheres.
	 * 
	 * \param radius Radius of sphere
	 * \param longitudeSegments Number of segments along longitude.
	 * \param latitudeSegments Number of segments along latitude.
	 * \param position Coordinates of position where this sphere will be added.
	 * \param[in,out] geometry GeometryData to which the sphere will be added.
	 */
	static void createSphereGeometry(float radius, unsigned int longitudeSegments, unsigned int latitudeSegments, glm::vec3 position, GeometryData& geometry);
	
	/*!
	 * Creates GeometryData for a cylinder.
	 * 
	 * \param radius Radius of cylinder.
	 * \param height Height of cylinder.
	 * \param segments Number of segments along cylinder used for triangulation.
	 * \return GeometryData for a cylinder.
	 */
	static GeometryData createCylinderGeometry(float radius, float height, unsigned int segments);

	/*!
	 * Creates a full-screen quad with texture coordinates from (0,0) to (1,1)
	 * 
	 * \return  GeometryData for a full-screen quad.
	 */
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

