#include "renderingmanager.h"
#include "core/broker.h"
#include "loading/loadingmanager.h"
#include <iostream>
#include <string>
#include "ShaderTools.h"
#include "PxRigidDynamic.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "vehicle/VehicleShoppingCart.h"
#include <deque>



using namespace physx;


std::deque<float> gVehicleThetas = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; // I WILL ENFORCE THIS TO BE A FIXED SIZE OF 10 (for now, holds the last 10 frames worth of VEHICLE ROTATIONS)

RenderingManager::RenderingManager(Broker *broker)
	: _broker(broker)
{

}

RenderingManager::~RenderingManager() {

}


void RenderingManager::init() {
	openWindow();
	initTextRender();

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

	glUseProgram(shaderProgram);

	/*TODO: Should do this in loading manager or a texture manager class*/
	init3DTextures();
	initSpriteTextures();
	initFrameBuffers();

}


void RenderingManager::updateSeconds(double variableDeltaTime) {
	// call LATEUPDATE() for all behaviour scripts...
	std::vector<std::shared_ptr<Entity>> entitiesCopy = _broker->getPhysicsManager()->getActiveScene()->_entities;
	for (std::shared_ptr<Entity> &entity : entitiesCopy) {
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
	RenderScene();
	glfwSwapBuffers(_window);
}


void RenderingManager::RenderScene() {
	//Clears the screen to a light grey background
	glClearColor(0.639f, 0.701f, 0.780f, 1.0f);


	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry

	float fov = 60.0f;
	int width;
	int height;
	glfwGetWindowSize(_window, &width, &height);

	glm::mat4 Projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 1.0f, 500.0f);

	// NOTE: later on... for right thumbstick, the cmaera position will curl around the circle proportional to -1.0 to 1.0 (post-process over the theta)
	std::shared_ptr<ShoppingCartPlayer> player = _broker->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers().at(0);
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

	gVehicleThetas.pop_front(); // pop the oldest frame
	gVehicleThetas.push_back(theta); // push current vehicle rotation (around +y-axis)

	// get the average vehicle rotation in last 10 frames (incl. this one) 
	// have to use vectors since thetas have edge case problems (e.g. going from pi to -pi)
	PxVec3 vehicleRotationVecSum(0.0f, 0.0f, 0.0f);
	for (float t : gVehicleThetas) {
		// assume a radius of 1 for this calculation (scale later by our intended radius)
		// also assume a y of 0.0f (can set y later)
		vehicleRotationVecSum += PxVec3(sin(t), 0.0f, cos(t));
	}
	vehicleRotationVecSum /= gVehicleThetas.size();

	// NOTE: I could change this to perform the glm::mix(t=0.5f) over all vectors, but the current solution seems to work

	float radius = 30.0f; // FIXED (for now)
	float camX = -1 * radius * vehicleRotationVecSum.x;
	float camY = 20.0f;
	float camZ = -1 * radius * vehicleRotationVecSum.z;



	glm::vec3 camPos(playerPos.x + camX, playerPos.y + camY, playerPos.z + camZ);
	glm::mat4 View = glm::lookAt(
		camPos, // camera position
		glm::vec3(playerPos.x, playerPos.y, playerPos.z), // looks at 
		glm::vec3(0.0f, 1.0f, 0.0f)  // up vector
	);




	GLuint ModelID;
	GLuint ViewID;
	GLuint ProjectionID;
	GLuint cameraID;
	GLuint LightViewID;
	GLuint LightProjectionID;

	

	glm::mat4 lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, 1.0f, 500.0f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(70.0f, 200.0f, 0.0f), glm::vec3(0.1f, 15.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));



	glViewport(0, 0, (GLuint)_shadowWidth, (GLuint)_shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, _lightDepthFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	ModelID = glGetUniformLocation(depthBufferShaderProgram, "Model");
	ViewID = glGetUniformLocation(depthBufferShaderProgram, "View");
	ProjectionID = glGetUniformLocation(depthBufferShaderProgram, "Projection");

	//render the scene from the light and fill the depth buffer
	for (Geometry& g : _objects) {	


		glUseProgram(depthBufferShaderProgram);

		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &g.model[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &lightView[0][0]);
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &lightProjection[0][0]);

		glBindVertexArray(g.vao);
		assignBuffers(g);
		setBufferData(g);

		Geometry geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::ROOF_GEO_NO_INDEX));

		if (g.verts.size() != geo.verts.size()) {
			glDrawArrays(GL_TRIANGLES, 0, g.verts.size());
		}

		glBindVertexArray(0);
	}

	
	/*
	glViewport(0, 0, (GLuint)1920, (GLuint)1080);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(quadTestShaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _depthMapTex);
	GLuint memeID = glGetUniformLocation(quadTestShaderProgram, "depthMap");
	glUniform1i(memeID, 0);

	Geometry quad;
	quad.verts.push_back(glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f));
	quad.verts.push_back(glm::vec4(1.0f, -1.0f, 0.0f, 1.0f));
	quad.verts.push_back(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	quad.verts.push_back(glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f));
	quad.verts.push_back(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	quad.verts.push_back(glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f));

	quad.uvs.push_back(glm::vec2(0.0f, 0.0f));
	quad.uvs.push_back(glm::vec2(1.0f, 0.0f));
	quad.uvs.push_back(glm::vec2(1.0f, 1.0f));
	quad.uvs.push_back(glm::vec2(0.0f, 0.0f));
	quad.uvs.push_back(glm::vec2(1.0f, 1.0f));
	quad.uvs.push_back(glm::vec2(0.0f, 1.0f));

	glBindVertexArray(quad.vao);
	assignBuffers(quad);
	setBufferData(quad);
	glDrawArrays(GL_TRIANGLES, 0, quad.verts.size());
	glBindVertexArray(0);
	deleteBufferData(quad);
	*/


	

	glViewport(0, 0, (GLuint)width, (GLuint)height);	//reset the viewport to the full window to render from the camera pov
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ModelID = glGetUniformLocation(shaderProgram, "Model");				//TODO: Probably dont need to do this everytime
	ViewID = glGetUniformLocation(shaderProgram, "View");
	ProjectionID = glGetUniformLocation(shaderProgram, "Projection");
	cameraID = glGetUniformLocation(shaderProgram, "CameraPos");
	LightViewID = glGetUniformLocation(shaderProgram, "LightView");
	LightProjectionID = glGetUniformLocation(shaderProgram, "LightProjection");


	for (Geometry& g : _objects) {

		glUseProgram(shaderProgram);					//use the default shader program

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(g.texture.target, g.texture.textureID);
		GLuint imageTexUniLocation = glGetUniformLocation(shaderProgram, "imageTexture");	//pass the geometry texture into the fragment shader
		glUniform1i(imageTexUniLocation, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, (GLuint) _depthMapTex);
		GLuint shadowTexUniLocation = glGetUniformLocation(shaderProgram, "shadowMap");		//pass the shadow map into the fragment shader
		glUniform1i(shadowTexUniLocation, 1);



		glUniform3f(cameraID, camPos.x, camPos.y, camPos.z);
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &g.model[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &Projection[0][0]);
		glUniformMatrix4fv(LightViewID, 1, GL_FALSE, &lightView[0][0]);
		glUniformMatrix4fv(LightProjectionID, 1, GL_FALSE, &lightProjection[0][0]);

		glBindVertexArray(g.vao);
		//assignBuffers(g);		//already done in the first rendering pass
		//setBufferData(g);
		glDrawArrays(GL_TRIANGLES, 0, g.verts.size());
		glBindVertexArray(0);
	}

	
	
	renderHud();
	

	CheckGLErrors();
}


