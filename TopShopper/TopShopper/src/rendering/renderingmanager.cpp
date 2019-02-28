#include "renderingmanager.h"
#include "core/broker.h"
#include "loading/loadingmanager.h"
#include <iostream>
#include <string>
#include "ShaderTools.h"

#include "PxRigidDynamic.h"

//**Must include glad and GLFW in this order or it breaks**
#include <glad/glad.h>
#include <GLFW/glfw3.h>


using namespace physx;


RenderingManager::RenderingManager(Broker *broker)
	: _broker(broker)
{
	
}

RenderingManager::~RenderingManager() {

}


void RenderingManager::init() {
	openWindow();
	glEnable(GL_DEPTH_TEST);
	shaderProgram = ShaderTools::InitializeShaders();
	if (shaderProgram == 0) {
		std::cout << "Program could not initialize shaders, TERMINATING" << std::endl;
		return;
	}
}


void RenderingManager::RenderScene(std::vector<Geometry>& objects) {
	//Clears the screen to a dark grey background
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(shaderProgram);

	float fov = 60.0f;
	int width;
	int height;
	glfwGetWindowSize(_window, &width, &height);

	glm::mat4 Projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 1000.0f);

	// TODO: fix the follow camera to lag behind the player, don't be blocked by walls and don't spin so much, and be panned with right stick (this changes where it is looking at)
	std::shared_ptr<ShoppingCartPlayer> player = _broker->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers().at(0);
	PxRigidDynamic* playerDyn = player->_actor->is<PxRigidDynamic>();
	PxTransform playerTransform = playerDyn->getGlobalPose();
	PxVec3 playerPos = playerTransform.p;
	PxQuat playerRot = playerTransform.q;

	PxVec3 testVec(0, 20, -30);
	testVec = playerRot.rotate(testVec);
	glm::vec3 cameraPosition = glm::vec3(playerPos.x + testVec.x, playerPos.y + testVec.y, playerPos.z + testVec.z);
	glm::mat4 View = glm::lookAt(
		glm::vec3(cameraPosition), // camera position
		glm::vec3(playerPos.x, playerPos.y, playerPos.z), // looks at 
		glm::vec3(0, 1, 0)  // up vector
	);

	//glm::mat4 Model = glm::mat4(1.0f);
	

	GLuint ModelID = glGetUniformLocation(shaderProgram, "Model");
	GLuint ViewID = glGetUniformLocation(shaderProgram, "View");
	GLuint ProjectionID = glGetUniformLocation(shaderProgram, "Projection");
	GLuint colorID = glGetUniformLocation(shaderProgram, "ColorMeme");
	GLuint cameraID = glGetUniformLocation(shaderProgram, "CameraPos");

	


	for (Geometry& g : objects) {
		
		glUniform3f(cameraID, cameraPosition.x, cameraPosition.y, cameraPosition.z);
		glUniform3f(colorID, g.color.x, g.color.y, g.color.z);
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &g.model[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &Projection[0][0]);

		glBindVertexArray(g.vao);
		assignBuffers(g);
		setBufferData(g);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g.indexBuffer);
		//glDrawElements(GL_TRIANGLES,g.vIndex.size(), GL_UNSIGNED_INT, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, g.verts.size());


		glBindVertexArray(0);
	}
	glUseProgram(0); 


	CheckGLErrors();
}



void RenderingManager::assignBuffers(Geometry& geometry) {
	//Generate vao for the object
	//Constant 1 means 1 vao is being generated
	glGenVertexArrays(1, &geometry.vao);
	glBindVertexArray(geometry.vao);

	//Generate vbos for the object
	//Constant 1 means 1 vbo is being generated
	glGenBuffers(1, &geometry.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &geometry.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.colorBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &geometry.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.normalBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);




}

void RenderingManager::setBufferData(Geometry& geometry) {
	//Send geometry to the GPU
	//Must be called whenever anything is updated about the object
	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * geometry.verts.size(), geometry.verts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, geometry.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * geometry.normals.size(), geometry.normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, geometry.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * geometry.colors.size(), geometry.colors.data(), GL_STATIC_DRAW);


	/*
	glGenBuffers(1, &geometry.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * geometry.vIndex.size(), geometry.vIndex.data(), GL_STATIC_DRAW);
	*/
}

void RenderingManager::deleteBufferData(Geometry& geometry) {
	glDeleteBuffers(1, &geometry.vertexBuffer);
	glDeleteBuffers(1, &geometry.normalBuffer);
	glDeleteBuffers(1, &geometry.colorBuffer);
	//glDeleteBuffers(1, &geometry.indexBuffer);
	glDeleteVertexArrays(1, &geometry.vao);
}


bool RenderingManager::CheckGLErrors() {
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		std::cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			std::cout << "GL_INVALID_ENUM" << std::endl; break;
		case GL_INVALID_VALUE:
			std::cout << "GL_INVALID_VALUE" << std::endl; break;
		case GL_INVALID_OPERATION:
			std::cout << "GL_INVALID_OPERATION" << std::endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl; break;
		case GL_OUT_OF_MEMORY:
			std::cout << "GL_OUT_OF_MEMORY" << std::endl; break;
		default:
			std::cout << "[unknown error code]" << std::endl;
		}
		error = true;
	}
	return error;
}


