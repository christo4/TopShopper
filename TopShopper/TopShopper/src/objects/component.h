


#ifndef COMPONENT_H_
#define COMPONENT_H_

// ~~~~NOTE: components could be changed to structs, if we want everything public by default

// ~~~~~NOTE: each child class will implement very specialized behaviour if attached to an entity
// ~~~~~NOTE: will have an enum here ComponentTypes that assigns a unique index/id to each component type (subclass)
// ex. TRANSFORM = 0

// THE MANAGERS WILL THEN UPDATE STATE BASED ON DATA STORED IN COMPONENTS THEY MANAGE

/*
SOME IDEAS OF COMPONENTS WE NEED (by category):

AI:

NavMeshAgent (movement properties, target, path finding weights)
NavMesh (vertices/tris)


AUDIO:

AudioListener (simple, just flags we want audio to be relitive to this point)
AudioSource (audioclip, isMute?, playOnAwake?, Loop?, Priority, volume)


CORE:

Transform (vec3 position, vec3 rotation)


INPUT:


LOADING:


PHYSICS:

Rigidbody (mass, affectedByGravity?, isKinematic?, axis-rotation-lock?)
Collider (size properties, isTrigger?) (can have different shapes, depends what PhysX gives us)
RayCaster?


RENDERING:

Mesh (vertices/tris)
Mesh Renderer? (maybe this behaviour is just in the manager)
Material/Texture (color properties)
Camera (FOV, clipping planes (near and far), etc.)
Light (type (point, directional, etc.), range?, color, intensity)
<UIComonents including a way to render text?>



*/
#include <vector>
#include <glm/glm.hpp>

enum ComponentTypes {
	AUDIO_LISTENER,
	AUDIO_SOURCE,
	CAMERA,
	INPUT_CONTROLLER,
	MESH,
	NAV_MESH,
	NAV_MESH_AGENT,
	UI_SPRITE,
	NUMBER_OF_COMPONENT_TYPES // NOTE: THIS MUST BE THE LAST ENUM ELEMENT AND ALL ENUM VALUES MUST GO FROM 0 TO NUMBER_OF_COMPONENT_TYPES-1 
};


/*
class Component {
public:
	Component();
	virtual ~Component();
private:
	// 1. have a reference to the entity this component is attached to (so we can get high-lvl info about it (e.g. tag/name) or access sibling components)
};
*/

struct Component {

};

struct AudioListener : Component {

};

struct AudioSource : Component {

};

struct Camera : Component {

};

struct InputController : Component {

};

struct Mesh : Component {
	std::vector<glm::vec4> verts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
};

struct NavMesh : Component {

};

struct NavMeshAgent : Component {

};

struct UISprite : Component {

};



#endif // COMPONENT_H_
