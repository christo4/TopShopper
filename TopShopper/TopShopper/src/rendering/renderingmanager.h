#ifndef RENDERINGMANAGER_H_
#define RENDERINGMANAGER_H_

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define SHADOW_MAP_WIDTH 6144
#define SHADOW_MAP_HEIGHT 6144

#define SPLIT_SCREEN true


//**Must include glad and GLFW in this order or it breaks**
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Geometry.h"
#include <map>
#include "../objects/entity.h"
#include <algorithm>

class Broker;
struct GLFWwindow;


struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

struct Player {
	int score;
	std::string player;
};

enum SpriteTexture {
	BORDER_SPRITE_BLACK,
	BORDER_SPRITE_BLUE,
	BORDER_SPRITE_RED,
	BORDER_SPRITE_PURPLE,
	BORDER_SPRITE_ORANGE,
	BORDER_SPRITE_GREEN,
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



class RenderingManager {
public:
	RenderingManager(Broker *broker);
	virtual ~RenderingManager();

	void init();
	void updateSeconds(double variableDeltaTime);
	void cleanup();

	//Renders each object
	void RenderGameScene(int playerID, int viewBottomLeftx, int viewBottomLeftY, int viewTopRightX, int viewTopRightY);
	void RenderMainMenu();
	void RenderLoading();
	void RenderSetup();
	void RenderCredits();
	void RenderControls();

	glm::mat4 computeCameraPosition(int playerID, glm::vec3 &camera);

	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	void renderSprite(MyTexture spriteTex, float bottomLeftX, float bottomLeftY, float topRightX, float topRightY);
	void renderHud(int playerID);
	void renderEndScreen();
	void renderPauseScreen();

	void initTextRender();
	void initSpriteTextures();
	void init3DTextures();
	void initFrameBuffers();

	MyTexture * getSpriteTexture(EntityTypes type);
	MyTexture * getPlayerBorderSprite(int playerID);
	glm::vec3 getPlayerColor(int playerID);


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
	GLuint depthBufferShaderProgram;
	GLuint quadTestShaderProgram;
	GLuint gradientShaderProgram;
	GLuint transparencyShaderProgram;

	GLFWwindow* getWindow();
	void QueryGLVersion();
	void push3DObjects();
	std::map<GLchar, Character> Characters;

	int windowHeight;
	int windowWidth;

	float pi = 3.14159265;

private:

	Broker *_broker = nullptr;
	GLFWwindow *_window = nullptr;
	std::vector<Geometry> _objects;
	unsigned int _lightDepthFBO;
	unsigned int _depthMapTex;
	float _gradientDegree;
	void openWindow();

	MyTexture *_borderSpriteBlack = new MyTexture();
	MyTexture *_borderSpriteBlue = new MyTexture();
	MyTexture *_borderSpriteRed = new MyTexture();
	MyTexture *_borderSpritePurple = new MyTexture();
	MyTexture *_borderSpriteOrange = new MyTexture();
	MyTexture *_borderSpriteGreen = new MyTexture();
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
	MyTexture *_boostSprite = new MyTexture();
	MyTexture *_buttonSprite = new MyTexture();
	MyTexture *_buttonHighlightSprite = new MyTexture();
	MyTexture *_timeBoxSprite = new MyTexture();
	MyTexture *_titleScreenSprite = new MyTexture();
	MyTexture *_resultsScreenSprite = new MyTexture();
	MyTexture *_backgroundSprite = new MyTexture();
	
	MyTexture *_shoppingCartRed = new MyTexture(); // 0
	MyTexture *_shoppingCartBlue = new MyTexture(); // 1
	MyTexture *_shoppingCartGreen = new MyTexture(); // 2
	MyTexture *_shoppingCartPurple = new MyTexture(); // 3
	MyTexture *_shoppingCartOrange = new MyTexture(); // 4
	MyTexture *_shoppingCartBlack = new MyTexture(); // 5
};

bool compareStruct1(Player one, Player two);
#endif RENDERINGMANAGER_H_

