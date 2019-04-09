#include "renderingmanager.h"
#include "core/broker.h"
#include "loading/loadingmanager.h"
#include <iostream>
#include <string>
#include <map>
#include "ShaderTools.h"
#include "PxRigidDynamic.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "vehicle/VehicleShoppingCart.h"
#include <deque>

using namespace physx;

// TODO: reset this map on game load...
std::map<int, std::deque<float>> gVehicleThetasMap;
std::map<int, std::deque<float>> gRightStickXValuesMap;

//std::deque<float> gVehicleThetas = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }; // I WILL ENFORCE THIS TO BE A FIXED SIZE OF 10 (for now, holds the last 10 frames worth of VEHICLE ROTATIONS)
// TODO: change this to a map and reset similiar to gVehicleThetasMap
//std::deque<float> gRightStickXValues = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; // always maintain a fixed size (here I use 10) 

RenderingManager::RenderingManager(Broker *broker)
	: _broker(broker)
{

}

RenderingManager::~RenderingManager() {

}


//Initializes the Rendering Manager by compiling all shaders and textures while also setting the inital state of openGL
//Called upon the start of the game, to set the state of OpenGL and all subsequent rendering passes
void RenderingManager::init() {
	openWindow();
	initTextRender();

	glfwGetWindowSize(_window, &windowWidth, &windowHeight);

	glEnable(GL_DEPTH_TEST);
	shaderProgram = ShaderTools::InitializeShaders(std::string("vertex"), std::string("fragment"));
	if (shaderProgram == 0) {
		std::cout << "Program could not initialize shaders, TERMINATING" << std::endl;
		return;
	}
	textShaderProgram = ShaderTools::InitializeShaders(std::string("vertexText"), std::string("fragmentText"));
	if (textShaderProgram == 0) {
		std::cout << "Program could not initialize shaders, TERMINATING" << std::endl;
		return;
	}
	spriteShaderProgram = ShaderTools::InitializeShaders(std::string("vertexSprite"), std::string("fragmentSprite"));
	if (spriteShaderProgram == 0) {
		std::cout << "Program could not initialize shaders, TERMINATING" << std::endl;
		return;
	}
	depthBufferShaderProgram = ShaderTools::InitializeShaders(std::string("depthBufferVertex"), std::string("depthBufferFragment"));
	if (depthBufferShaderProgram == 0) {
		std::cout << "Program could not initialize shaders, TERMINATING" << std::endl;
		return;
	}

	quadTestShaderProgram = ShaderTools::InitializeShaders(std::string("quadTestVertex"), std::string("quadTestFragment"));
	if (quadTestShaderProgram == 0) {
		std::cout << "Program could not initialize shaders, TERMINATING" << std::endl;
		return;
	}

	gradientShaderProgram = ShaderTools::InitializeShaders(std::string("gradientVertex"), std::string("gradientFragment"));
	if (gradientShaderProgram == 0) {
		std::cout << "Program could not initialize shaders, TERMINATING" << std::endl;
		return;
	}

	transparencyShaderProgram = ShaderTools::InitializeShaders(std::string("transparencyShaderVertex"), std::string("transparencyShaderFragment"));
	if (transparencyShaderProgram == 0) {
		std::cout << "Program could not initialize shaders, TERMINATING" << std::endl;
		return;
	}

	glfwGetWindowSize(_window, &windowWidth, &windowHeight);


	glUseProgram(shaderProgram);

	/*TODO: Should do this in loading manager or a texture manager class*/
	init3DTextures();
	initSpriteTextures();
	initFrameBuffers();
}



