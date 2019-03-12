#include "Geometry.h"

ProceduralGeometry::ProceduralGeometry(glm::mat4 modelMatrix, GeometryData & geometryData, std::shared_ptr<Material> material)
	:material(material), nrOfVertices(geometryData.indices.size())
{
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

	isEmpty = false;
}
ProceduralGeometry::ProceduralGeometry(glm::mat4 modelMatrix, GeometryData & geometryData, std::shared_ptr<Shader> shader)
	:ProceduralGeometry(modelMatrix,geometryData, std::make_shared<Material>(shader))
{
}

ProceduralGeometry::~ProceduralGeometry()
{
	if (!isEmpty)
	{
		glDeleteBuffers(1, &vboIndices);
		glDeleteBuffers(1, &vboPositions);
		glDeleteBuffers(1, &vboNormals);
		glDeleteBuffers(1, &vboUV);
		glDeleteVertexArrays(1, &vao);
		std::cout << "Buffers deleted" << std::endl;
	}
}

void ProceduralGeometry::draw(glm::mat4 matrix)
{
	//Combined Matrices, would allow hierarchical animation later
	glm::mat4 totalMatrix = matrix * modelMatrix;
	std::shared_ptr<Shader> shader = material->getShader();
	//set Model Uniforms
	material->setUniforms(0);
	shader->use();
	shader->setUniform("modelMatrix", totalMatrix);
	shader->setUniform("normalMatrix", glm::mat3(glm::inverse(glm::transpose(totalMatrix))));
	shader->setUniform("materialColor", color);
	//Bind Buffers
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, nrOfVertices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

GeometryData ProceduralGeometry::createCubeGeometry(float width, float height, float depth)
{
	GeometryData data;

	//Vertices position
	data.positions = {
		//front
		glm::vec3(-width / 2.0f, height / 2.0f, depth / 2.0f), //0
		glm::vec3(-width / 2.0f, -height / 2.0f, depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f, depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f, depth / 2.0f),

		//back
		glm::vec3(width / 2.0f, height / 2.0f, -depth / 2.0f), //4
		glm::vec3(width / 2.0f, -height / 2.0f, -depth / 2.0f),
		glm::vec3(-width / 2.0f, -height / 2.0f, -depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f, -depth / 2.0f),

		//top
		glm::vec3(-width / 2.0f, height / 2.0f, depth / 2.0f), //8
		glm::vec3(width / 2.0f, height / 2.0f, depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f, -depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f, -depth / 2.0f),

		//bottom
		glm::vec3(width / 2.0f, -height / 2.0f, -depth / 2.0f), //12
		glm::vec3(-width / 2.0f, -height / 2.0f, -depth / 2.0f),
		glm::vec3(-width / 2.0f, -height / 2.0f, depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f, depth / 2.0f),

		//left
		glm::vec3(-width / 2.0f, height / 2.0f, -depth / 2.0f), //16
		glm::vec3(-width / 2.0f, -height / 2.0f, -depth / 2.0f),
		glm::vec3(-width / 2.0f, -height / 2.0f, depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f, depth / 2.0f),

		//right
		glm::vec3(width / 2.0f, -height / 2.0f, depth / 2.0f), //20
		glm::vec3(width / 2.0f, height / 2.0f, depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f, -depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f, -depth / 2.0f)
	};

	data.normals = {
		//front
		glm::vec3(0.0f,0.0f,1.0f),
		glm::vec3(0.0f,0.0f,1.0f),
		glm::vec3(0.0f,0.0f,1.0f),
		glm::vec3(0.0f,0.0f,1.0f),

		//back
		glm::vec3(0.0f,0.0f,-1.0f),
		glm::vec3(0.0f,0.0f,-1.0f),
		glm::vec3(0.0f,0.0f,-1.0f),
		glm::vec3(0.0f,0.0f,-1.0f),

		//top
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f),

		//bottom
		glm::vec3(0.0f,-1.0f,0.0f),
		glm::vec3(0.0f,-1.0f,0.0f),
		glm::vec3(0.0f,-1.0f,0.0f),
		glm::vec3(0.0f,-1.0f,0.0f),

		//left
		glm::vec3(-1.0f,0.0f,0.0f),
		glm::vec3(-1.0f,0.0f,0.0f),
		glm::vec3(-1.0f,0.0f,0.0f),
		glm::vec3(-1.0f,0.0f,0.0f),

		//right
		glm::vec3(1.0f,0.0f,0.0f),
		glm::vec3(1.0f,0.0f,0.0f),
		glm::vec3(1.0f,0.0f,0.0f),
		glm::vec3(1.0f,0.0f,0.0f)
	};

	data.uv = {
		//front
		glm::vec2(0.0f,1.0f),
		glm::vec2(0.0f,0.0f),
		glm::vec2(1.0f,0.0f),
		glm::vec2(1.0f,1.0f),

		//back
		glm::vec2(1.0f,1.0f),
		glm::vec2(1.0f,0.0f),
		glm::vec2(0.0f,0.0f),
		glm::vec2(0.0f,1.0f),

		//top
		glm::vec2(0.0f,1.0f),
		glm::vec2(1.0f,1.0f),
		glm::vec2(1.0f,0.0f),
		glm::vec2(0.0f,0.0f),

		//bottom
		glm::vec2(1.0f,0.0f),
		glm::vec2(0.0f,0.0f),
		glm::vec2(0.0f,1.0f),
		glm::vec2(1.0f,1.0f),

		//left
		glm::vec2(0.0f,1.0f),
		glm::vec2(0.0f,0.0f),
		glm::vec2(1.0f,0.0f),
		glm::vec2(1.0f,1.0f),

		//right
		glm::vec2(1.0f,0.0f),
		glm::vec2(1.0f,1.0f),
		glm::vec2(0.0f,1.0f),
		glm::vec2(0.0f,0.0f)


	};

	data.indices = {
		//Front
		0,1,2,
		2,3,0,

		//Back
		4, 5, 6,
		6, 7, 4,

		//Top
		8, 9, 10,
		10, 11, 8,

		//bottom
		12,14,13,
		14,12,15,

		//left
		16,17,18,
		18,19,16,

		//right
		20,22,21,
		22,20,23
	};

	return std::move(data);
}

