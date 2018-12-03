#include <math.h>

#include "physics.h"
#include "gameobject.h"


void physics_world_init(physics_world_t *world)
{
    world->broadphase = new btDbvtBroadphase();

    world->collisionConfiguration = new btDefaultCollisionConfiguration();
    world->dispatcher = new btCollisionDispatcher(world->collisionConfiguration);

    world->solver = new btSequentialImpulseConstraintSolver;

    world->dynamicsWorld = new btDiscreteDynamicsWorld(world->dispatcher, world->broadphase, world->solver, world->collisionConfiguration);
    world->dynamicsWorld->setGravity(btVector3(0, -10.0f, 0));
}

void physics_world_update(physics_world_t *world, void *scene, float delta)
{
    world->dynamicsWorld->stepSimulation(delta, 10);

    std::set<std::pair<void*, void*>> newCollisions;
    size_t manifoldCount = world->dispatcher->getNumManifolds();
    for (size_t i = 0; i < manifoldCount; ++i)
    {
        btPersistentManifold* contactManifold = world->dispatcher->getManifoldByIndexInternal(i);
        btCollisionObject* obA = (btCollisionObject*)contactManifold->getBody0();
        btCollisionObject* obB = (btCollisionObject*)contactManifold->getBody1();
        int contacts = contactManifold->getNumContacts();
        void *a = (gameobject_t*)obA->getUserPointer();
        void *b = (gameobject_t*)obB->getUserPointer();
        
        if (contacts > 0)
            newCollisions.insert(std::pair<void*, void*>(a, b));
    }

    for (std::set<std::pair<void*, void*>>::iterator it = newCollisions.begin(); it != newCollisions.end(); ++it)
    {
        if (world->collisions.find(*it) == world->collisions.end())
        {
            gameobject_on_collision_enter((gameobject_t*)it->first, (gameobject_t*)it->second, scene);
            gameobject_on_collision_enter((gameobject_t*)it->second, (gameobject_t*)it->first, scene);
        }
    }

    for (std::set<std::pair<void*, void*>>::iterator it = world->collisions.begin(); it != world->collisions.end(); ++it)
    {
        if (newCollisions.find(*it) == newCollisions.end())
        {
            gameobject_on_collision_exit((gameobject_t*)it->first, (gameobject_t*)it->second, scene);
            gameobject_on_collision_exit((gameobject_t*)it->second, (gameobject_t*)it->first, scene);
        }
    }

    world->collisions = newCollisions;
}

void physics_world_add(physics_world_t *world, physics_object_t *object)
{
    world->dynamicsWorld->addRigidBody(object->rigidBody);
}

ray_hit_t physics_world_ray_test(physics_world_t *world, glm::vec3 inFrom, glm::vec3 inTo)
{
    btVector3 from(inFrom.x, inFrom.y, inFrom.z);
    btVector3 to(inTo.x, inTo.y, inTo.z);
    btCollisionWorld::ClosestRayResultCallback result(from, to);

    world->dynamicsWorld->rayTest(from, to, result);
    ray_hit_t output;
    if (result.hasHit())
    {
        output.hit = true;
        output.point.x = result.m_hitPointWorld.getX();
        output.point.y = result.m_hitPointWorld.getY();
        output.point.z = result.m_hitPointWorld.getZ();
        output.normal.x = result.m_hitNormalWorld.getX();
        output.normal.y = result.m_hitNormalWorld.getY();
        output.normal.z = result.m_hitNormalWorld.getZ();
        float xDist = inFrom.x - output.point.x;
        float yDist = inFrom.y - output.point.y;
        float zDist = inFrom.z - output.point.z;
        output.dist = sqrt(xDist * xDist + yDist * yDist + zDist * zDist);
    }
    return output;
}

void physics_world_destroy(physics_world_t *world)
{
    delete world->dynamicsWorld;
    delete world->solver;
    delete world->dispatcher;
    delete world->collisionConfiguration;
    delete world->broadphase;
}

void physics_object_init_box(physics_object_t *object, void *user, float mass, float x, float y, float z)
{
    object->collisionShape = new btBoxShape(btVector3(x, y, z));
    physics_object_init(object, user, mass);
}

void physics_object_init_sphere(physics_object_t *object, void *user, float mass, float radius)
{
    object->collisionShape = new btSphereShape(radius);
    physics_object_init(object, user, mass);
}

void physics_object_init_capsule(physics_object_t *object, void *user, float mass, float radius, float height)
{
    object->collisionShape = new btCapsuleShape(radius, height);
    physics_object_init(object, user, mass);
}

void physics_object_init_convex_hull(physics_object_t *object, void *user, float mass)
{
    gameobject_t *gameobject = (gameobject_t*)user;
    if (!gameobject->model)
        return;
    std::vector<btVector3> points;
    for (auto it = gameobject->model->vertices.begin(); it != gameobject->model->vertices.end(); ++it)
    {
        points.push_back(btVector3(it->pos.x, it->pos.y, it->pos.z));
    }
    btConvexHullShape *shape = new btConvexHullShape((btScalar*)points.data(), points.size(), sizeof(btVector3));
    shape->optimizeConvexHull();
    shape->initializePolyhedralFeatures();
    object->collisionShape = shape;
    physics_object_init(object, user, mass);
}

void physics_object_init_mesh(physics_object_t *object, void *user, float mass)
{
    gameobject_t *gameobject = (gameobject_t*)user;
    if (!gameobject->model)
        return;
    btTriangleMesh *mesh = new btTriangleMesh();
    btVector3 verts[3];
    size_t idx = 0;
    for (auto it = gameobject->model->indices.begin(); it != gameobject->model->indices.end(); ++it)
    {
        glm::vec3 vert = gameobject->model->vertices[(*it)].pos;
        verts[idx].setX(vert.x);
        verts[idx].setY(vert.y);
        verts[idx].setZ(vert.z);
        ++idx;
        if (idx == 3)
        {
            idx = 0;
            mesh->addTriangle(verts[0], verts[1], verts[2]);
        }
    }
    object->collisionShape = new btBvhTriangleMeshShape(mesh, true);
    physics_object_init(object, user, mass);
}

void physics_object_init(physics_object_t *object, void *user, float mass)
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
    rigidBodyCI.m_restitution = 0.0f;
    rigidBodyCI.m_friction = 1.0f;

    object->rigidBody = new btRigidBody(rigidBodyCI);
    object->rigidBody->setCollisionFlags(object->rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    object->rigidBody->setActivationState(DISABLE_DEACTIVATION);
    object->rigidBody->setUserPointer(user);
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

glm::vec3 physics_object_get_angular_velocity(physics_object_t *object)
{
    btVector3 velocity = object->rigidBody->getAngularVelocity();
    return glm::vec3(velocity.getX(), velocity.getY(), velocity.getZ());
}

void physics_object_set_angular_velocity(physics_object_t *object, float x, float y, float z)
{
    object->rigidBody->setAngularVelocity(btVector3(x, y, z));
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
    if (object->mesh)
        delete object->mesh;
}