//UpdateSeconds called every frame from the main loop
//handles the deletion of objects after completing the rendering of each frame as well as the updating of model positions
//calls render scene after pushing back the 3d objects in order to render the scene again. 
void RenderingManager::updateSeconds(double variableDeltaTime) {
	glfwGetWindowSize(_window, &windowWidth, &windowHeight);
	if (_broker->_scene == GAME) {
		// call LATEUPDATE() for all behaviour scripts...
		std::vector<std::shared_ptr<Entity>> entitiesCopy = _broker->getPhysicsManager()->getActiveScene()->_entities;
		for (std::shared_ptr<Entity> &entity : entitiesCopy) {
			std::shared_ptr<Component> comp = entity->getComponent(ComponentTypes::BEHAVIOUR_SCRIPT);
			if (comp != nullptr) {
				std::shared_ptr<BehaviourScript> script = std::static_pointer_cast<BehaviourScript>(comp);
				script->lateUpdate(variableDeltaTime);
			}
		}
	}

	for (Geometry& geoDel : _objects) {
		deleteBufferData(geoDel);
	}
	_objects.clear();


	if (_broker->_scene == GAME || _broker->_scene == PAUSED || _broker->_scene == END_SCREEN) {
		push3DObjects();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (_broker->_nbPlayers == 1) {
			RenderGameScene(0, 0, 0, windowWidth, windowHeight);
		}
		else {
			RenderGameScene(0, 0, windowHeight / 2, windowWidth / 2, windowHeight / 2);						//split screen rendering
			RenderGameScene(1, windowWidth / 2, windowHeight / 2, windowWidth / 2, windowHeight / 2);
			RenderGameScene(2, 0, 0, windowWidth / 2, windowHeight / 2);
		}

	}
	else if (_broker->_scene == MAIN_MENU) {
		RenderMainMenu();
	}
	else if (_broker->_scene == LOADING) {
		// NOTE: these starting values are in [-pi, pi], based on the starting rotation of each cart...
		gVehicleThetasMap[0] = { 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f };
		gVehicleThetasMap[1] = { 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f, 1.5708f };
		gVehicleThetasMap[2] = { -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f };
		gVehicleThetasMap[3] = { -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f, -2.6180f };
		gVehicleThetasMap[4] = { -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f };
		gVehicleThetasMap[5] = { -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f, -0.5236f };

		gRightStickXValuesMap[0] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		gRightStickXValuesMap[1] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		gRightStickXValuesMap[2] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		gRightStickXValuesMap[3] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		gRightStickXValuesMap[4] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		gRightStickXValuesMap[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		RenderLoading();
	}
	else if (_broker->_scene == SETUP) {
		RenderSetup();
	}
	else if (_broker->_scene == CREDITS) {
		RenderCredits();
	}
	else if (_broker->_scene == CONTROLS) {
		RenderControls();
	}
	
	glfwSwapBuffers(_window);
}


//RenderScene utilizes the current array of objects in the rendering manager, setting and assigning the buffers for each geometry,
//then sending the vertex info down the openGL pipeline, while utilizing the approprite shaders tied to the geometry.
//performs multiple rendering passes in order to create shadowsm, while calculating the camera information each time it is called.
void RenderingManager::RenderGameScene(int playerID, int viewBottomLeftx, int viewBottomLeftY, int viewTopRightX, int viewTopRightY){  
	//Clears the screen to a light grey background
	glClearColor(0.639f, 0.701f, 0.780f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);

	float fov = 65.0f;
	glfwGetWindowSize(_window, &windowWidth, &windowHeight);
	glm::mat4 Projection = glm::perspective(glm::radians(fov), (float)windowWidth / (float)windowHeight, 1.0f, 800.0f);

	glm::vec3 cameraPos;
	glm::mat4 View = computeCameraPosition(playerID, cameraPos);	//compute the cameraPosition and view matrix for player 0
	glm::mat4 lightProjection = glm::ortho(-270.0f, 270.0f, -270.0f, 270.0f, 1.0f, 500.0f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(70.0f, 200.0f, 0.0f), glm::vec3(0.1f, 15.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glViewport(0, 0, (GLuint)SHADOW_MAP_WIDTH, (GLuint)SHADOW_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, _lightDepthFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	//render the scene from the light and fill the depth buffer for shadows
	for (Geometry& g : _objects) {
		if (g.player != playerID && g.pointer) {
			continue;
		}

		glUseProgram(depthBufferShaderProgram);

		glUniformMatrix4fv(glGetUniformLocation(depthBufferShaderProgram, "Model"), 1, GL_FALSE, &g.model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(depthBufferShaderProgram, "View"), 1, GL_FALSE, &lightView[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(depthBufferShaderProgram, "Projection"), 1, GL_FALSE, &lightProjection[0][0]);

		glBindVertexArray(g.vao);
		glDrawArrays(GL_TRIANGLES, 0, g.verts.size());	//ignore the roof in the shadow map
		glBindVertexArray(0);
	}

	glViewport((GLuint)viewBottomLeftx, (GLuint)viewBottomLeftY, (GLuint)viewTopRightX, (GLuint)viewTopRightY);	//reset the viewport to the full window to render from the camera pov
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Geometry& g : _objects) {
		if (g.player != playerID && g.pointer) {
			continue;
		}

		if (g.cullBackFace) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
		if (g.gradientShader) {
			glUseProgram(gradientShaderProgram);
			glUniform3f(glGetUniformLocation(gradientShaderProgram, "CameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
			glUniformMatrix4fv(glGetUniformLocation(gradientShaderProgram, "Model"), 1, GL_FALSE, &g.model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(gradientShaderProgram, "View"), 1, GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(gradientShaderProgram, "Projection"), 1, GL_FALSE, &Projection[0][0]);
			glUniform1f(glGetUniformLocation(gradientShaderProgram, "gradientDegree"), _gradientDegree);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(g.texture.target, g.texture.textureID);
			GLuint imageTexUniLocation = glGetUniformLocation(gradientShaderProgram, "imageTexture");	//pass the geometry texture into the fragment shader
			glUniform1i(imageTexUniLocation, 0);
		}

		else if (g.isTransparent) {
			glUseProgram(transparencyShaderProgram);
			glUniform3f(glGetUniformLocation(transparencyShaderProgram, "CameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
			glUniformMatrix4fv(glGetUniformLocation(transparencyShaderProgram, "Model"), 1, GL_FALSE, &g.model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(transparencyShaderProgram, "View"), 1, GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(transparencyShaderProgram, "Projection"), 1, GL_FALSE, &Projection[0][0]);
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(g.texture.target, g.texture.textureID);
			GLuint imageTexUniLocation = glGetUniformLocation(transparencyShaderProgram, "imageTexture");	//pass the geometry texture into the fragment shader
			glUniform1i(imageTexUniLocation, 0);
		}
				
		else{
			glUseProgram(shaderProgram);					//use the default shader program

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(g.texture.target, g.texture.textureID);					//pass the geometry texture into the fragment shader
			glUniform1i(glGetUniformLocation(shaderProgram, "imageTexture"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, (GLuint)_depthMapTex);						//pass the shadow map into the fragment shader
			glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 1);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "LightView"), 1, GL_FALSE, &lightView[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "LightProjection"), 1, GL_FALSE, &lightProjection[0][0]);

			glUniform3f(glGetUniformLocation(shaderProgram, "CameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Model"), 1, GL_FALSE, &g.model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "View"), 1, GL_FALSE, &View[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Projection"), 1, GL_FALSE, &Projection[0][0]);
		}
		glBindVertexArray(g.vao);
		glDrawArrays(GL_TRIANGLES, 0, g.verts.size());
		glUseProgram(0);
		glBindVertexArray(0);
	}
	if (_broker->_scene == GAME) {
		renderHud(playerID);
	}
	else if (_broker->_scene == END_SCREEN) {
		renderEndScreen();
	}
	else if (_broker->_scene == PAUSED) {
		renderPauseScreen();
	}
	CheckGLErrors();
}



//Computes the cameraPosition of an  particular player in the game (represented by their player id) using an averaging technique to smooth the camera
//returns a mat4 representing the lookat matrix of the camera for the input player to be used to represent where the scene will be rendered from
// NOTE: I'm assuming that this function only gets called for human players (which will be upto [0], [1], [2], [3] in the vector and thus their inputID = playerID+1)
glm::mat4 RenderingManager::computeCameraPosition(int playerID, glm::vec3 &camera) {

	std::shared_ptr<ShoppingCartPlayer> player = _broker->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers().at(playerID);
	PxRigidDynamic* playerDyn = player->_actor->is<PxRigidDynamic>();
	PxTransform playerTransform = playerDyn->getGlobalPose();
	PxVec3 playerPos = playerTransform.p;
	PxQuat playerRot = playerTransform.q;

	PxVec3 forward(0.0f, 0.0f, 1.0f);
	forward = playerRot.rotate(forward);
	PxVec3 forwardNoYNorm = PxVec3(forward.x, 0.0f, forward.z).getNormalized();
	float theta = acos(forwardNoYNorm.dot(PxVec3(0.0f, 0.0f, 1.0f))); // theta in [0, pi]

	// now use cross product to figure out which side (sign of theta)...
	// +ve theta if CCW, -ve theta if CW

	PxVec3 crossprod = forwardNoYNorm.cross(PxVec3(0.0f, 0.0f, 1.0f));
	bool isCCW = crossprod.y <= 0.0f;
	if (!isCCW) theta *= -1;

	// now theta in [-pi, 0] or [0, pi]
	
	gVehicleThetasMap[playerID].pop_front(); // pop the oldest frame
	gVehicleThetasMap[playerID].push_back(theta); // push current vehicle rotation (around +y-axis)

	// get the average vehicle rotation in last 10 frames (incl. this one) 
	// have to use vectors since thetas have edge case problems (e.g. going from pi to -pi)
	PxVec3 vehicleRotationVecSum(0.0f, 0.0f, 0.0f);
	const std::deque<float> &gVehicleThetas = gVehicleThetasMap[playerID];

	for (float t : gVehicleThetas) {
		// assume a radius of 1 for this calculation (scale later by our intended radius)
		// also assume a y of 0.0f (can set y later)
		vehicleRotationVecSum += PxVec3(sin(t), 0.0f, cos(t));
	}
	vehicleRotationVecSum /= gVehicleThetas.size();

	// NOTE: I could change this to perform the glm::mix(t=0.5f) over all vectors, but the current solution seems to work
	
	// RIGHT STICK PANNING (NOTE: there is no keyboard equivalent for now, would probably need to use mouse, or just leave it out)...
	PxVec3 finalCamVec;
	int inputID = playerID + 1;
	if (_broker->getInputManager()->getGamePad(inputID) != nullptr && _broker->_scene == GAME) {
		float newRightStickXValue = _broker->getInputManager()->getGamePad(inputID)->rightStickX;
		gRightStickXValuesMap[playerID].pop_front();
		gRightStickXValuesMap[playerID].push_back(newRightStickXValue);
		float avgRightStickXValue = 0.0f;
		const std::deque<float> &gRightStickXValues = gRightStickXValuesMap[playerID];

		for (float f : gRightStickXValues) {
			avgRightStickXValue += f;
		}
		avgRightStickXValue /= gRightStickXValues.size();

		float rightStickTheta = avgRightStickXValue *-1 * 1.0472f; // 60deg (1.0472 rads) max
		PxMat33 rightStickMat = PxMat33(PxVec3(cos(rightStickTheta), 0, -sin(rightStickTheta)), PxVec3(0, 1, 0), PxVec3(sin(rightStickTheta), 0, cos(rightStickTheta))); // rotation matrix around y-axis
		finalCamVec = rightStickMat * vehicleRotationVecSum;
	}
	else {
		finalCamVec = vehicleRotationVecSum;
	}

	float radius = 30.0f; // FIXED (for now)
	float camX = -1 * radius * finalCamVec.x;
	float camY = 12.5f;
	float camZ = -1 * radius * finalCamVec.z;
	

	glm::vec3 camPos(playerPos.x + camX, playerPos.y + camY, playerPos.z + camZ);

	glm::mat4 View = glm::lookAt(
		camPos, // camera position
		glm::vec3(playerPos.x, playerPos.y, playerPos.z), // looks at 
		glm::vec3(0.0f, 1.0f, 0.0f)  // up vector
	);

	camera = camPos;
	return View;
}



void RenderingManager::renderEndScreen() {
	std::vector<std::shared_ptr<ShoppingCartPlayer>> players = _broker->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
	std::shared_ptr<ShoppingCartPlayer> player = players[0];
	std::shared_ptr<PlayerScript> script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));

	Player scores[6];

	scores[0].score = script->_points;
	scores[0].player = "Player1 ";

	player = players[1];
	script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));
	scores[1].score = script->_points;
	scores[1].player = "Player2 "; // Check if its a human or cpu

	
	player = players[2];
	script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));
	scores[2].score = script->_points;
	scores[2].player = "Player3 "; // Check if its a human or cpu
	
	player = players[3];
	script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));
	scores[3].score = script->_points;
	scores[3].player = "Player4 "; // Check if its a human or cpu

	player = players[4];
	script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));
	scores[4].score = script->_points;
	scores[4].player = "Player5 ";

	player = players[5];
	script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));
	scores[5].score = script->_points;
	scores[5].player = "Player6 ";

	std::sort(scores, scores+6, compareStruct1);
	
	
	//renderText("Shopper Ranks", windowWidth*0.35, windowHeight*0.63, 1.7f, glm::vec3(0.0f, 0.0f, 0.0f));

	renderText("1st: " + scores[0].player + std::to_string(scores[0].score), windowWidth*0.35, windowHeight*0.55, 1.5f, glm::vec3(0.83f, 0.69f, 0.22f));

	renderText("2nd: " + scores[1].player + std::to_string(scores[1].score), windowWidth*0.35, windowHeight*0.46, 1.5f, glm::vec3(0.65f, 0.65f, 0.65f));

	renderText("3rd: " + scores[2].player + std::to_string(scores[2].score), windowWidth*0.35, windowHeight*0.38, 1.5f, glm::vec3(0.70f, 0.36f, 0.0f));

	renderText("4th: " + scores[3].player + std::to_string(scores[3].score), windowWidth*0.35, windowHeight*0.33, 1.0f, glm::vec3(0, 0, 0));

	renderText("5th: " + scores[4].player + std::to_string(scores[4].score), windowWidth*0.35, windowHeight*0.29, 1.0f, glm::vec3(0, 0, 0));

	renderText("6th: " + scores[5].player + std::to_string(scores[5].score), windowWidth*0.35, windowHeight*0.25, 1.0f, glm::vec3(0, 0, 0));

	renderText("Menu", GLfloat(windowWidth*0.466), GLfloat(windowHeight*0.106), 1.0f, glm::vec3(0, 0, 0));
	renderSprite(*_buttonHighlightSprite, -0.15, -0.85, 0.15, -0.65);
	renderSprite(*_resultsScreenSprite, -0.5, -0.77, 0.5, 1);
}