GLFWwindow* RenderingManager::getWindow() {
	return _window;
}




void RenderingManager::updateSeconds(double variableDeltaTime) {
	// call LATEUPDATE() for all behaviour scripts...
	for (std::shared_ptr<Entity> &entity : _broker->getPhysicsManager()->getActiveScene()->_entities) {
		std::shared_ptr<Component> comp = entity->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
		if (comp != nullptr) {
			std::shared_ptr<BehaviourScript> script = std::static_pointer_cast<BehaviourScript>(comp);
			script->lateUpdate(variableDeltaTime);
		}
	}


	for (Geometry& geoDel : _objects) {
		deleteBufferData(geoDel);
	}

	_objects.clear();

	push3DObjects();

	
	glfwSwapBuffers(_window);
}

void RenderingManager::push3DObjects() {
	for (const std::shared_ptr<Entity> &entity : _broker->getPhysicsManager()->getActiveScene()->_entities) {
		PxRigidActor *actor = entity->_actor->is<PxRigidActor>();
		PxTransform transform = actor->getGlobalPose();
		PxVec3 pos = transform.p;
		const PxQuat rot = transform.q;
		EntityTypes tag = entity->getTag();

		Geometry geo;

		switch (tag) {
		case EntityTypes::SHOPPING_CART_PLAYER:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::VEHICLE_CHASSIS_GEO_NO_INDEX));
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::VEHICLE_CHASSIS_GEO));
			geo.color = glm::vec3(0.2f, 0.65f, 0.95f);
			break;
		}

		case EntityTypes::GROUND:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::GROUND_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::GROUND_GEO));
			geo.color = glm::vec3(0.5f, 0.5f, 0.5f);
			break;
		}

		case EntityTypes::MILK:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			geo.color = glm::vec3(0.9f, 0.9f, 0.9f);
			break;
		}
		case EntityTypes::WATER:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			geo.color = glm::vec3(0.0f, 0.25f, 0.9f);
			break;
		}
		case EntityTypes::COLA:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			geo.color = glm::vec3(0.45f, 0.25f, 0.1f);
			break;
		}
		case EntityTypes::APPLE:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			geo.color = glm::vec3(0.95f, 0.15f, 0.2f);
			break;
		}
		case EntityTypes::WATERMELON:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			geo.color = glm::vec3(0.95f, 0.15f, 0.85f);
			break;
		}
		case EntityTypes::BANANA:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			geo.color = glm::vec3(0.95f, 0.85f, 0.15f);
			break;
		}
		case EntityTypes::CARROT:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			geo.color = glm::vec3(0.95f, 0.35f, 0.0f);
			break;
		}
		case EntityTypes::EGGPLANT:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			geo.color = glm::vec3(0.45f, 0.0f, 0.95f);
			break;
		}

		case EntityTypes::BROCCOLI:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			geo.color = glm::vec3(0.05f, 0.5f, 0.2f);
			break;
		}

		case EntityTypes::SPARE_CHANGE:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX));
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			geo.color = glm::vec3(0.95f, 0.65f, 0.2f);
			break;
		}




		default:
			continue;
		}


		glm::mat4 model;


		PxMat44 rotation = PxMat44(rot);
		PxMat44 translation = PxMat44(PxMat33(PxIdentity), pos);
		PxMat44	pxModel = translation * rotation;

		model = glm::mat4(glm::vec4(pxModel.column0.x, pxModel.column0.y, pxModel.column0.z, pxModel.column0.w),
			glm::vec4(pxModel.column1.x, pxModel.column1.y, pxModel.column1.z, pxModel.column1.w),
			glm::vec4(pxModel.column2.x, pxModel.column2.y, pxModel.column2.z, pxModel.column2.w),
			glm::vec4(pxModel.column3.x, pxModel.column3.y, pxModel.column3.z, pxModel.column3.w));

		geo.model = model;


		geo.drawMode = GL_TRIANGLES;
		_objects.push_back(geo);
	}

	RenderScene(_objects);
}

void RenderingManager::cleanup() {
	glfwTerminate();
}



void RenderingManager::openWindow() {

	//Initialize the GLFW windowing system
	if (!glfwInit()) {
		std::cout << "ERROR: GLFW failed to initialize, TERMINATING" << std::endl;
		return;
	}

	//Attempt to create a window with an OpenGL 4.1 core profile context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	int width = 1920;
	int height = 1080;
	_window = glfwCreateWindow(width, height, "Top Shopper", 0, 0);
	if (!_window) {
		std::cout << "Program failed to create GLFW window, TERMINATING" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(_window);
	glfwSetWindowUserPointer(_window, this);

	if (!gladLoadGL()) {
		std::cout << "GLAD init failed" << std::endl;
		return;
	}

	QueryGLVersion();
}

void RenderingManager::QueryGLVersion() {
	// query opengl version and renderer information
	std::string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	std::string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	std::string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	std::cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << std::endl;
}

void ErrorCallback(int error, const char* description) {
	std::cout << "GLFW ERROR " << error << ":" << std::endl;
	std::cout << description << std::endl;
}

