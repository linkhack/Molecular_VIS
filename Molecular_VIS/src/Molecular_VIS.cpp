// SDF_Raymarcher.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <sstream>
#include <iostream>
#include <memory>
#include <string>
#include <algorithm>


#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "Shader/Shader.h"
#include "Geometry/Geometry.h"
#include "Camera/Camera.h"
#include "Light/LightManager.h"
#include "PDB_Loader/PDB_Tests.h"
#include "Material/LambertMaterial.h"
#include "Geometry/MoleculeModel.h"
#include "Texture/VolumetricTexture.h"

#include "Shader/SSBO.h"
#include "Molecule.h"


#define EXIT_WITH_ERROR(err) \
	std::cout << "ERROR: " << err << std::endl; \
	system("PAUSE"); \
	return EXIT_FAILURE;

/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void mouseKeyCallback(GLFWwindow* window, int button, int action, int mods);
static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
// static void perFrameUniforms(std::vector<std::shared_ptr<Shader>>& shaders, Camera& camera);

/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */
float _zoom = 8;
bool _dragging = false;
bool _strafing = false;
bool _wireframe = false;
bool _backFaceCulling = true;
float _slice = 0.5f;

/* --------------------------------------------- */
// Structs
/* --------------------------------------------- */
struct GridCell
{
	unsigned int count;
	unsigned int ids[31];
};

/* --------------------------------------------- */
// Main
/* --------------------------------------------- */