void RenderingManager::RenderMainMenu() {
	//Clears the screen to a light grey background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.639f, 0.701f, 0.780f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	glfwGetWindowSize(_window, &windowWidth, &windowHeight);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, (GLuint)windowWidth, (GLuint)windowHeight);	//reset the viewport to the full window to render from the camera pov

	renderText("Start", GLfloat(windowWidth*0.47), GLfloat(windowHeight* 0.4768), 1.0f, glm::vec3(0, 0, 0));
	renderText("Controls", GLfloat(windowWidth*0.45), GLfloat(windowHeight* 0.3564), 1.0f, glm::vec3(0, 0, 0));
	renderText("Credits", GLfloat(windowWidth*0.455), GLfloat(windowHeight* 0.2314), 1.0f, glm::vec3(0, 0, 0));
	renderText("Quit", GLfloat(windowWidth*0.47), GLfloat(windowHeight* 0.1064), 1.0f, glm::vec3(0, 0, 0));

	switch (_broker->_cursorPositionStart) {
	case (0):// Quit highlighted
		renderSprite(*_buttonSprite, -0.15, -0.1, 0.15, 0.1);
		renderSprite(*_buttonSprite, -0.15, -0.35, 0.15, -0.15);
		renderSprite(*_buttonSprite, -0.15, -0.6, 0.15, -0.4);
		renderSprite(*_buttonHighlightSprite, -0.15, -0.85, 0.15, -0.65);
		break;
	case(1):
		renderSprite(*_buttonSprite, -0.15, -0.1, 0.15, 0.1);
		renderSprite(*_buttonSprite, -0.15, -0.35, 0.15, -0.15);
		renderSprite(*_buttonHighlightSprite, -0.15, -0.6, 0.15, -0.4);
		renderSprite(*_buttonSprite, -0.15, -0.85, 0.15, -0.65);
		break;
	case(2):
		renderSprite(*_buttonSprite, -0.15, -0.1, 0.15, 0.1);
		renderSprite(*_buttonHighlightSprite, -0.15, -0.35, 0.15, -0.15);
		renderSprite(*_buttonSprite, -0.15, -0.6, 0.15, -0.4);
		renderSprite(*_buttonSprite, -0.15, -0.85, 0.15, -0.65);
		break;
	case(3):
		renderSprite(*_buttonHighlightSprite, -0.15, -0.1, 0.15, 0.1);
		renderSprite(*_buttonSprite, -0.15, -0.35, 0.15, -0.15);
		renderSprite(*_buttonSprite, -0.15, -0.6, 0.15, -0.4);
		renderSprite(*_buttonSprite, -0.15, -0.85, 0.15, -0.65);
		break;
	}

	renderSprite(*_titleScreenSprite, -1, -1, 1, 1);

	CheckGLErrors();
}

