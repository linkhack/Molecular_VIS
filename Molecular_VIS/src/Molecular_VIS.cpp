// SDF_Raymarcher.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <sstream>
#include <iostream>
#include <memory>
#include <string>
#include <algorithm>




#include "Shader/Shader.h"
#include "Geometry/Geometry.h"
#include "Camera/Camera.h"
#include "Light/LightManager.h"
#include "PDBLoader/PDBLoader.h"
#include "Material/LambertMaterial.h"
#include "Geometry/MoleculeModel.h"
#include "Texture/VolumetricTexture.h"
#include "Molecule/SESSurface.h"

#include "Shader/SSBO.h"
#include "Molecule/Molecule.h"

#include "Renderer/FBO.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "GLFW/glfw3.h"
#include "GL/glew.h"

#include "tinyfiledialogs.h"

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
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
static std::unique_ptr<SESSurface> loadModel(const char* path);
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
float subsurfaceDepth = 5.0f;
float dmax = 2.0f;
float refraction = 1.7f;
bool reflectionOn = true;
bool refractionOn = true;
bool translucencyOn = true;
std::unique_ptr<MoleculeModel> atomModel;
std::shared_ptr<Shader> shader;
std::vector<std::shared_ptr<Shader>> shaders;
/* --------------------------------------------- */
// Structs
/* --------------------------------------------- */

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

	//Set up IMGUI
	const char* glsl_version = "#version 430";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	
	//Set GL defaults (color etc.)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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
		shader = std::make_shared<Shader>("Phong.vert", "Phong.frag");
		std::shared_ptr<Shader> mainShader = std::make_shared<Shader>("base.vert","ses_raymarch.frag");
		shaders.push_back(mainShader);
		shaders.push_back(shader);
		std::shared_ptr<Geometry> fullScreenQuad;
		std::shared_ptr<Shader> texVis = std::make_shared<Shader>("fullScreenQuad.vert", "volTexInspector.frag");
		std::shared_ptr<Shader> tex = std::make_shared<Shader>("base.vert", "translucency.frag");
		GeometryData quadGeom = ProceduralGeometry::createFullScreenQuad();
		Geometry* quad = new ProceduralGeometry(glm::mat4(1.0f), quadGeom, mainShader);
		Geometry* translucencyQuad = new ProceduralGeometry(glm::mat4(1.0f), quadGeom, tex);

		//Camera
		Camera* camera = new Camera(fov, float(window_width) / float(window_height), nearZ, farZ);

		//Lights
		LightManager* lightManager = new LightManager();
		lightManager->createPointLight(glm::vec3(1.0f, 1.0f, 1.0f), 0.8f*glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.1f, 0.4f, 1.0f));
		lightManager->createDirectionalLight(glm::vec3(1.8f), glm::vec3(0.0f, -1.0f, -1.0f));
		
		std::unique_ptr<SESSurface> surfaceRepresentation = loadModel("data/1af6.cif");		
		
		FBO atomFbo((unsigned int)1, true, false, window_width, window_height);
		FBO SESFbo((unsigned int)3, false, false, window_width, window_height);		

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

			atomFbo.setActive();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader->use();
			shader->setUniform("cameraPosition", camera->getPosition());
			shader->setUniform("viewProjectionMatrix", camera->getProjectionViewMatrix());
			atomModel->draw();
			
	
			//lightManager->setUniforms(shaders);
			//SESFbo.setActive();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			mainShader->use();
			mainShader->setUniform("inversePVMatrix", camera->getInverseProjectionViewMatrix());
			mainShader->setUniform("viewProjectionMatrix", camera->getProjectionViewMatrix());
			mainShader->setUniform("cameraPosition", camera->getPosition());
			mainShader->setUniform("cameraDirection", camera->getDirection());
			mainShader->setUniform("grid_min", surfaceRepresentation->getTexMin());
			mainShader->setUniform("grid_max", surfaceRepresentation->getTexMax());
			mainShader->setUniform("dmax", dmax);
			mainShader->setUniform("subsurfaceDepth", subsurfaceDepth);
			mainShader->setUniform("refraction", refraction);
			mainShader->setUniform("refractionOn", refractionOn);
			mainShader->setUniform("reflectionOn", reflectionOn);
			mainShader->setUniform("translucencyOn", translucencyOn);
			mainShader->setUniform("near", nearZ);
			mainShader->setUniform("far", farZ);
			surfaceRepresentation->bindToUnit(0);
			mainShader->setUniform("SESTexture", 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, atomFbo.getColorTexture(0));
			mainShader->setUniform("AtomTexture", 1);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, atomFbo.getDepthTexture());
			mainShader->setUniform("AtomDepth", 2);
			
			quad->draw();
			
			

			

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

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::Begin("Options");

			ImGui::SliderFloat("Dmax", &dmax, 0.00f, 5.0f);
			ImGui::SliderFloat("Subsurface Depth", &subsurfaceDepth, 0.50f, 15.0f);
			ImGui::Checkbox("Reflection On",&reflectionOn);
			ImGui::Checkbox("Refraction On", &refractionOn);
			ImGui::SliderFloat("Refraction", &refraction, 1.00f, 2.0f);
			
			ImGui::Checkbox("Translucency On", &translucencyOn);
			if (ImGui::Button("Open"))
			{
				char const* lFilterPatterns[1] = { "*.cif" };
				char const* path;
				path = tinyfd_openFileDialog(
					"Open Molecule",
					"",
					1,
					lFilterPatterns,
					NULL,
					0);
				surfaceRepresentation = loadModel(path);
			}
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();

			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

std::unique_ptr<SESSurface> loadModel(const char* path)
{
	PDBLoader test(path);
	Molecule molecule = test.getMolecule();
	std::cout << "Nr of Atoms: " << molecule.atoms.size() << '\n';

	std::shared_ptr<LambertMaterial> material = std::make_shared<LambertMaterial>(shader);
	material->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
	atomModel = std::make_unique<MoleculeModel>(glm::mat4(1.0f), material, molecule);

	//SES calculations
	float probeRadius = 1.7f;
	auto surfaceRepresentation = std::make_unique<SESSurface>(molecule, probeRadius);

	return std::move(surfaceRepresentation);
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
