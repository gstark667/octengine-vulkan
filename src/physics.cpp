#include "physics.h"

#include <iostream>

int i = 0;
bool physics_collision_callback(btManifoldPoint& cp,const btCollisionObjectWrapper* obj1,int id1,int index1,const btCollisionObjectWrapper* obj2,int id2,int index2)
{
    std::cout << "added" << i << std::endl;
    i += 1;
    return false;
}

bool physics_contact_processed_callback(btManifoldPoint& cp, void* body0, void* body1)
{
    return false;
}

void physics_world_init(physics_world_t *world)
{
    gContactAddedCallback = physics_collision_callback;
    gContactProcessedCallback = physics_contact_processed_callback;
    world->broadphase = new btDbvtBroadphase();

    world->collisionConfiguration = new btDefaultCollisionConfiguration();
    world->dispatcher = new btCollisionDispatcher(world->collisionConfiguration);

    world->solver = new btSequentialImpulseConstraintSolver;

    world->dynamicsWorld = new btDiscreteDynamicsWorld(world->dispatcher, world->broadphase, world->solver, world->collisionConfiguration);
    world->dynamicsWorld->setGravity(btVector3(0, -10.0f, 0));
}

void physics_world_update(physics_world_t *world, float delta)
{
    world->dynamicsWorld->stepSimulation(delta, 10);
}

void physics_world_add(physics_world_t *world, physics_object_t *object)
{
    world->dynamicsWorld->addRigidBody(object->rigidBody);
}

void physics_world_destroy(physics_world_t *world)
{
    delete world->dynamicsWorld;
    delete world->solver;
    delete world->dispatcher;
    delete world->collisionConfiguration;
    delete world->broadphase;
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
    object->rigidBody->setCollisionFlags(object->rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    object->rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

glm::vec3 physics_object_get_position(physics_object_t *object)
{
    btTransform trans = object->rigidBody->getWorldTransform();
    btVector3 origin = trans.getOrigin();
    return glm::vec3(origin.getX(), origin.getY(), origin.getZ());
}

void physics_object_set_position(physics_object_t *object, float x, float y, float z)
{
    btTransform trans = object->rigidBody->getWorldTransform();
    btVector3 origin(x, y, z);
    trans.setOrigin(origin);
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

void physics_object_set_rotation(physics_object_t *object, float x, float y, float z)
{
    btTransform trans = object->rigidBody->getWorldTransform();
    btQuaternion quat(0, 0, 0, 1);
    quat.setEulerZYX(z, y, x);
    trans.setRotation(quat);
    object->motionState->setWorldTransform(trans);
    object->rigidBody->setMotionState(object->motionState);
}

glm::vec3 physics_object_get_velocity(physics_object_t *object)
{
    btVector3 velocity = object->rigidBody->getLinearVelocity();
    return glm::vec3(velocity.getX(), velocity.getY(), velocity.getZ());
}

void physics_object_set_velocity(physics_object_t *object, float x, float y, float z)
{
    object->rigidBody->setLinearVelocity(btVector3(x, y, z));
}

void physics_object_apply_force(physics_object_t *object, float fx, float fy, float fz, float px, float py, float pz)
{
    object->rigidBody->applyImpulse(btVector3(fx, fy, fz), btVector3(px, py, pz));
}

void physics_object_set_mass(physics_object_t *object, float mass)
{
    object->rigidBody->setMassProps(mass, btVector3(0.0f, 0.0f, 0.0f));
}

void physics_object_set_angular_factor(physics_object_t *object, float factor)
{
    object->rigidBody->setAngularFactor(factor); 
}

void physics_object_destroy(physics_object_t *object)
{
    delete object->motionState;
    delete object->rigidBody;
    delete object->collisionShape;
}