void RenderingManager::RenderLoading() {
	//Clears the screen to a light grey background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.639f, 0.701f, 0.780f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	glfwGetWindowSize(_window, &windowWidth, &windowHeight);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, (GLuint)windowWidth, (GLuint)windowHeight);	//reset the viewport to the full window to render from the camera pov

	renderText("Loading.. Press A", GLfloat(windowWidth*0.4192), GLfloat(windowHeight* 0.4768), 1.0f, glm::vec3(1, 0, 1));
	renderSprite(*_backgroundSprite, -1, -1, 1, 1);

	CheckGLErrors();
}

void RenderingManager::RenderSetup() {
	//Clears the screen to a light grey background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.639f, 0.701f, 0.780f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	glfwGetWindowSize(_window, &windowWidth, &windowHeight);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, (GLuint)windowWidth, (GLuint)windowHeight);	//reset the viewport to the full window to render from the camera pov
	if (_broker->_cursorPositionSetup == 2) {
		renderText("Map < name >", GLfloat(windowWidth * 0.4192), GLfloat(windowHeight* 0.4768), 1.0f, glm::vec3(1, 0, 1));
	}
	else {
		renderText("Map < name >", GLfloat(windowWidth * 0.4192), GLfloat(windowHeight* 0.4768), 1.0f, glm::vec3(0, 0, 0));
	}
	if (_broker->_cursorPositionSetup == 1) {
		renderText("Number of Human Players: " + std::to_string(_broker->_nbPlayers), GLfloat(windowWidth* 0.3385), GLfloat(windowHeight* 0.3842), 1.0f, glm::vec3(1, 0, 1));
	}
	else {
		renderText("Number of Human Players: " + std::to_string(_broker->_nbPlayers), GLfloat(windowWidth* 0.3385), GLfloat(windowHeight* 0.3842), 1.0f, glm::vec3(0, 0, 0));
	}

	renderText("Start Game", GLfloat(windowWidth* 0.4348), GLfloat(windowHeight* 0.2314), 1.0f, glm::vec3(0, 0, 0));

	if (_broker->_cursorPositionSetup == 0) {
		renderSprite(*_buttonHighlightSprite, -0.15, -0.6, 0.15, -0.4);
	}
	else {
		renderSprite(*_buttonSprite, -0.15, -0.6, 0.15, -0.4);
	}

	renderSprite(*_titleScreenSprite, -1, -1, 1, 1);

	CheckGLErrors();
}

void RenderingManager::RenderCredits() {
	//Clears the screen to a light grey background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.639f, 0.701f, 0.780f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	glfwGetWindowSize(_window, &windowWidth, &windowHeight);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, (GLuint)windowWidth, (GLuint)windowHeight);	//reset the viewport to the full window to render from the camera pov

	renderText("Credits here", GLfloat(windowWidth * 0.4192), GLfloat(windowHeight* 0.4768), 1.0f, glm::vec3(1, 0, 1));

	renderSprite(*_backgroundSprite, -1, -1, 1, 1);

	CheckGLErrors();
}

void RenderingManager::RenderControls() {
	//Clears the screen to a light grey background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.639f, 0.701f, 0.780f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	glfwGetWindowSize(_window, &windowWidth, &windowHeight);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, (GLuint)windowWidth, (GLuint)windowHeight);	//reset the viewport to the full window to render from the camera pov

	renderText("Controls here", GLfloat(windowWidth * 0.4192), GLfloat(windowHeight* 0.4768), 1.0f, glm::vec3(1, 0, 1));

	renderSprite(*_backgroundSprite, -1, -1, 1, 1);

	CheckGLErrors();
}


bool compareStruct1(Player one, Player two) {
	return one.score > two.score;
}


