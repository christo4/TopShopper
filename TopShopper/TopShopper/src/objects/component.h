#ifndef COMPONENT_H_
#define COMPONENT_H_



#include <vector>
#include <glm/glm.hpp>

// ORDERED ALPHABETICALLY FROM 0 (corresponding to index in entity::_components)
enum ComponentTypes {
	// AUDIO...
	AUDIO_LISTENER=0,
	AUDIO_SOURCE,

	// RENDERING...
	CAMERA,
	MESH,

	// AI...
	NAV_MESH_AGENT,

	NUMBER_OF_COMPONENT_TYPES
};



struct Component {

};

struct AudioListener : Component {
	// simple, just flags we want audio to be relative to this point
};

struct AudioSource : Component {
	// audioclip(s), isMute ? , playOnAwake ? , Loop ? , Priority, volume)
};

struct Camera : Component {
	// FOV, clipping planes(near and far), etc.
};

struct Mesh : Component {
	std::vector<glm::vec4> verts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
	// TODO: material properties 
};

struct NavMeshAgent : Component {
	//movement properties, target, path finding weights, etc.
};





#endif // COMPONENT_H_
