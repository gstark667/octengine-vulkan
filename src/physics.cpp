#include "physics.h"


void physics_world_init(physics_world_t *world)
{
    world->broadphase = new btDbvtBroadphase();

    world->collisionConfiguration = new btDefaultCollisionConfiguration();
    world->dispatcher = new btCollisionDispatcher(world->collisionConfiguration);

    world->solver = new btSequentialImpulseConstraintSolver;

    world->dynamicsWorld = new btDiscreteDynamicsWorld(world->dispatcher, world->broadphase, world->solver, world->collisionConfiguration);
    world->dynamicsWorld->setGravity(btVector3(0,-9.81f,0));
}

void physics_world_update(physics_world_t *world, float delta)
{
    world->dynamicsWorld->stepSimulation(delta);
}

void physics_world_destroy(physics_world_t *world)
{
    delete world->broadphase;
    delete world->collisionConfiguration;
    delete world->dispatcher;
    delete world->solver;
    delete world->dynamicsWorld;
}

void physics_object_init_box(physics_object_t *object, float x, float y, float z)
{
    object->collisionShape = new btBoxShape(btVector3(x, y, z));
    physics_object_init(object);
}

void physics_object_init_sphere(physics_object_t *object, float radius)
{
    object->collisionShape = new btSphereShape(radius);
    physics_object_init(object);
}

void physics_object_init_capsule(physics_object_t *object, float radius, float height)
{
    object->collisionShape = new btCapsuleShape(radius, height);
    physics_object_init(object);
}

void physics_object_init(physics_object_t *object)
{
    object->motionState = new btDefaultMotionState(btTransform(
        btQuaternion(0, 0, 0, 1),
        btVector3(0, 0, 0)
    ));

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
        1,                  // mass, in kg. 0 -> Static object, will never move.
        object->motionState,
        object->collisionShape,  // collision shape of body
        btVector3(0,0,0)    // local inertia
    );

    object->rigidBody = new btRigidBody(rigidBodyCI);
}

void physics_object_set_velocity(physics_object_t *object, float x, float y, float z)
{
    object->rigidBody->setLinearVelocity(btVector3(x, y, z));
}

void physics_object_destroy(physics_object_t *object)
{
    delete object->collisionShape;
    delete object->motionState;
    delete object->rigidBody;
}