void RenderingManager::renderPauseScreen() {
	//960 540
	renderText("PAUSED", windowWidth*0.37, windowHeight*0.45, 3.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	renderText("Resume", GLfloat(windowWidth*0.455), GLfloat(windowHeight*0.23), 1.0f, glm::vec3(0, 0, 0));
	renderText("Menu", GLfloat(windowWidth*0.466), GLfloat(windowHeight*0.106), 1.0f, glm::vec3(0, 0, 0));

	switch (_broker->_cursorPositionPause) {
	case (0):
		renderSprite(*_buttonSprite, -0.15, -0.6, 0.15, -0.4);
		renderSprite(*_buttonHighlightSprite, -0.15, -0.85, 0.15, -0.65);
		break;
	case(1):
		renderSprite(*_buttonHighlightSprite, -0.15, -0.6, 0.15, -0.4);
		renderSprite(*_buttonSprite, -0.15, -0.85, 0.15, -0.65);
		break;
	}
}


//TODO: *****needs to retrieve all the other players in the game to render to the side of the screen 
void RenderingManager::renderHud(int playerID) {

	std::vector<std::shared_ptr<ShoppingCartPlayer>> players = _broker->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
	std::shared_ptr<ShoppingCartPlayer> player = players[playerID];
	std::shared_ptr<PlayerScript> script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));
	int points = script->_points;
	int boost = script->getNBBoosts();


	//INPUT PLAYER STUFF BELOW
	//render the input players boost meter
	for (int i = 0; i < boost; i++) {
		renderSprite(*_boostSprite, -0.95 + (i * 0.15), -0.95, -0.75 + (i * 0.15), -0.75);
	}

	//render the input players shopping list on the bottom of the screen
	float offset = 0.10;
	int i = 0;
	for (EntityTypes eType : script->_shoppingList_Types) {

		if (script->_shoppingList_Flags[i]) {
			renderSprite(*getSpriteTexture(EntityTypes::CHECK_MARK), -0.15f + (offset*i), -0.9f, -0.05f + (offset*i), -0.75f);
		}
		renderSprite(*getSpriteTexture(eType), -0.15f + (offset*i), -0.9f, -0.05f + (offset*i), -0.75f);
		i++;
	}
	i = 0;

	renderSprite(*getPlayerBorderSprite(playerID), -0.15f, -0.92f, 0.16f, -0.73f);
	std::string pointDisplay = std::to_string(points);
	renderText("Your Score: " + pointDisplay, windowWidth*0.795, windowHeight*0.93, 1.0f, getPlayerColor(playerID));
	

	//OTHER PLAYER STUFF BELOW
	//Render the other player's lists and points to the side of the input player's screen
	std::vector<int> otherPlayerIDs;
	for (int i = 0; i < players.size(); i ++) {
		if (i != playerID) {
			otherPlayerIDs.push_back(i);
		}
	}


	float shoppingListOffSetAccum = 0.0f;
	float scoreOffsetAccum = -0.02f;
	float shoppingListYOffset = -0.25f;
	float scoreYOffset = -0.125f;

	for (int otherPlayerID : otherPlayerIDs) {

		player = players[otherPlayerID];
		script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));
		points = script->_points;


		int i = 0;
		offset = 0.07f;
		for (EntityTypes eType : script->_shoppingList_Types) {

			if (script->_shoppingList_Flags[i]) {

				renderSprite(*getSpriteTexture(EntityTypes::CHECK_MARK), 0.69f + (offset*i), 0.55f + shoppingListOffSetAccum, 0.76f + (offset*i), 0.70f + shoppingListOffSetAccum);
			}

			renderSprite(*getSpriteTexture(eType), 0.69f + (offset*i), 0.55f + shoppingListOffSetAccum, 0.76f + (offset*i), 0.70f + shoppingListOffSetAccum);
			i++;
		}
		renderSprite(*getPlayerBorderSprite(otherPlayerID), 0.69f, 0.55f + shoppingListOffSetAccum, 0.90f, 0.70f + shoppingListOffSetAccum);

		pointDisplay = std::to_string(points);
		renderText("Score: " + pointDisplay, windowWidth*0.82, windowHeight*(0.88 + scoreOffsetAccum), 0.8f, getPlayerColor(otherPlayerID));


		shoppingListOffSetAccum += shoppingListYOffset;
		scoreOffsetAccum += scoreYOffset;
	}

	std::string timeString = _broker->getAIManager()->getMatchTimePrettyFormat();
	//renderText(timeString, 870, 1010, 1.2f, glm::vec3(0.0f, 0.0f, 0.0f));
	renderText(timeString, windowWidth*0.45, windowHeight*0.94, 1.2f, glm::vec3(0.0f, 0.0f, 0.0f));

	//renderText("Boost:", 100, 100, 1.5f, glm::vec3(0.0f, 0.0f, 0.0f));
	renderText("Boost:", windowWidth*0.04, windowHeight*0.101, 1.5f, glm::vec3(0.0f, 0.0f, 0.0f));

	renderSprite(*_timeBoxSprite, -0.18, 0.83, 0.13, 1);
}




