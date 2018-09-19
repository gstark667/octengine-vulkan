#ifndef H_PHYSICS
#define H_PHYSICS

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <LinearMath/btMotionState.h>
#include <LinearMath/btDefaultMotionState.h>

#include <glm/glm.hpp>
#include <set>


struct physics_world_t
{
    btBroadphaseInterface *broadphase;
    btDefaultCollisionConfiguration *collisionConfiguration;
    btCollisionDispatcher *dispatcher;
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld *dynamicsWorld;

    std::set<std::pair<void*, void*>> collisions;
};


struct physics_object_t
{
    btCollisionShape *collisionShape;
    btDefaultMotionState *motionState;
    btRigidBody *rigidBody;
};


void physics_world_init(physics_world_t *world);
void physics_world_update(physics_world_t *world, void *scene, float delta);
void physics_world_add(physics_world_t *world, physics_object_t *object);
void physics_world_destroy(physics_world_t *world);


void physics_object_init_box(physics_object_t *object, void *user, float mass, float x, float y, float z);
void physics_object_init_sphere(physics_object_t *object, void *user, float mass, float radius);
void physics_object_init_capsule(physics_object_t *object, void *user, float mass, float radius, float height);
void physics_object_init(physics_object_t *object, void *user, float mass);

glm::vec3 physics_object_get_position(physics_object_t *object);
void physics_object_set_position(physics_object_t *object, float x, float y, float z);

glm::vec3 physics_object_get_rotation(physics_object_t *object);
void physics_object_set_rotation(physics_object_t *object, float x, float y, float z);

glm::vec3 physics_object_get_velocity(physics_object_t *object);
void physics_object_set_velocity(physics_object_t *object, float x, float y, float z);

void physics_object_apply_force(physics_object_t *object, float fx, float fy, float fz, float px, float py, float pz);
void physics_object_set_mass(physics_object_t *object, float mass);
void physics_object_set_angular_factor(physics_object_t *object, float factor);

void physics_object_destroy(physics_object_t *object);

#endif