GeometryData ProceduralGeometry::createSphereGeometry(float radius, unsigned int longitudeSegments, unsigned int latitudeSegments)
{
	GeometryData data;

	//top pole
	for (unsigned int i = 0; i < longitudeSegments + 1; ++i)
	{
		data.positions.push_back(glm::vec3(0, +radius, 0));
		data.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		data.uv.push_back(glm::vec2(float(i) / (longitudeSegments), 1.0f));
		data.positions.push_back(glm::vec3(0.0f, -radius, 0.0f));
		data.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		data.uv.push_back(glm::vec2(float(i) / (longitudeSegments), 0.0f));
		//first ring indices
	}
	for (unsigned int i = 0; i < longitudeSegments; ++i)
	{
		data.indices.push_back(2 * i);
		data.indices.push_back(2 * (longitudeSegments + 1) + 1 + i);
		data.indices.push_back(2 * (longitudeSegments + 1) + 0 + i);

		data.indices.push_back(2 * i + 1);
		data.indices.push_back((latitudeSegments)*((longitudeSegments + 1)) + i);
		data.indices.push_back((latitudeSegments)*((longitudeSegments + 1)) + i + 1);
	}

	//construct rings and vertices
	for (unsigned int latIndex = 1; latIndex < latitudeSegments; ++latIndex) {
		float verticalAngle = float(latIndex) * glm::pi<float>() / float(latitudeSegments);
		for (unsigned int longIndex = 0; longIndex < longitudeSegments + 1; ++longIndex) {
			float horizontalAngle = 2 * float(longIndex) * glm::pi<float>() / float(longitudeSegments);
			data.positions.push_back(glm::vec3(
				radius * glm::sin(verticalAngle) * glm::cos(horizontalAngle),
				radius * glm::cos(verticalAngle),
				radius * glm::sin(verticalAngle) * glm::sin(horizontalAngle)
			));
			//normal is in direcction of vertex but with unit length
			data.normals.push_back(glm::vec3(
				glm::sin(verticalAngle) * glm::cos(horizontalAngle),
				glm::cos(verticalAngle),
				glm::sin(verticalAngle) * glm::sin(horizontalAngle)
			));
			data.uv.push_back(glm::vec2(static_cast<float>(longIndex) / longitudeSegments, static_cast<float>(latitudeSegments - latIndex) / (latitudeSegments)));
			if (latIndex == 1 || longIndex == longitudeSegments) continue;

			data.indices.push_back((latIndex - 1)*(longitudeSegments + 1) + longIndex + 2 * (longitudeSegments + 1));
			data.indices.push_back((latIndex - 2)*(longitudeSegments + 1) + longIndex + 2 * (longitudeSegments + 1));
			data.indices.push_back((latIndex - 2)*(longitudeSegments + 1) + longIndex + 1 + 2 * (longitudeSegments + 1));


			data.indices.push_back((latIndex - 1)*(longitudeSegments + 1) + longIndex + 2 * (longitudeSegments + 1));
			data.indices.push_back((latIndex - 2)*(longitudeSegments + 1) + longIndex + 1 + 2 * (longitudeSegments + 1));
			data.indices.push_back((latIndex - 1)*(longitudeSegments + 1) + longIndex + 1 + 2 * (longitudeSegments + 1));

		}
	}

	return std::move(data);
}