//TODO: shouldn't be hardcoding the offsets for when the score display is dependent on the number of players in the game
void RenderingManager::renderHud() {

	std::vector<std::shared_ptr<ShoppingCartPlayer>> players = _broker->getPhysicsManager()->getActiveScene()->getAllShoppingCartPlayers();
	std::shared_ptr<ShoppingCartPlayer> player = players[0];
	std::shared_ptr<PlayerScript> script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));
	int points = script->_points;
	int boost = script->getNBBoosts();
	
	renderSprite(*_borderSprite, 852, 100, 1068, 172);
	int offset = 72;
	int i = 0;
	for (EntityTypes eType : script->_shoppingList_Types) {
		if (script->_shoppingList_Flags[i]) {
			renderSprite(*getSpriteTexture(CHECK_MARK), (856 + i * offset), 104, (920 + i * offset), 168);
		}		
		renderSprite(*getSpriteTexture(eType), (856 + i * offset), 104, (920 + i * offset), 168);
		i++;
	}
	i = 0;
	std::string pointDisplay = std::to_string(points);
	renderText("Your Score: " + pointDisplay, 1550, 1000, 1.0f, glm::vec3(0.8f, 0.0f, 0.0f));

	//player 2 hud elements
	player = players[1];
	script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));
	points = script->_points;

	renderSprite(*_borderSprite, 1580, 850, 1796, 922);

	for (EntityTypes eType : script->_shoppingList_Types) {
		if (script->_shoppingList_Flags[i]) {
			renderSprite(*getSpriteTexture(CHECK_MARK), (1584 + i * offset), 854, (1648 + i * offset), 918);
		}

		renderSprite(*getSpriteTexture(eType), (1584 + i*offset), 854, (1648 + i*offset), 918);
	
		i++;
	}
	i = 0;
	pointDisplay = std::to_string(points);
	renderText("Opp1 Score: " + pointDisplay, 1580, 950, 0.8f, glm::vec3(0.0f, 0.0f, 0.8f));
	
	
	//player 3 hud elements
	player = players[2];
	script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));
	points = script->_points;

	renderSprite(*_borderSprite, 1580, 700, 1796, 772);

	for (EntityTypes eType : script->_shoppingList_Types) {
		if (script->_shoppingList_Flags[i]) {
			renderSprite(*getSpriteTexture(CHECK_MARK), (1584 + i * offset), 704, (1648 + i * offset), 768);
		}
		renderSprite(*getSpriteTexture(eType), (1584 + i * offset), 704, (1648 + i * offset), 768);
		
		i++;
	}
	i = 0;
	pointDisplay = std::to_string(points);
	renderText("Opp2 Score: " + pointDisplay, 1580, 800, 0.8f, glm::vec3(0.0f, 0.8f, 0.0f));

	std::string timeString = _broker->getAIManager()->getMatchTimePrettyFormat();
	renderText(timeString, 870, 1010, 1.2f, glm::vec3(0.0f, 0.0f, 0.0f));

	renderText("Boost:", 100, 100, 1.5f, glm::vec3(0.0f, 0.0f, 0.0f));

	for (int i = 0; i < boost; i++) {
		renderSprite(*_boostSprite, 300 + (i * 105), 50, 450 + (i * 105), 200);
	}

}