void RenderingManager::renderSprite(MyTexture spriteTex, float bottomLeftX, float bottomLeftY, float topRightX, float topRightY) {
	Geometry sprite;

	sprite.texture = spriteTex;

	sprite.verts.push_back(glm::vec4(bottomLeftX, bottomLeftY, 0.0f, 1.0f));
	sprite.verts.push_back(glm::vec4(topRightX, bottomLeftY, 0.0f, 1.0f));
	sprite.verts.push_back(glm::vec4(topRightX, topRightY, 0.0f, 1.0f));

	sprite.verts.push_back(glm::vec4(bottomLeftX, bottomLeftY, 0.0f, 1.0f));
	sprite.verts.push_back(glm::vec4(topRightX, topRightY, 0.0f, 1.0f));
	sprite.verts.push_back(glm::vec4(bottomLeftX, topRightY, 0.0f, 1.0f));

	sprite.uvs.push_back(glm::vec2(0.0f, 0.0f));
	sprite.uvs.push_back(glm::vec2(1.0f, 0.0f));
	sprite.uvs.push_back(glm::vec2(1.0f, 1.0f));
	sprite.uvs.push_back(glm::vec2(0.0f, 0.0f));
	sprite.uvs.push_back(glm::vec2(1.0f, 1.0f));
	sprite.uvs.push_back(glm::vec2(0.0f, 1.0f));
	

	glUseProgram(spriteShaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(sprite.texture.target, sprite.texture.textureID);
	GLuint uniformLocation = glGetUniformLocation(spriteShaderProgram, "SpriteTexture"); //send sprite data to the sprite shader
	glUniform1i(uniformLocation, 0);

	assignSpriteBuffers(sprite);
	setSpriteBufferData(sprite);

	glBindVertexArray(sprite.vao);
	glDrawArrays(GL_TRIANGLES, 0, sprite.verts.size());

	deleteBufferData(sprite);
	sprite.verts.clear();
	sprite.uvs.clear();
	glUseProgram(0);
	glBindVertexArray(0);
}





//https://learnopengl.com/In-Practice/Text-Rendering
void RenderingManager::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	glUseProgram(textShaderProgram);
	GLuint textVao, textVbo;

	//glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwGetWindowSize(_window, &windowWidth, &windowHeight);
	glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);

	glGenVertexArrays(1, &textVao);
	glGenBuffers(1, &textVbo);
	glBindVertexArray(textVao);
	glBindBuffer(GL_ARRAY_BUFFER, textVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// Activate corresponding render state	

	glUniformMatrix4fv(glGetUniformLocation(textShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(glGetUniformLocation(textShaderProgram, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVao);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textVbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDeleteBuffers(1, &textVbo);
	glDeleteVertexArrays(1, &textVao);

	glUseProgram(0);

	//glDisable(GL_CULL_FACE);
	//glDisable(GL_BLEND);
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
			// CHASSIS RENDERING...
			const std::vector<std::shared_ptr<ShoppingCartPlayer>> &players = _broker->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
			std::shared_ptr<ShoppingCartPlayer> player = std::dynamic_pointer_cast<ShoppingCartPlayer>(entity);
			
			int vehicleID = -1;
			for (int i = 0; i < players.size(); i++) {
				if (player == players.at(i)) {
					vehicleID = i;
					break;
				}
			}
			if (-1 == vehicleID) {
				std::cout << "ERROR: push3DObjects() - invalid vehicleID" << std::endl;
				continue;
			}

			switch (vehicleID) {
				case 0:
					geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::CART_RED_GEO_NO_INDEX));
					geo.texture = *_shoppingCartRed;
					break;
				case 1:
					geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::CART_BLUE_GEO_NO_INDEX));
					geo.texture = *_shoppingCartBlue;
					break;
				case 2:
					geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::CART_GREEN_GEO_NO_INDEX));
					geo.texture = *_shoppingCartGreen;
					break;
				case 3:
					geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::CART_PURPLE_GEO_NO_INDEX));
					geo.texture = *_shoppingCartPurple;
					break;
				case 4:
					geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::CART_ORANGE_GEO_NO_INDEX));
					geo.texture = *_shoppingCartOrange;
					break;
				case 5:
					geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::CART_BLACK_GEO_NO_INDEX));
					geo.texture = *_shoppingCartBlack;
					break;
				default:
					break;
			}


			// WHEEL RENDERING...
			const std::vector<PxShape*> &wheelShapes = player->_shoppingCartBase->_wheelShapes;
			for (PxShape *wheelShape : wheelShapes) {

				Geometry geoWheel = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::VEHICLE_WHEEL_GEO_NO_INDEX));
				geoWheel.color = glm::vec3(0.0f, 0.0f, 0.0f);

				glm::mat4 model;

				PxQuat netRotation = rot * wheelShape->getLocalPose().q; // MUST BE IN THIS ORDER
				PxMat44 rotation = PxMat44(netRotation); // compound rot (parent.rotate() and then local.rotate())
				PxVec3 wheelOffset = wheelShape->getLocalPose().p;
				wheelOffset = rot.rotate(wheelOffset);
				PxMat44 translation = PxMat44(PxMat33(PxIdentity), pos + wheelOffset);
				PxMat44	pxModel = translation * rotation;

				model = glm::mat4(glm::vec4(pxModel.column0.x, pxModel.column0.y, pxModel.column0.z, pxModel.column0.w),
					glm::vec4(pxModel.column1.x, pxModel.column1.y, pxModel.column1.z, pxModel.column1.w),
					glm::vec4(pxModel.column2.x, pxModel.column2.y, pxModel.column2.z, pxModel.column2.w),
					glm::vec4(pxModel.column3.x, pxModel.column3.y, pxModel.column3.z, pxModel.column3.w));

				geoWheel.model = model;
				geoWheel.drawMode = GL_TRIANGLES;
				assignBuffers(geoWheel);
				setBufferData(geoWheel);
				_objects.push_back(geoWheel);
			}


			// HOT POTATO RENDERING...
			std::shared_ptr<PlayerScript> playerScript = std::static_pointer_cast<PlayerScript>(player->getComponent(ComponentTypes::PLAYER_SCRIPT));
			if (playerScript->_hasHotPotato) {
				Geometry geoPotato = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::HOT_POTATO_GEO_NO_INDEX));
				geoPotato.color = glm::vec3(3.0f, 4.0f, 3.0f);
				
				_gradientDegree = playerScript->_hotPotatoTimer;

				glm::mat4 model;
				PxMat44 rotation = PxMat44(rot);
				PxVec3 potatoOffset(0.0f, 5.0f, 0.0f);
				PxMat44 translation = PxMat44(PxMat33(PxIdentity), pos + potatoOffset);
				PxMat44	pxModel = translation * rotation;
				model = glm::mat4(glm::vec4(pxModel.column0.x, pxModel.column0.y, pxModel.column0.z, pxModel.column0.w),
					glm::vec4(pxModel.column1.x, pxModel.column1.y, pxModel.column1.z, pxModel.column1.w),
					glm::vec4(pxModel.column2.x, pxModel.column2.y, pxModel.column2.z, pxModel.column2.w),
					glm::vec4(pxModel.column3.x, pxModel.column3.y, pxModel.column3.z, pxModel.column3.w));

				geoPotato.model = model;
				geoPotato.gradientShader = true;

				geoPotato.drawMode = GL_TRIANGLES;
				assignBuffers(geoPotato);
				setBufferData(geoPotato);
				_objects.push_back(geoPotato);

			}


			//TODO: only render pointers on a single camera for that 1 player
			// POINTER RENDERING...
			Geometry geoPointer = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::POINTER_GEO_NO_INDEX));
			geoPointer.color = glm::vec3(0, 0, 0);
			geoPointer.pointer = true;
			geoPointer.player = vehicleID;
			glm::mat4 model; 
			PxVec3 otherPos;

			float yOffset = 0;

			for (int i = 0; i < players.size(); i++) {
				// ignore self...
				if (i == vehicleID) continue;

				// create a pointer to this other cart...
				switch (i) {
					case 0:
						geoPointer.texture = *_shoppingCartRed;
						break;
					case 1:
						geoPointer.texture = *_shoppingCartBlue;
						break;
					case 2:
						geoPointer.texture = *_shoppingCartGreen;
						break;
					case 3:
						geoPointer.texture = *_shoppingCartPurple;
						break;
					case 4:
						geoPointer.texture = *_shoppingCartOrange;
						break;
					case 5:
						geoPointer.texture = *_shoppingCartBlack;
						break;
					default:
						break;
				}

				otherPos = players.at(i)->_actor->is<PxRigidDynamic>()->getGlobalPose().p;
				PxVec3 forward = rot.getBasisVector2();
				PxVec3 cartForward = forward;
				PxVec3 cartToCart = otherPos - pos;
				cartForward.y = 0;
				cartToCart.y = 0;

				float dot = cartForward.dot(cartToCart);
				float magnitudes = cartForward.magnitude()*cartToCart.magnitude();
				float angle = acos(dot / magnitudes);

				if ((cartForward.cross(cartToCart)).y <= 0)	angle = -angle;
				PxQuat localRot(angle, PxVec3(0.0f, 1.0f, 0.0f));
				PxQuat netRotation = rot * localRot;
				PxMat44 rotation = PxMat44(netRotation);

				PxVec3 pointerOffset(0.0f, yOffset, 5.0f);
				PxMat44 translation = PxMat44(PxMat33(PxIdentity), pos + rotation.rotate(pointerOffset));
				PxMat44	pxModel = translation * rotation;
				model = glm::mat4(glm::vec4(pxModel.column0.x, pxModel.column0.y, pxModel.column0.z, pxModel.column0.w),
					glm::vec4(pxModel.column1.x, pxModel.column1.y, pxModel.column1.z, pxModel.column1.w),
					glm::vec4(pxModel.column2.x, pxModel.column2.y, pxModel.column2.z, pxModel.column2.w),
					glm::vec4(pxModel.column3.x, pxModel.column3.y, pxModel.column3.z, pxModel.column3.w));

				geoPointer.model = model;
				geoPointer.drawMode = GL_TRIANGLES;

				assignBuffers(geoPointer);
				setBufferData(geoPointer);
				_objects.push_back(geoPointer);
				yOffset += 0.5f;
			}
			break;
		}
		case EntityTypes::GROUND:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::GROUND_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			geo.cullBackFace = true;
			break;
		}
		case EntityTypes::ROOF:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::ROOF_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			break;
		}
		case EntityTypes::MILK:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::MILK_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			break;
		}
		case EntityTypes::WATER:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::WATER_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			break;
		}
		case EntityTypes::COLA:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::COLA_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			break;
		}
		case EntityTypes::APPLE:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::APPLE_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			break;
		}
		case EntityTypes::WATERMELON:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::WATERMELON_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			break;
		}
		case EntityTypes::BANANA:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::BANANA_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			break;
		}
		case EntityTypes::CARROT:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::CARROT_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			break;
		}
		case EntityTypes::EGGPLANT:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::EGGPLANT_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			break;
		}
		case EntityTypes::BROCCOLI:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::BROCCOLI_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			break;
		}
		case EntityTypes::MYSTERY_BAG:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::MYSTERY_BAG_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			//geo.color = glm::vec3(0.05f, 0.5f, 0.2f);
			break;
		}
		case EntityTypes::COOKIE:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::COOKIE_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			//geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO));
			//geo.color = glm::vec3(0.05f, 0.5f, 0.2f);
			break;
		}
		case EntityTypes::SPARE_CHANGE:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::SPARE_CHANGE_GEO_NO_INDEX));
			break;
		}
		case EntityTypes::OBSTACLE1:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE1_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			geo.cullBackFace = true;
			break;
		}
		case EntityTypes::OBSTACLE2:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE2_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			geo.cullBackFace = true;
			break;
		}
		case EntityTypes::OBSTACLE3:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE3_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			geo.cullBackFace = true;
			break;
		}
		case EntityTypes::OBSTACLE4:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE4_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			geo.cullBackFace = true;
			break;
		}
		case EntityTypes::OBSTACLE5:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE5_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			geo.cullBackFace = true;
			break;
		}
		case EntityTypes::OBSTACLE6:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE6_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			geo.cullBackFace = true;
			break;
		}
		case EntityTypes::OBSTACLE7:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE7_GEO_NO_INDEX)); // TODO: change this to use specific mesh
			geo.cullBackFace = true;
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

		assignBuffers(geo);
		setBufferData(geo);
		_objects.push_back(geo);
	}
}

