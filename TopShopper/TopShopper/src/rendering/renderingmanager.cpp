#include "renderingmanager.h"
#include "core/broker.h"
#include "loading/loadingmanager.h"
#include <iostream>
#include <string>
#include "ShaderTools.h"

#include "PxRigidDynamic.h"

#include <ft2build.h>
#include FT_FREETYPE_H

//**Must include glad and GLFW in this order or it breaks**
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

#include "vehicle/VehicleShoppingCart.h"



using namespace physx;


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

	glUseProgram(shaderProgram);

	MyTexture texture;
	InitializeTexture(&texture, "../TopShopper/resources/Textures/yellow.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(SPARE_CHANGE_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background2-marble.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(VEHICLE_CHASSIS_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background1-asphalt.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(VEHICLE_WHEEL_GEO_NO_INDEX)->texture = texture;

	InitializeTexture(&texture, "../TopShopper/resources/Textures/background3-wood.jpg", GL_TEXTURE_2D);
	_broker->getLoadingManager()->getGeometry(GROUND_GEO_NO_INDEX)->texture = texture;

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
	RenderScene();
	glfwSwapBuffers(_window);
}


void RenderingManager::RenderScene() {
	//Clears the screen to a dark grey background
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry


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


	GLuint ModelID = glGetUniformLocation(shaderProgram, "Model");
	GLuint ViewID = glGetUniformLocation(shaderProgram, "View");
	GLuint ProjectionID = glGetUniformLocation(shaderProgram, "Projection");
	GLuint colorID = glGetUniformLocation(shaderProgram, "Color");
	GLuint cameraID = glGetUniformLocation(shaderProgram, "CameraPos");


	for (Geometry& g : _objects) {
		glUseProgram(shaderProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(g.texture.target, g.texture.textureID);
		GLuint uniformLocation = glGetUniformLocation(shaderProgram, "imageTexture");

		glUniform1i(uniformLocation, 0);
		glUniform3f(cameraID, cameraPosition.x, cameraPosition.y, cameraPosition.z);
		glUniform3f(colorID, g.color.x, g.color.y, g.color.z);
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &g.model[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &Projection[0][0]);

		glBindVertexArray(g.vao);
		assignBuffers(g);
		setBufferData(g);
		glDrawArrays(GL_TRIANGLES, 0, g.verts.size());
		glBindVertexArray(0);
	}


	renderText("Test Text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
	renderSprites();
	CheckGLErrors();
}


void RenderingManager::renderSprites() {
	MyTexture texture;
	Geometry sprite;
	InitializeTexture(&texture, "../TopShopper/resources/Sprites/Border.png", GL_TEXTURE_2D);
	sprite.texture = texture;

	sprite.verts.push_back(glm::vec4(-0.2f, -0.85f, 0.0f, 1.0f));
	sprite.verts.push_back(glm::vec4(0.2f, -0.85f, 0.0f, 1.0f));
	sprite.verts.push_back(glm::vec4(0.2f, -0.65f, 0.0f, 1.0f));
	sprite.verts.push_back(glm::vec4(-0.2f, -0.85f, 0.0f, 1.0f));
	sprite.verts.push_back(glm::vec4(0.2f, -0.65f, 0.0f, 1.0f));
	sprite.verts.push_back(glm::vec4(-0.2f, -0.65f, 0.0f, 1.0f));

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
	glDeleteBuffers(1, &geometry.colorBuffer);
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


