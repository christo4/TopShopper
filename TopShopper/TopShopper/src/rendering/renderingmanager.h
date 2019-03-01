#ifndef RENDERINGMANAGER_H_
#define RENDERINGMANAGER_H_

//**Must include glad and GLFW in this order or it breaks**
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Geometry.h"
#include <map>

class Broker; 
struct GLFWwindow;


struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};



class RenderingManager {
public:
	RenderingManager(Broker *broker);
	virtual ~RenderingManager();


	void init();
	void updateSeconds(double variableDeltaTime);
	void cleanup();

	//Renders each object
	void RenderScene(std::vector<Geometry>& objects);
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	//Create vao and vbos for objects
	static void assignBuffers(Geometry& geometry);
	static void setBufferData(Geometry& geometry);
	static void deleteBufferData(Geometry& geometry);

	//Ensures that vao and vbos are set up properly
	bool CheckGLErrors();

	//Pointer to the current shader program being used to render
	GLuint shaderProgram;
	GLuint textShaderProgram;

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

	GLuint textVao, textVbo;
};

#endif // RENDERINGMANAGER_H_