void RenderingManager::renderSprite(MyTexture spriteTex, int bottomLeftX, int bottomLeftY, int topRightX, int topRightY) {
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

	int width;
	int height;
	glfwGetWindowSize(_window, &width, &height);
	glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
	glUniformMatrix4fv(glGetUniformLocation(spriteShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


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

	int width;
	int height;
	glfwGetWindowSize(_window, &width, &height);
	glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);

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

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
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

			std::shared_ptr<ShoppingCartPlayer> player = std::dynamic_pointer_cast<ShoppingCartPlayer>(entity);
			std::shared_ptr<PlayerScript> script = std::static_pointer_cast<PlayerScript>(player->getComponent(PLAYER_SCRIPT));
			int playerInputID = script->_inputID;


			if (playerInputID == 1) {					//TODO: make this less hacky
				geo.texture = *_shoppingCartRed;
			}

			if (playerInputID == -1) {
				geo.texture = *_shoppingCartBlue;
			}
			if (playerInputID == -2) {
				geo.texture = *_shoppingCartGreen;
			}



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
				_objects.push_back(geoWheel);
			}

			// TODO: put color indicators above carts (match color to _inputID?, but AI have inpoutID 0 i think..., I could init theirs to -1, -2, -3, etc.)

			// add hot potato model above cart if it has it...

			std::shared_ptr<PlayerScript> playerScript = std::static_pointer_cast<PlayerScript>(player->getComponent(ComponentTypes::PLAYER_SCRIPT));
			if (playerScript->_hasHotPotato) {
				Geometry geoPotato = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::HOT_POTATO_GEO_NO_INDEX));
				geoPotato.color = glm::vec3(0.0f, 0.0f, 0.0f);

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

				geoPotato.drawMode = GL_TRIANGLES;
				_objects.push_back(geoPotato);

			}
			break;
		}
		case EntityTypes::GROUND:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::GROUND_GEO_NO_INDEX)); // TODO: change this to use specific mesh
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
			break;
		}
		case EntityTypes::OBSTACLE2:
		{
			geo = *(_broker->getLoadingManager()->getGeometry(GeometryTypes::OBSTACLE2_GEO_NO_INDEX)); // TODO: change this to use specific mesh
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
}

void RenderingManager::initFrameBuffers() {
	glGenFramebuffers(1, &_lightDepthFBO);


	glGenTextures(1, &_depthMapTex);								//init the texture for the depth map information
	glBindTexture(GL_TEXTURE_2D, _depthMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _shadowWidth, _shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

	InitializeTexture(_borderSprite, "../TopShopper/resources/Sprites/Border.png", GL_TEXTURE_2D);
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
}

void RenderingManager::init3DTextures() {
	MyTexture texture;
	InitializeTexture(&texture, "../TopShopper/resources/Textures/gold.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(SPARE_CHANGE_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/yellow.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(BANANA_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(VEHICLE_CHASSIS_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background1-asphalt.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(VEHICLE_WHEEL_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(GROUND_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(ROOF_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(OBSTACLE1_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background3-wood.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(OBSTACLE2_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/white.png", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(MILK_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/blue.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(WATER_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/brown.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(COLA_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/red.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(APPLE_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/pink.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(WATERMELON_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/orange.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(CARROT_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/purple.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(EGGPLANT_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/green.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(BROCCOLI_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/gold.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(MYSTERY_BAG_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/gold.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(COOKIE_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/gold.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(HOT_POTATO_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(_shoppingCartBlue, "../TopShopper/resources/Textures/CartBlue.jpg", GL_TEXTURE_2D);
	InitializeTexture(_shoppingCartGreen, "../TopShopper/resources/Textures/CartGreen.jpg", GL_TEXTURE_2D);
	InitializeTexture(_shoppingCartRed, "../TopShopper/resources/Textures/CartRed.jpg", GL_TEXTURE_2D);


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


