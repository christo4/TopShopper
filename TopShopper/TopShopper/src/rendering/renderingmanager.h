#ifndef RENDERINGMANAGER_H_
#define RENDERINGMANAGER_H_

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


//**Must include glad and GLFW in this order or it breaks**
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Geometry.h"
#include <map>
#include "../objects/entity.h"

class Broker;
struct GLFWwindow;


struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

enum SpriteTexture {
	BORDER_SPRITE,
	APPLE_SPRITE,
	BANANA_SPRITE,
	BROCCOLI_SPRITE,
	CARROT_SPRITE,
	COLA_SPRITE,
	COOKIE_SPRITE,
	EGGPLANT_SPRITE,
	HOT_POTATO_SPRITE,
	MILK_SPRITE,
	WATER_SPRITE,
	WATERMELON_SPRITE
};

/*
static struct HUDInfo {
	glm::vec2 playerListBotLeft = glm::vec2(852.0f, 100.0f);
	glm::vec2 playerListTopRight = glm::vec2(1068.0f, 172.0f);
	glm::vec2 playerListFirstBotLeft = glm::vec2(856.0f, 104.0f);
	glm::vec2 playerListFirstTopRight = glm::vec2(920, 168);



};

*/


class RenderingManager {
public:
	RenderingManager(Broker *broker);
	virtual ~RenderingManager();


	void init();
	void updateSeconds(double variableDeltaTime);
	void cleanup();

	//Renders each object
	void RenderScene();
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	void renderSprite(MyTexture spriteTex, int bottomLeftX, int bottomLeftY, int topRightX, int topRightY);
	void renderHud();
	void initTextRender();
	void initSpriteTextures();
	MyTexture * getSpriteTexture(EntityTypes type);


	//Create vao and vbos for objects
	static void assignBuffers(Geometry& geometry);
	static void setBufferData(Geometry& geometry);
	static void deleteBufferData(Geometry& geometry);

	static void assignSpriteBuffers(Geometry& geometry);
	static void setSpriteBufferData(Geometry& geometry);

	//Ensures that vao and vbos are set up properly
	bool CheckGLErrors();

	//Pointer to the current shader program being used to render
	GLuint shaderProgram;
	GLuint textShaderProgram;
	GLuint spriteShaderProgram;
	GLuint shaderProgramNoTex;

	GLFWwindow* getWindow();

	void QueryGLVersion();

	glm::mat4 Camera(float theta, float radius, float phi);
	void push3DObjects();
	//void textBuffers();

	std::map<GLchar, Character> Characters;




private:

	Broker *_broker = nullptr;
	GLFWwindow *_window = nullptr;
	std::vector<Geometry> _objects;
	void openWindow();

	MyTexture *_borderSprite = new MyTexture();
	MyTexture *_appleSprite = new MyTexture();
	MyTexture *_bananaSprite = new MyTexture();
	MyTexture *_broccoliSprite = new MyTexture();
	MyTexture *_carrotSprite = new MyTexture();
	MyTexture *_colaSprite = new MyTexture();
	MyTexture *_cookieSprite = new MyTexture();
	MyTexture *_eggplantSprite = new MyTexture();
	MyTexture *_hotPotatoSprite = new MyTexture();
	MyTexture *_milkSprite = new MyTexture();
	MyTexture *_waterSprite = new MyTexture();
	MyTexture *_watermelonSprite = new MyTexture();
	MyTexture *_clockSprite = new MyTexture();
	MyTexture *_checkMarkSprite = new MyTexture();
};

#endif // RENDERINGMANAGER_H_