void RenderingManager::initFrameBuffers() {
	glGenFramebuffers(1, &_lightDepthFBO);
	glGenTextures(1, &_depthMapTex);								//init the texture for the depth map information
	glBindTexture(GL_TEXTURE_2D, _depthMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, _lightDepthFBO);			//configure the frame buffer to retain depth info
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMapTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);						//rebind the frame buffer to default		
}


void RenderingManager::initSpriteTextures() {

	InitializeTexture(_borderSpriteBlue, "../TopShopper/resources/Sprites/BlueBorder.png", GL_TEXTURE_2D);
	InitializeTexture(_borderSpriteBlack, "../TopShopper/resources/Sprites/BlackBorder.png", GL_TEXTURE_2D);
	InitializeTexture(_borderSpritePurple, "../TopShopper/resources/Sprites/PurpleBorder.png", GL_TEXTURE_2D);
	InitializeTexture(_borderSpriteRed, "../TopShopper/resources/Sprites/RedBorder.png", GL_TEXTURE_2D);
	InitializeTexture(_borderSpriteOrange, "../TopShopper/resources/Sprites/OrangeBorder.png", GL_TEXTURE_2D);
	InitializeTexture(_borderSpriteGreen, "../TopShopper/resources/Sprites/GreenBorder.png", GL_TEXTURE_2D);
	InitializeTexture(_appleSprite, "../TopShopper/resources/Sprites/Apple.png", GL_TEXTURE_2D);
	InitializeTexture(_bananaSprite, "../TopShopper/resources/Sprites/Banana.png", GL_TEXTURE_2D);
	InitializeTexture(_broccoliSprite, "../TopShopper/resources/Sprites/Broccoli.png", GL_TEXTURE_2D);
	InitializeTexture(_carrotSprite, "../TopShopper/resources/Sprites/Carrot.png", GL_TEXTURE_2D);
	InitializeTexture(_colaSprite, "../TopShopper/resources/Sprites/Cola.png", GL_TEXTURE_2D);
	InitializeTexture(_cookieSprite, "../TopShopper/resources/Sprites/Cookie.png", GL_TEXTURE_2D);
	InitializeTexture(_eggplantSprite, "../TopShopper/resources/Sprites/Eggplant.png", GL_TEXTURE_2D);
	InitializeTexture(_hotPotatoSprite, "../TopShopper/resources/Sprites/Hotpotato.png", GL_TEXTURE_2D);
	InitializeTexture(_milkSprite, "../TopShopper/resources/Sprites/Milk.png", GL_TEXTURE_2D);
	InitializeTexture(_waterSprite, "../TopShopper/resources/Sprites/Water.png", GL_TEXTURE_2D);
	InitializeTexture(_watermelonSprite, "../TopShopper/resources/Sprites/Watermelon.png", GL_TEXTURE_2D);
	InitializeTexture(_clockSprite, "../TopShopper/resources/Sprites/clock.png", GL_TEXTURE_2D);
	InitializeTexture(_checkMarkSprite, "../TopShopper/resources/Sprites/Check_Mark.png", GL_TEXTURE_2D);
	InitializeTexture(_boostSprite, "../TopShopper/resources/Sprites/Boost.png", GL_TEXTURE_2D);
	InitializeTexture(_buttonSprite, "../TopShopper/resources/Sprites/Button.png", GL_TEXTURE_2D);
	InitializeTexture(_buttonHighlightSprite, "../TopShopper/resources/Sprites/ButtonHighlight.png", GL_TEXTURE_2D);
	InitializeTexture(_timeBoxSprite, "../TopShopper/resources/Sprites/TimeBox.png", GL_TEXTURE_2D);
	InitializeTexture(_titleScreenSprite, "../TopShopper/resources/Sprites/TitleScreen.png", GL_TEXTURE_2D);
	InitializeTexture(_resultsScreenSprite, "../TopShopper/resources/Sprites/ResultsScreen.png", GL_TEXTURE_2D);
	InitializeTexture(_backgroundSprite, "../TopShopper/resources/Sprites/Background.png", GL_TEXTURE_2D);
}