int main(int argc, char** argv)
{
	/* --------------------------------------------- */
	// Load settings.ini
	/* --------------------------------------------- */


	int window_width = 1920;
	int window_height = 1080;
	float fov = 60.0f;
	float nearZ = 0.1f;
	float farZ = 200.0f;
	int refreshRate = 120;
	std::string windowTitle = "SDF - Raymarcher";


	/* --------------------------------------------- */
	// Create Window Context
	/* --------------------------------------------- */

	if (!glfwInit()) {
		EXIT_WITH_ERROR("Failed to init glfw");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Request OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile													  
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Prevent window resizing because viewport would have to resize as well (-> not needed in this course)
	glfwWindowHint(GLFW_REFRESH_RATE, refreshRate);

	//Debug Context

#if _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, windowTitle.c_str(), nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		EXIT_WITH_ERROR("Failed to create window")
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	GLenum error = glewInit();

	if (error != GLEW_OK) {
		glfwTerminate();
		EXIT_WITH_ERROR("Failed to init glew");
	}

#if _DEBUG
	//Register Callback function
	glDebugMessageCallback(DebugCallbackDefault, NULL);
	//Synchronuous Callback - imidiatly after error
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	// Check needed extensions
	if(!GL_ARB_shader_group_vote)
	{
		EXIT_WITH_ERROR("Need extensions: GL_ARB_shader_group_vote")
	}

	//Set input callbacks
	glfwSetKeyCallback(window, keyCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetMouseButtonCallback(window, mouseKeyCallback);

	//Set GL defaults (color etc.)
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Depth Test
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */
	{

		//Shaders
		std::vector<std::shared_ptr<Shader>> shaders;
		std::shared_ptr<Shader> mainShader = std::make_shared<Shader>("base.vert","ses_raymarch.frag");
		shaders.push_back(mainShader);
		std::shared_ptr<Geometry> fullScreenQuad;
		std::shared_ptr<Shader> gridBuilder = std::make_shared<Shader>("compute_grid.glsl");
		std::shared_ptr<Shader> probeInterscetion = std::make_shared<Shader>("probe_intersection.glsl");
		std::shared_ptr<Shader> df_refinement = std::make_shared<Shader>("df_refinement.glsl");
		std::shared_ptr<Shader> texVis = std::make_shared<Shader>("fullScreenQuad.vert", "volTexInspector.frag");
		GeometryData quadGeom = ProceduralGeometry::createFullScreenQuad();
		Geometry* quad = new ProceduralGeometry(glm::mat4(1.0f), quadGeom, mainShader);

		//Camera
		Camera* camera = new Camera(fov, float(window_width) / float(window_height), nearZ, farZ);

		//Lights
		LightManager* lightManager = new LightManager();
		lightManager->createPointLight(glm::vec3(1.0f, 1.0f, 1.0f), 0.8f*glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.1f, 0.4f, 1.0f));
		lightManager->createDirectionalLight(glm::vec3(1.8f), glm::vec3(0.0f, -1.0f, -1.0f));



		double loadTime = glfwGetTime();
		PDB_Tests test("data/5xyu.cif");
		//std::vector<std::unique_ptr<Geometry>> atoms = test.doStuff();
		loadTime = glfwGetTime() - loadTime;
		std::cout << "time to load: " << loadTime << '\n';
		loadTime = glfwGetTime();
		Molecule molecule = test.doStuff();
		loadTime = glfwGetTime() - loadTime;
		std::cout << "Nr of Atoms: "<< molecule.atoms.size() << '\n';
		std::cout << "time to do stuff: " << loadTime << '\n';
		std::shared_ptr<Shader> shader = std::make_shared<Shader>("Phong.vert", "Phong.frag");
		GeometryData ball = ProceduralGeometry::createSphereGeometry(1.1f, 16u, 8u);
	
		std::shared_ptr<LambertMaterial> material = std::make_shared<LambertMaterial>(shader);
		material->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
		std::unique_ptr<Geometry> ballGeometry = std::make_unique<ProceduralGeometry>(glm::mat4(1.0f), ball, material);
		std::unique_ptr<MoleculeModel> atomModel = std::make_unique<MoleculeModel>(glm::scale(glm::mat4(1.0f),glm::vec3(0.5f,0.5f,0.5f)), material, molecule);
		shaders.push_back(shader);

		//SES calculations
		float probeRadius = 2.f;
		glm::uvec3 dimensions = glm::uvec3((molecule.max_pos - molecule.min_pos+glm::vec3(probeRadius)) / (molecule.max_radius + probeRadius));
		SSBO<Atom> atomSSBO(100000);
		loadTime = glfwGetTime();
		atomSSBO.uploadData(molecule.atoms);
		atomSSBO.bindToTarget(0);
		SSBO<GridCell> grid((dimensions.x + 1)*(dimensions.y + 1)*(dimensions.z + 1));
		grid.bindToTarget(1);
		gridBuilder->use();
		gridBuilder->setUniform("nrAtoms", (int)molecule.atoms.size());
		gridBuilder->setUniform("nr_cells", dimensions);
		gridBuilder->setUniform("max", molecule.max_pos + glm::vec3(probeRadius/2));
		gridBuilder->setUniform("min", molecule.min_pos - glm::vec3(probeRadius / 2));
		int number = (int)(molecule.atoms.size() / (1024)) + 1;
		glDispatchCompute(number, 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		probeInterscetion->use();
		probeInterscetion->setUniform("grid_max", molecule.max_pos + glm::vec3(probeRadius / 2));
		probeInterscetion->setUniform("grid_min", molecule.min_pos - glm::vec3(probeRadius / 2));
		probeInterscetion->setUniform("nr_cells", dimensions);
		probeInterscetion->setUniform("probeRadius", probeRadius);
		//Construct Texture
		glm::vec3 moleculeDimension = molecule.max_pos - molecule.min_pos + glm::vec3(probeRadius);
		float idealRadius = 0.5;
		glm::ivec3 idealTexSize = glm::ivec3(moleculeDimension / idealRadius);
		glm::vec3 resolutions = moleculeDimension / glm::vec3(idealTexSize);
		float maxResolution = std::max(resolutions.x, std::max(resolutions.y, resolutions.z));
		std::cout << "idealTexSize: " << idealTexSize.x << " \ " << idealTexSize.y << " \ " << idealTexSize.z << '\n';
		std::cout << "resolutions: " << resolutions.x << " \ " << resolutions.y << " \ " << resolutions.z << '\n';
		std::cout << number << '\n';
		std::cout << dimensions.x << " \ " << dimensions.y << " \ " << dimensions.z << " \n";

		glm::ivec3 dispatchSize = idealTexSize / glm::ivec3(8) + glm::ivec3(1);
		std::cout << "dispatch " <<dispatchSize.x << " \ " << dispatchSize.y << " \ " << dispatchSize.z << " \n";
		probeInterscetion->setUniform("texRadius", maxResolution);
		VolumetricTexture SESTexture(idealTexSize.x, idealTexSize.y, idealTexSize.z, GL_R32F, GL_RED,  GL_FLOAT);
		VolumetricTexture classification(idealTexSize.x, idealTexSize.y, idealTexSize.z, GL_R32F, GL_RED, GL_FLOAT);
		SESTexture.bindAsImage(0);
		classification.bindAsImage(1);
		glDispatchCompute(dispatchSize.x, dispatchSize.y, dispatchSize.z);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);


		df_refinement->use();
		df_refinement->setUniform("probeRadius", probeRadius);
		df_refinement->setUniform("texRadius", maxResolution);
		glDispatchCompute(dispatchSize.x, dispatchSize.y, dispatchSize.z);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		loadTime = glfwGetTime() - loadTime;
		std::cout << "Compute shader time: " << loadTime << '\n';

		//Variables
		double mouseX, mouseY;
		double thisFrameTime = 0, oldFrameTime = 0, deltaT = 0;
		double startTime = glfwGetTime();
		oldFrameTime = startTime;
		unsigned long framecounter = 0;
		while (!glfwWindowShouldClose(window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//timings
			++framecounter;
			thisFrameTime = glfwGetTime();
			deltaT = thisFrameTime - oldFrameTime;
			oldFrameTime = thisFrameTime;

			//Poll Input Events
			glfwPollEvents();
			glfwGetCursorPos(window, &mouseX, &mouseY);
			//update camera
			camera->update(mouseX, mouseY, _zoom, _dragging, _strafing);
			//update uniforms
			lightManager->setUniforms(shaders);
			mainShader->use();
			mainShader->setUniform("inversePVMatrix", camera->getInverseProjectionViewMatrix());
			mainShader->setUniform("cameraPosition", camera->getPosition());
			mainShader->setUniform("grid_min", molecule.min_pos - glm::vec3(probeRadius / 2));
			mainShader->setUniform("grid_max", molecule.max_pos + glm::vec3(probeRadius / 2));
			SESTexture.bindAsTexture(0);
			mainShader->setUniform("SESTexture", 0);
			quad->draw();
			//lightManager->setUniforms(shaders);
			//shader->use();
			//shader->setUniform("cameraPosition", camera->getPosition());
			//shader->setUniform("viewProjectionMatrix", camera->getProjectionViewMatrix());

			//atomModel->draw();
			//quad->draw();


			//Texture inspection
			//texVis->use();
			//SESTexture.bindAsTexture(0);
			//texVis->setUniform("slice", _slice);
			//texVis->setUniform("SESTexture", 0);
			//texVis->setUniform("probeRadius", probeRadius);
			//texVis->setUniform("texRadius", maxResolution);

			//quad->draw();
			//

			//Swap Buffers
			glfwSwapBuffers(window);
		}
		/* --------------------------------------------- */
		// Statistics
		/* --------------------------------------------- */
		double endTime = glfwGetTime();
		std::cout << "Program ran for " << endTime - startTime << "seconds." << std::endl;
		std::cout << "Average fps: " << framecounter / (endTime - startTime) << "." << std::endl;

		delete quad;
		delete camera;
		delete lightManager;
	}

	// Destroy context and exit
	/* --------------------------------------------- */

	glfwTerminate();

	return EXIT_SUCCESS;
}


static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action != GLFW_RELEASE) return;

	if (key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_F1)
	{
		_wireframe = !_wireframe;
		if (_wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	if (key == GLFW_KEY_F2)
	{
		_backFaceCulling = !_backFaceCulling;
		if (_backFaceCulling)
		{
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}
	if (key == GLFW_KEY_L)
	{
		_slice = std::min(_slice + 0.01f, 1.0f);
	}
	if (key == GLFW_KEY_J)
	{
		_slice = std::max(_slice - 0.01f, 0.0f);
	}
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	_zoom -= 0.4f*float(yoffset);
}

static void mouseKeyCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		_dragging = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		_dragging = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		_strafing = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		_strafing = false;
	}
}

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	if (id == 131185 || id == 131218) return; // ignore performance warnings (buffer uses GPU memory, shader recompilation) from nvidia
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	switch (source) {
	case GL_DEBUG_SOURCE_API: {
		sourceString = "API";
		break;
	}
	case GL_DEBUG_SOURCE_APPLICATION: {
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		sourceString = "Third Party";
		break;
	}
	case GL_DEBUG_SOURCE_OTHER: {
		sourceString = "Other";
		break;
	}
	default: {
		sourceString = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		typeString = "Error";
		break;
	}
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_TYPE_OTHER: {
		typeString = "Other";
		break;
	}
	default: {
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severityString = "Low";
		break;
	}
	default: {
		severityString = "Unknown";
		break;
	}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}
/*
void perFrameUniforms(std::vector<std::shared_ptr<Shader>>& shaders, Camera & camera)
{
	for (std::shared_ptr<Shader> shader : shaders) {
		shader->use();
		shader->setUniform("viewProjectionMatrix", camera.getViewProjectionMatrix());
		shader->setUniform("cameraPosition", camera.getPosition());
	}
}
*/
