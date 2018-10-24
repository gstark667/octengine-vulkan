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
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btConvexShape.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <LinearMath/btMotionState.h>
#include <LinearMath/btDefaultMotionState.h>
#include <LinearMath/btIDebugDraw.h>

#include <glm/glm.hpp>
#include <set>

#include "export.h"

struct ray_hit_t
{
    bool hit = false;
    float dist = 0.0f;
    glm::vec3 point = glm::vec3(0.0);
    glm::vec3 normal = glm::vec3(0.0);
};

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
    btTriangleMesh *mesh = NULL;
    btDefaultMotionState *motionState;
    btRigidBody *rigidBody;
};


extern "C" {
void EXPORT physics_world_init(physics_world_t *world);
void EXPORT physics_world_update(physics_world_t *world, void *scene, float delta);
void EXPORT physics_world_add(physics_world_t *world, physics_object_t *object);
ray_hit_t EXPORT physics_world_ray_test(physics_world_t *world, glm::vec3 from, glm::vec3 to);
void EXPORT physics_world_destroy(physics_world_t *world);


void EXPORT physics_object_init_box(physics_object_t *object, void *user, float mass, float x, float y, float z);
void EXPORT physics_object_init_sphere(physics_object_t *object, void *user, float mass, float radius);
void EXPORT physics_object_init_capsule(physics_object_t *object, void *user, float mass, float radius, float height);
void EXPORT physics_object_init_convex_hull(physics_object_t *object, void *user, float mass);
void EXPORT physics_object_init_mesh(physics_object_t *object, void *user, float mass);
void EXPORT physics_object_init(physics_object_t *object, void *user, float mass);

glm::vec3 EXPORT physics_object_get_position(physics_object_t *object);
void EXPORT physics_object_set_position(physics_object_t *object, float x, float y, float z);

glm::vec3 EXPORT physics_object_get_rotation(physics_object_t *object);
void EXPORT physics_object_set_rotation(physics_object_t *object, float x, float y, float z);

glm::vec3 EXPORT physics_object_get_velocity(physics_object_t *object);
void EXPORT physics_object_set_velocity(physics_object_t *object, float x, float y, float z);

glm::vec3 EXPORT physics_object_get_angular_velocity(physics_object_t *object);
void EXPORT physics_object_set_angular_velocity(physics_object_t *object, float x, float y, float z);

void EXPORT physics_object_apply_force(physics_object_t *object, float fx, float fy, float fz, float px, float py, float pz);
void EXPORT physics_object_set_mass(physics_object_t *object, float mass);
void EXPORT physics_object_set_angular_factor(physics_object_t *object, float factor);

void EXPORT physics_object_destroy(physics_object_t *object);
}

#endif