GeometryData ProceduralGeometry::createCylinderGeometry(float radius, float height, unsigned int segments)
{
	GeometryData data;

	//bottom
	data.positions.push_back(glm::vec3(0, -height / 2.0f, 0.0f));
	data.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	data.uv.push_back(glm::vec2(0.5f, 0.5f));
	//top
	data.positions.push_back(glm::vec3(0, height / 2.0f, 0.0f));
	data.normals.push_back(glm::vec3(0, 1.0f, 0.0f));
	data.uv.push_back(glm::vec2(0.5f, 0.5f));


	for (unsigned int i = 0; i < segments + 1; i++)
	{
		float angle = 2 * i * glm::pi<float>() / float(segments);
		//two vertices because of normals (one one normal for bottom/top, and one normal to side face
		//Vertices are ordered counterclockwise!!
		glm::vec3 circelPos = radius * glm::vec3(glm::cos(angle), 0, glm::sin(angle));
		glm::vec3 circlePosTop = circelPos + glm::vec3(0.0f, height / 2.0f, 0.0f);
		glm::vec3 circlePosBot = circelPos - glm::vec3(0.0f, height / 2.0f, 0.0f);
		glm::vec2 circleInSquareUV = 0.5f / radius * glm::vec2(circelPos.x, circelPos.z) + 0.5f;

		data.positions.push_back(circlePosBot); // 2 + 4i //bottom face
		data.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		data.uv.push_back(circleInSquareUV);

		data.positions.push_back(glm::vec3(radius*glm::cos(angle), -height / 2.0f, radius*glm::sin(angle))); // 3 + 4i //side face bottom
		data.normals.push_back(glm::vec3(glm::cos(angle), 0.0f, glm::sin(angle)));
		data.uv.push_back(glm::vec2(static_cast<float>(i) / (segments), 0.0f));

		data.positions.push_back(glm::vec3(radius*glm::cos(angle), height / 2.0f, radius*glm::sin(angle)));  // 4 + 4i //side face top
		data.normals.push_back(glm::vec3(glm::cos(angle), 0.0f, glm::sin(angle)));
		data.uv.push_back(glm::vec2(static_cast<float>(i) / segments, 1.0f));

		data.positions.push_back(glm::vec3(radius*glm::cos(angle), height / 2.0f, radius* glm::sin(angle)));  // 5 + 4i //top face
		data.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		data.uv.push_back(circleInSquareUV);

		//bottom faces
		if (i == segments) continue;
		data.indices.push_back(0);
		data.indices.push_back(2 + 4 * i);
		data.indices.push_back(2 + 4 * (i + 1));

		//side faces
		data.indices.push_back(3 + 4 * i);
		data.indices.push_back(4 + 4 * (i + 1));
		data.indices.push_back(3 + 4 * (i + 1));


		data.indices.push_back(3 + 4 * i);
		data.indices.push_back(4 + 4 * i);
		data.indices.push_back(4 + 4 * (i + 1));

		//top face
		data.indices.push_back(1);
		data.indices.push_back(5 + 4 * (i + 1));
		data.indices.push_back(5 + 4 * i);
	}
	return std::move(data);
}

GeometryData ProceduralGeometry::createFullScreenQuad()
{
	GeometryData data;

	//Vertices position
	data.positions = {
		glm::vec3(-1.0f,1.0f,0.0f),
		glm::vec3(-1.0f,-1.0f,0.0f),
		glm::vec3(1.0f,1.0f,0.0f),
		glm::vec3(1.0f,-1.0f,0.0f)
	};

	data.normals = {
		//glm::vec3(0.0f,-1.0f,0.0f),
		//glm::vec3(0.0f,-1.0f,0.0f),
		//glm::vec3(0.0f,-1.0f,0.0f),
		//glm::vec3(0.0f,-1.0f,0.0f)
	};

	data.uv = {
		glm::vec2(0.0f,1.0f),
		glm::vec2(0.0f,0.0f),
		glm::vec2(1.0f,1.0f),
		glm::vec2(1.0f,0.0f),
	};

	data.indices = {
		0,1,2,
		1,3,2
	};

	return std::move(data);
}
