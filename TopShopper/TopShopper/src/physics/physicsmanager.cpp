


#include "physicsmanager.h"
#include <iostream>



using namespace physx;

PhysicsManager::PhysicsManager(Broker* broker)
	: _broker(broker)
{

}

PhysicsManager::~PhysicsManager() {

}


void PhysicsManager::init() {
	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _defaultAlloc, _defaultError);
	if (!_foundation) {
		std::cout << "PxCreateFoundation failed!" << std::endl;
		exit(EXIT_FAILURE);
	}

	_simulationScale.length = 1.0f; // 1 meter by default
	_simulationScale.speed = 9.81f; // 9.81 m/s by default (speed reached after falling for 1 second under 9.81m/s^2 gravity)
	
	bool recordMemoryAllocations = true;

	// WITH PVD...
	/*
	_pvd = PxCreatePvd(*_foundation);
	PxPvdTransport *transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	_factory = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, _simulationScale, recordMemoryAllocations, _pvd);
	*/

	// W/O PVD...
	_factory = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, _simulationScale, recordMemoryAllocations);
	if (!_factory) {
		std::cout << "PxCreatePhysics failed!" << std::endl;
		exit(EXIT_FAILURE);
	}



	PxCookingParams params(_simulationScale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);

	_cooker = PxCreateCooking(PX_PHYSICS_VERSION, *_foundation, params);
	if (!_cooker) {
		std::cout << "PxCreateCooking failed!" << std::endl;
		exit(EXIT_FAILURE);
	}




}


// ASIDE: if we want a fixed timestep like Unity then pass in say 1/30 or 1/60 but then we need to change UpdateAll() to only call physcismanager.init()
void PhysicsManager::updateMilliseconds(double deltaTime) {
	// scene->simulate(dt); - from docs
	// for us, maybe add a scene manager and query through  broker to get the scene - PxScene, or just keep here??
	// replace dt with deltaTime

	// later on.. scene->fetchResults(true); // wait untile simulation frame ends
}



void PhysicsManager::cleanup() {
	_factory->release();
	_foundation->release();
}



/*
// MAKE KINEMATIC?
void PhysicsManager::disableShapeInContactTests(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
}

// MAKE NON_KINEMATIC?
void PhysicsManager::enableShapeInContactTests(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
}

// MAKE FOREIGN
void PhysicsManager::disableShapeInSceneQueryTests(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
}

// MAKE LOCAL
void PhysicsManager::enableShapeInSceneQueryTests(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
}


void PhysicsManager::setShapeTrigger(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
}

void PhysicsManager::setShapeSolid(physx::PxShape *shape) {
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
}
*/





// RESEARCH NOTES BELOW

/*
NOTE: https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/Startup.html

SHUTTING DOWN...

To dispose of any PhysX object, call its release() method. 
This will destroy the object, and all contained objects. 
The precise behavior depends on the object type being released, so refer to the reference guide for details. 
To shut down the extensions library, call the function PxCloseExtensions(). 
To shut down physics, call release() on the PxPhysics object, and this will clean up all of the physics objects:

mPhysics->release(); // for us, _factory->release()

Do not forget to release the foundation object as well, but only after all other PhysX modules have been released:

mFoundation->release(); // for us _foundation->release()

*/


/*
NOTES: http://gameworksdocs.nvidia.com/PhysX/4.0/documentation/PhysXGuide/Manual/Threading.html

User changes take precedence: for example, a user change to the position of an object while the simulation is running will overwrite the position which resulted from the simulation.
The delayed application of updates does not affect scene queries, which always take into account the latest changes.

*/

