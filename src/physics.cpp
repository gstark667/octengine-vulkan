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

void physics_world_add(physics_world_t *world, physics_object_t *object)
{
    world->dynamicsWorld->addRigidBody(object->rigidBody);
}

void physics_world_destroy(physics_world_t *world)
{
    delete world->broadphase;
    delete world->collisionConfiguration;
    delete world->dispatcher;
    delete world->solver;
    delete world->dynamicsWorld;
}

void physics_object_init_box(physics_object_t *object, float mass, float x, float y, float z)
{
    object->collisionShape = new btBoxShape(btVector3(x, y, z));
    physics_object_init(object, mass);
}

void physics_object_init_sphere(physics_object_t *object, float mass, float radius)
{
    object->collisionShape = new btSphereShape(radius);
    physics_object_init(object, mass);
}

void physics_object_init_capsule(physics_object_t *object, float mass, float radius, float height)
{
    object->collisionShape = new btCapsuleShape(radius, height);
    physics_object_init(object, mass);
}

void physics_object_init(physics_object_t *object, float mass)
{
    object->motionState = new btDefaultMotionState(btTransform(
        btQuaternion(0, 0, 0, 1),
        btVector3(0, 0, 0)
    ));

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
        mass,
        object->motionState,
        object->collisionShape,
        btVector3(0.1,0.1,0.1)
    );

    object->rigidBody = new btRigidBody(rigidBodyCI);
}

glm::vec3 physics_object_get_position(physics_object_t *object)
{
    btTransform trans = object->rigidBody->getWorldTransform();
    btVector3 origin = trans.getOrigin();
    return glm::vec3(origin.getX(), origin.getY(), origin.getZ());
}

void physics_object_set_position(physics_object_t *object, float x, float y, float z)
{
    btVector3 origin(x, y, z);
    btTransform trans;
    trans.setOrigin(origin);
    trans.setRotation(btQuaternion(0, 0, 0, 1));
    object->motionState->setWorldTransform(trans);
    object->rigidBody->setMotionState(object->motionState);
}

glm::vec3 physics_object_get_rotation(physics_object_t *object)
{
    btTransform trans = object->rigidBody->getWorldTransform();
    btQuaternion rot = trans.getRotation();
    btScalar x, y, z;
    rot.getEulerZYX(z, y, x);
    return glm::vec3(x, y, z);
}

void physics_object_set_velocity(physics_object_t *object, float x, float y, float z)
{
    object->rigidBody->setLinearVelocity(btVector3(x, y, z));
}

void physics_object_set_mass(physics_object_t *object, float mass)
{
    object->rigidBody->setMassProps(mass, btVector3(0.0f, 0.0f, 0.0f));
}

void physics_object_destroy(physics_object_t *object)
{
    delete object->collisionShape;
    delete object->motionState;
    delete object->rigidBody;
}