void RenderingManager::init3DTextures() {
	MyTexture texture;
	InitializeTexture(&texture, "../TopShopper/resources/Textures/gold.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(SPARE_CHANGE_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/yellow.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(BANANA_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D); // CAN THIS BE REMOVED??
	_broker->getLoadingManager()->getGeometry(VEHICLE_CHASSIS_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/TireTexture.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(VEHICLE_WHEEL_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/StoreFloor.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(GROUND_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(ROOF_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(OBSTACLE1_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(OBSTACLE2_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(OBSTACLE3_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(OBSTACLE4_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(OBSTACLE5_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(OBSTACLE6_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(OBSTACLE7_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/MilkTexture.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(MILK_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/WaterTexture.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(WATER_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/ColaTexture.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(COLA_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/AppleTexture.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(APPLE_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/WatermelonTexture.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(WATERMELON_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/CarrotTexture.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(CARROT_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/EggplantTexture.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(EGGPLANT_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/BroccoliTexture.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(BROCCOLI_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/gold.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(MYSTERY_BAG_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/CookieTexture.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(COOKIE_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/PotatoTexture.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(HOT_POTATO_GEO_NO_INDEX)->texture = texture;

	//InitializeTexture(&texture, "../TopShopper/resources/Textures/gold.jpg", GL_TEXTURE_2D);
	//_broker->getLoadingManager()->getGeometry(POINTER_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(_shoppingCartRed, "../TopShopper/resources/Textures/CartRedTexture.jpg", GL_TEXTURE_2D); // 0
	InitializeTexture(_shoppingCartBlue, "../TopShopper/resources/Textures/CartBlueTexture.jpg", GL_TEXTURE_2D); // 1
	InitializeTexture(_shoppingCartGreen, "../TopShopper/resources/Textures/CartGreenTexture.jpg", GL_TEXTURE_2D); // 2
	InitializeTexture(_shoppingCartPurple, "../TopShopper/resources/Textures/CartPurpleTexture.jpg", GL_TEXTURE_2D); // 3
	InitializeTexture(_shoppingCartOrange, "../TopShopper/resources/Textures/CartOrangeTexture.jpg", GL_TEXTURE_2D); // 4
	InitializeTexture(_shoppingCartBlack, "../TopShopper/resources/Textures/CartBlackTexture.jpg", GL_TEXTURE_2D); // 5
}


MyTexture * RenderingManager::getPlayerBorderSprite(int playerID) {
	switch (playerID) {
	case 0:
		return _borderSpriteRed;
	case 1:
		return _borderSpriteBlue;
	case 2:
		return _borderSpriteGreen;
	case 3:
		return _borderSpritePurple;
	case 4:
		return _borderSpriteOrange;
	case 5:
		return _borderSpriteBlack;
	default:
		return nullptr;
	}
}

glm::vec3 RenderingManager::getPlayerColor(int playerID) {
	switch (playerID) {
	case 0:
		return glm::vec3(0.8f, 0.0f, 0.0f);
	case 1:
		return glm::vec3(0.0f, 0.0f, 0.8f);
	case 2:
		return glm::vec3(0.0f, 0.8f, 0.0f);
	case 3:
		return glm::vec3(0.8f, 0.0f, 0.8f);
	case 4:
		return glm::vec3(1.0f, 0.5f, 0.0f);
	case 5:
		return glm::vec3(0.0f, 0.0f, 0.0f);
	default:
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}
}





MyTexture * RenderingManager::getSpriteTexture(EntityTypes type) {
	switch (type) {
	case EntityTypes::MILK:
		return _milkSprite;
	case EntityTypes::WATER:
		return _waterSprite;
	case EntityTypes::COLA:
		return _colaSprite;
	case EntityTypes::APPLE:
		return _appleSprite;
	case EntityTypes::WATERMELON:
		return _watermelonSprite;
	case EntityTypes::BANANA:
		return _bananaSprite;
	case EntityTypes::CARROT:
		return _carrotSprite;
	case EntityTypes::EGGPLANT:
		return _eggplantSprite;
	case EntityTypes::BROCCOLI:
		return _broccoliSprite;
	case EntityTypes::CHECK_MARK:
		return _checkMarkSprite;
	default:
		return nullptr;
	}
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

	glGenBuffers(1, &geometry.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.uvBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
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

	glBindBuffer(GL_ARRAY_BUFFER, geometry.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * geometry.uvs.size(), geometry.uvs.data(), GL_STATIC_DRAW);
}


void RenderingManager::deleteBufferData(Geometry& geometry) {
	glDeleteBuffers(1, &geometry.vertexBuffer);
	glDeleteBuffers(1, &geometry.normalBuffer);
	glDeleteBuffers(1, &geometry.uvBuffer);
	glDeleteVertexArrays(1, &geometry.vao);
}

void RenderingManager::assignSpriteBuffers(Geometry& geometry) {
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


	glGenBuffers(1, &geometry.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.uvBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

}


void RenderingManager::setSpriteBufferData(Geometry& geometry) {
	//Send geometry to the GPU
	//Must be called whenever anything is updated about the object
	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * geometry.verts.size(), geometry.verts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, geometry.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * geometry.uvs.size(), geometry.uvs.data(), GL_STATIC_DRAW);
}


void RenderingManager::initTextRender() {
	//Text initialization
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "../TopShopper/resources/Fonts/lora/Lora-Regular.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}




void RenderingManager::cleanup() {
	glfwTerminate();
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
	int width = WINDOW_WIDTH;
	int height = WINDOW_HEIGHT;
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