/*
geometry: https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyCollision.html

When a geometry object is specified for a shape, the geometry object is copied into the shape. There are some restrictions on which geometries may be specified for a shape, depending on the shape flags and the type of the parent actors.

TriangleMesh, HeightField and Plane geometries are not supported for simulation shapes that are attached to dynamic actors, unless the dynamic actors are configured to be kinematic.
TriangleMesh and HeightField geometries are not supported for trigger shapes.

DEFAULTS: SOLID COLLIDER, QUERIED, DEBUG RENDERED

NOTE: SIMULATION SHAPE (SOLID) AND TRIGGER SHAPE are MUTUALLY exlusive

TriangleMesh, HeightField and Plane geometries are not supported for simulation shapes that are attached to dynamic actors, unless the dynamic actors are configured to be kinematic.
TriangleMesh and HeightField geometries are not supported for trigger shapes.

Detach the shape from the actor as follows:

myActor.detachShape(*shape);


Note If the movement of the shape's actor does not need to be controlled by the simulation at all, i.e., the shape is used for scene queries only and gets moved manually if necessary, then memory can be saved by additionally disabling simulation on the actor (see the API documentation on PxActorFlag::eDISABLE_SIMULATION).

STATIC - Transform doesnt change at all
vs.
DYNAMIC - Transform can change

KINEMATIC - A dynamic actor that can ignore the rules of physics, moves based on what we tell it

WARNING
there is a major caveat for kinematic actors.
they do not trigger callbacks.
you will not get a callback when a "box" hit a kinematic object.
you need to use rays or sweeps to get objects around your actor.


The desired broad-phase algorithm is controlled by the PxBroadPhaseType enum, within the PxSceneDesc structure.


A region of interest is a world-space AABB around a volume of space controlled by the broad-phase. Objects contained inside those regions are properly handled by the broad-phase. Objects falling outside of those regions lose all collision detection. Ideally those regions should cover the whole game space, while limiting the amount of covered empty space.
Regions can overlap, although for maximum efficiency it is recommended to minimize the amount of overlap between regions as much as possible. Note that two regions whose AABBs just touch are not considered overlapping. For example the PxBroadPhaseExt::createRegionsFromWorldBounds helper function creates a number of non-overlapping region bounds by simply subdividing a given world AABB into a regular 2D grid.

The maximum number of regions is currently limited to 256.

Broad-Phase Callback:

A callback for broad-phase-related events can be defined within the PxSceneDesc structure. This PxBroadPhaseCallback object will be called when objects are found out of the specified regions of interest, i.e. "out of bounds". The SDK disables collision detection for those objects. It is re-enabled automatically as soon as the objects re-enter a valid region.

It is up to users to decide what to do with out-of-bounds objects. Typical options are:

delete the objects
let them continue their motion without collisions until they re-enter a valid region
artificially teleport them back to a valid place


*****Collision Filtering:


SampleSubmarineFilterShader() will be called for all pairs of shapes that come near each other -- more precisely: for all pairs of shapes whose axis aligned bounding boxes in world space are found to intersect for the first time. All behavior beyond that is determined by what SampleSubmarineFilterShader() returns.



THUS, I think that PhysX inits all shapes as non-interacting, until the move and then if there AABB intersect, they then get flagged as a potential collision pair

*******Each PxShape and PxParticleBase object in PhysX has a member variable of type PxFilterData. This is 128 bits of user defined data that can be used to store application specific information related to collision filtering. This is the other variable that is passed to SampleSubmarineFilterShader() for each object.
There is also the constant block. This is a chunk of per-scene global information that the application can give to the shader to operate on. You will want to use this to encode rules about what to filter and what not.


EVENTS:
- consult PxSimulationEventCallback::onTrigger/onWake/onSleep/onContact

ONTRIGGER():
ex. at bottom of https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyCollision.html



*/

/*
other notes:

set the scene desc. gravity and bounceThresholeBeclotiy

release geometry after attaching to an actor

The parameter 'true' to createShape() informs the SDK that the shape will not be shared with other actors. You can use shape sharing to reduce the memory costs of your simulation when you have many actors with identical geometry, but shared shapes have a very strong restriction: you cannot update the attributes of a shared shape while it is attached to an actor.

https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/apireference/files/structPxShapeFlag.html


NOTE: I THINK THAY CREATEEXCLUSIVESHAPE just makes a no-name shape and attaches it to an actor to force only 1 actor having that shape


****************Scene.addACtor()

*/


/*
RIGIDBODY DYNAMICS: https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyDynamics.html


The easiest way to calculate mass properties is to use the PxRigidBodyExt::updateMassAndInertia() helper function, which will set all three properties based on the actor's shapes and a uniform density value.

When manually setting the mass/inertia tensor of bodies, PhysX requires positive values for the mass and each principal axis of inertia. However, it is legal to provide 0s in these values. When provided with a 0 mass or inertia value, PhysX interprets this to mean infinite mass or inertia around that principal axis. This can be used to create bodies that resist all linear motion or that resist all or some angular motion. Examples of the effects that could be achieved using this approach are:

Bodies that behave as if they were kinematic.
Bodies whose translation behaves kinematically but whose rotation is dynamic.
Bodies whose translation is dynamic but whose rotation is kinematic.
Bodies which can only rotate around a specific axis.

0 mass == infinite

*/


/*
PhysX types: ACTORS

PxRigidStatic - SOLID & FIXED
PxRigidDynamic - SOLID & FIXED (split into kinematic and non-kinematic)

Thus the RigidBody component can have a constructor param isStatic that you must specify
then depending on flag, you create either the static or dynamic



*/