#include "bx/engine/modules/physics.hpp"

#include "bx/engine/core/macros.hpp"
#include "bx/engine/core/time.hpp"
#include "bx/engine/containers/hash_map.hpp"
#include "bx/engine/modules/graphics.hpp"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

class btDebugDraw;
class btOffsetCapsuleShape;

static btBroadphaseInterface* g_broadphase = nullptr;
static btDefaultCollisionConfiguration* g_collisionConfiguration = nullptr;
static btCollisionDispatcher* g_dispatcher = nullptr;
static btSequentialImpulseConstraintSolver* g_solver = nullptr;
static btDiscreteDynamicsWorld* g_dynamicsWorld = nullptr;
static btAxisSweep3* g_sweepBP = nullptr;
static const float g_fixedTimeStep = 1.0f / 60.0f;
static const int g_maxSubSteps = 3;
static f32 g_accumulator = 0.0;

static bool g_debugDrawEnabled = false;
static btDebugDraw* g_debugDraw = nullptr;

struct ColliderImpl
{
    btCollisionShape* collisionShape = nullptr;
    btCollisionObject* collisionObject = nullptr;
};

struct RigidBodyImpl
{
    btRigidBody* rigidBody = nullptr;
};

struct CharacterControllerImpl
{
    btConvexShape* collisionShape = nullptr;
    btPairCachingGhostObject* ghostObject = nullptr;
    btKinematicCharacterController* characterController = nullptr;
};

static HashMap<PhysicsHandle, ColliderImpl> g_colliders;
static HashMap<PhysicsHandle, RigidBodyImpl> g_rigidBodies;
static HashMap<PhysicsHandle, CharacterControllerImpl> g_characterControllers;

template <typename T>
static T& GetImpl(PhysicsHandle handle, HashMap<PhysicsHandle, T>& map)
{
    auto it = map.find(handle);
    ENGINE_ENSURE(it != map.end());
    return it->second;
}

static Vec3 ConvertVec3(const btVector3& v)
{
    return Vec3(v.getX(), v.getY(), v.getZ());
}

static btVector3 ConvertVec3(const Vec3& v)
{
    return btVector3(v.x, v.y, v.z);
}

static Quat ConvertQuat(const btQuaternion& q)
{
    return Quat(q.getX(), q.getY(), q.getZ(), q.getW());
}

static btQuaternion ConvertQuat(const Quat& q)
{
    return btQuaternion(q.x, q.y, q.z, q.w);
}

static Mat4 ConvertMat4(const btTransform& trx)
{
    return Mat4::TRS(ConvertVec3(trx.getOrigin()), ConvertQuat(trx.getRotation()), Vec3(1, 1, 1));
}

static btTransform ConvertMat4(const Mat4& trx)
{
    Vec3 p; Quat r; Vec3 s;
    Mat4::Decompose(trx, p, r, s);
    return btTransform(ConvertQuat(r), ConvertVec3(p));
}

static u32 ConvertColor(const btVector3& color)
{
    u8 r = static_cast<u8>(color.getX() * 255);
    u8 g = static_cast<u8>(color.getY() * 255);
    u8 b = static_cast<u8>(color.getZ() * 255);
    return 0xFF000000 | ((r << 16) | (g << 8) | b);
}

static void PackID(btCollisionObject* obj, u64 value)
{
    obj->setUserIndex(static_cast<i32>(value & 0xFFFFFFFF));
    obj->setUserIndex2(static_cast<i32>((value >> 32) & 0xFFFFFFFF));
}

static u64 UnpackID(const btCollisionObject* obj)
{
    u64 lower = static_cast<u32>(obj->getUserIndex());
    u64 upper = static_cast<u32>(obj->getUserIndex2());
    return (upper << 32) | lower;
}

class btDebugDraw : public btIDebugDraw
{
    int m_debugMode;

public:
    btDebugDraw() : m_debugMode(DBG_DrawWireframe | DBG_DrawContactPoints) {}

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
    {
        if (!g_debugDrawEnabled)
            return;

        Graphics::DebugLine(ConvertVec3(from), ConvertVec3(to), ConvertColor(color));
    }

    virtual void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override
    {
        if (!g_debugDrawEnabled)
            return;

        Vec3 start = ConvertVec3(pointOnB);
        Vec3 end = start + ConvertVec3(normalOnB) * distance;
        Graphics::DebugLine(start, end, ConvertColor(color), (f32)lifeTime);
    }

    virtual void reportErrorWarning(const char* warningString) override
    {
        ENGINE_LOGW("Bullet Warning: {}", warningString);
    }

    virtual void draw3dText(const btVector3& location, const char* textString) override
    {
        // TODO: Not supported
    }

    virtual void setDebugMode(int debugMode) override
    {
        m_debugMode = debugMode;
    }

    virtual int getDebugMode() const override
    {
        return m_debugMode;
    }
};

void Physics::Initialize()
{
    // Initialize Bullet
    btVector3 worldMin(-1000, -1000, -1000);
    btVector3 worldMax(1000, 1000, 1000);
    g_sweepBP = new btAxisSweep3(worldMin, worldMax);

    g_broadphase = g_sweepBP;// new btDbvtBroadphase();
    g_collisionConfiguration = new btDefaultCollisionConfiguration();
    g_dispatcher = new btCollisionDispatcher(g_collisionConfiguration);
    g_solver = new btSequentialImpulseConstraintSolver();

    g_dynamicsWorld = new btDiscreteDynamicsWorld(g_dispatcher, g_broadphase, g_solver, g_collisionConfiguration);
    g_dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
    //g_dynamicsWorld->setGravity(btVector3(0, 0, 0));

    g_sweepBP->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

    g_debugDraw = new btDebugDraw();
    g_dynamicsWorld->setDebugDrawer(g_debugDraw);
}

void Physics::Shutdown()
{
    delete g_dynamicsWorld;
    delete g_solver;
    delete g_dispatcher;
    delete g_collisionConfiguration;
    delete g_broadphase;
    delete g_debugDraw;

    g_dynamicsWorld = nullptr;
    g_solver = nullptr;
    g_dispatcher = nullptr;
    g_collisionConfiguration = nullptr;
    g_broadphase = nullptr;
    g_debugDraw = nullptr;
}

void Physics::Tick()
{
    //g_accumulator += Time::GetDeltaTime();
    //while (g_accumulator >= g_fixedTimeStep)
    {
        //g_dynamicsWorld->stepSimulation(Time::GetDeltaTime(), g_maxSubSteps, g_fixedTimeStep);
        //g_accumulator -= g_fixedTimeStep;
    }

    g_dynamicsWorld->stepSimulation(g_fixedTimeStep, g_maxSubSteps);
    for (const auto& entry : g_characterControllers)
    {
        const auto ghostObject = entry.second.ghostObject;
        g_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(ghostObject->getBroadphaseHandle(), g_dynamicsWorld->getDispatcher());
    }
}

void Physics::SetDebugDraw(bool enabled)
{
    g_debugDrawEnabled = enabled;
}

void Physics::DebugDraw()
{
    g_dynamicsWorld->debugDrawWorld();
}

CastHitResult Physics::RayCast(const Vec3& origin, const Vec3& direction, f32 distance, CollisionFlags group, CollisionFlags mask)
{
    CastHitResult hit;

    btVector3 start = ConvertVec3(origin);
    btVector3 end = ConvertVec3(origin + direction * distance);

    btCollisionWorld::ClosestRayResultCallback resultCallback(start, end);
    resultCallback.m_collisionFilterGroup = group;
    resultCallback.m_collisionFilterMask = mask;

    g_dynamicsWorld->rayTest(start, end, resultCallback);
    if (resultCallback.hasHit())
    {
        hit.hasHit = true;

        const btCollisionObject* hitObject = resultCallback.m_collisionObject;
        hit.id = UnpackID(hitObject);
        hit.point = ConvertVec3(resultCallback.m_hitPointWorld);
        hit.normal = ConvertVec3(resultCallback.m_hitNormalWorld);
    }

    return hit;
}

List<CastHitResult> Physics::RayCastAll(const Vec3& origin, const Vec3& direction, f32 distance, CollisionFlags group, CollisionFlags mask)
{
    List<CastHitResult> hits;

    btVector3 start = ConvertVec3(origin);
    btVector3 end = ConvertVec3(origin + direction * distance);

    btCollisionWorld::AllHitsRayResultCallback resultCallback(start, end);
    resultCallback.m_collisionFilterGroup = group;
    resultCallback.m_collisionFilterMask = mask;

    g_dynamicsWorld->rayTest(start, end, resultCallback);
    if (resultCallback.hasHit())
    {
        for (SizeType i = 0; i < resultCallback.m_hitPointWorld.size(); ++i)
        {
            CastHitResult hit;
            hit.hasHit = true;
            
            const btCollisionObject* hitObject = resultCallback.m_collisionObjects[i];
            hit.id = UnpackID(hitObject);
            hit.point = ConvertVec3(resultCallback.m_hitPointWorld[i]);
            hit.normal = ConvertVec3(resultCallback.m_hitNormalWorld[i]);

            hits.emplace_back(hit);
        }
    }

    return hits;
}

CastHitResult Physics::SphereCast(const Vec3& origin, const Vec3& direction, f32 distance, f32 radius, CollisionFlags group, CollisionFlags mask)
{
    CastHitResult hit;

    btTransform start;
    start.setIdentity();
    start.setOrigin(ConvertVec3(origin));

    btTransform end;
    end.setIdentity();
    end.setOrigin(ConvertVec3(origin + direction * distance));

    btSphereShape sphereShape(radius);

    btCollisionWorld::ClosestConvexResultCallback resultCallback(start.getOrigin(), end.getOrigin());
    resultCallback.m_collisionFilterGroup = group;
    resultCallback.m_collisionFilterMask = mask;

    g_dynamicsWorld->convexSweepTest(&sphereShape, start, end, resultCallback);
    if (resultCallback.hasHit())
    {
        hit.hasHit = true;

        const btCollisionObject* hitObject = resultCallback.m_hitCollisionObject;
        hit.id = UnpackID(hitObject);
        hit.point = ConvertVec3(resultCallback.m_hitPointWorld);
        hit.normal = ConvertVec3(resultCallback.m_hitNormalWorld);
    }

    return hit;
}

struct AllHitsConvexResultCallback : public btCollisionWorld::ConvexResultCallback
{
    AllHitsConvexResultCallback(const btVector3& convexFromWorld, const btVector3& convexToWorld)
        : m_convexFromWorld(convexFromWorld),
        m_convexToWorld(convexToWorld),
        m_hitCollisionObject(0)
    {
    }

    const btCollisionObject* m_hitCollisionObject;
    btAlignedObjectArray<const btCollisionObject*> m_hitCollisionObjects;

    btVector3 m_convexFromWorld;  //used to calculate hitPointWorld from hitFraction
    btVector3 m_convexToWorld;

    btAlignedObjectArray<btVector3> m_hitNormalWorld;
    btAlignedObjectArray<btVector3> m_hitPointWorld;
    btAlignedObjectArray<btScalar> m_hitFractions;

    virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
    {
        m_hitCollisionObject = convexResult.m_hitCollisionObject;
        m_hitCollisionObjects.push_back(convexResult.m_hitCollisionObject);
        btVector3 hitNormalWorld;
        if (normalInWorldSpace)
        {
            hitNormalWorld = convexResult.m_hitNormalLocal;
        }
        else
        {
            ///need to transform normal into worldspace
            hitNormalWorld = m_hitCollisionObject->getWorldTransform().getBasis() * convexResult.m_hitNormalLocal;
        }
        m_hitNormalWorld.push_back(hitNormalWorld);
        m_hitPointWorld.push_back(convexResult.m_hitPointLocal);
        m_hitFractions.push_back(convexResult.m_hitFraction);
        return m_closestHitFraction;
    }
};

List<CastHitResult> Physics::SphereCastAll(const Vec3& origin, const Vec3& direction, f32 distance, f32 radius, CollisionFlags group, CollisionFlags mask)
{
    List<CastHitResult> hits;

    btTransform start;
    start.setIdentity();
    start.setOrigin(ConvertVec3(origin));

    btTransform end;
    end.setIdentity();
    end.setOrigin(ConvertVec3(origin + direction * distance));

    btSphereShape sphereShape(radius);

    AllHitsConvexResultCallback resultCallback(start.getOrigin(), end.getOrigin());
    resultCallback.m_collisionFilterGroup = group;
    resultCallback.m_collisionFilterMask = mask;

    g_dynamicsWorld->convexSweepTest(&sphereShape, start, end, resultCallback);
    if (resultCallback.m_hitPointWorld.size() > 0)
    {
        for (SizeType i = 0; i < resultCallback.m_hitPointWorld.size(); ++i)
        {
            CastHitResult hit;
            hit.hasHit = true;

            const btCollisionObject* hitObject = resultCallback.m_hitCollisionObjects[i];
            hit.id = UnpackID(hitObject);
            hit.point = ConvertVec3(resultCallback.m_hitPointWorld[i]);
            hit.normal = ConvertVec3(resultCallback.m_hitNormalWorld[i]);

            hits.emplace_back(hit);
        }
    }

    return hits;
}

PhysicsHandle Physics::CreateCollider(const ColliderInfo& info)
{
    ColliderImpl collider_impl;

    btCompoundShape* compoundShape = new btCompoundShape();
    collider_impl.collisionShape = compoundShape;

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(ConvertVec3(info.center));

    switch (info.shape)
    {
    case ColliderShape::PLANE:
        compoundShape->addChildShape(transform, new btStaticPlaneShape(ConvertVec3(info.normal), info.constant));
        break;

    case ColliderShape::BOX:
        compoundShape->addChildShape(transform, new btBoxShape(ConvertVec3(info.size)));
        break;

    case ColliderShape::SPHERE:
        compoundShape->addChildShape(transform, new btSphereShape(info.radius));
        break;

    case ColliderShape::CAPSULE:
        switch (info.axis)
        {
        case ColliderAxis::AXIS_X:
            compoundShape->addChildShape(transform, new btCapsuleShapeX(info.radius, info.height));
            break;
        case ColliderAxis::AXIS_Y:
            compoundShape->addChildShape(transform, new btCapsuleShape(info.radius, info.height));
            break;
        case ColliderAxis::AXIS_Z:
            compoundShape->addChildShape(transform, new btCapsuleShapeZ(info.radius, info.height));
            break;
        }
        break;

    case ColliderShape::MESH:
    {
        if (info.isConcave)
        {
            if (!info.vertices.empty())
            {
                btTriangleMesh* triangleMesh = new btTriangleMesh();
                for (SizeType i = 0; i < info.vertices.size(); i += 3)
                {
                    const Vec3& v1 = info.vertices[i];
                    const Vec3& v2 = info.vertices[i + 1];
                    const Vec3& v3 = info.vertices[i + 2];
                    triangleMesh->addTriangle(
                        ConvertVec3(Vec3(v1.x, v1.z, -v1.y)),
                        ConvertVec3(Vec3(v2.x, v2.z, -v2.y)),
                        ConvertVec3(Vec3(v3.x, v3.z, -v3.y)));
                }
                btBvhTriangleMeshShape* triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true);
                triangleMeshShape->setLocalScaling(ConvertVec3(info.scale));
                compoundShape->addChildShape(transform, triangleMeshShape);
            }
        }
        else
        {
            btConvexHullShape* convexHullShape = new btConvexHullShape();
            for (const auto& v : info.vertices)
            {
                convexHullShape->addPoint(btVector3(ConvertVec3(Vec3(v.x, v.z, -v.y))));
            }
            convexHullShape->setLocalScaling(ConvertVec3(info.scale));
            compoundShape->addChildShape(transform, convexHullShape);
        }
        break;
    }
    default:
        break;
    }

    if (info.isObject)
    {
        collider_impl.collisionObject = new btCollisionObject();
        collider_impl.collisionObject->setCollisionShape(collider_impl.collisionShape);
        collider_impl.collisionObject->setWorldTransform(ConvertMat4(info.matrix));
        collider_impl.collisionObject->setCollisionFlags(info.colliderFlags);
        PackID(collider_impl.collisionObject, info.id);
        g_dynamicsWorld->addCollisionObject(collider_impl.collisionObject, info.collisionGroup, info.collisionMask);
    }

    static PhysicsHandle counter = 0;
    PhysicsHandle handle = counter++;
    g_colliders.insert(std::make_pair(handle, collider_impl));
    return handle;
}

void Physics::DestroyCollider(const PhysicsHandle collider)
{
    auto it = g_colliders.find(collider);
    if (it == g_colliders.end())
        return;

    auto& collider_impl = it->second;

    if (collider_impl.collisionObject != nullptr)
        g_dynamicsWorld->removeCollisionObject(collider_impl.collisionObject);
    delete collider_impl.collisionShape;
    delete collider_impl.collisionObject;

    g_colliders.erase(it);
}

Mat4 Physics::GetColliderMatrix(const PhysicsHandle collider)
{
    const auto& collider_impl = GetImpl(collider, g_colliders);

    if (collider_impl.collisionObject == nullptr)
        return Mat4::Identity();

    return ConvertMat4(collider_impl.collisionObject->getWorldTransform());
}

void Physics::SetColliderMatrix(const PhysicsHandle collider, const Mat4& matrix)
{
    auto& collider_impl = GetImpl(collider, g_colliders);

    if (collider_impl.collisionObject == nullptr)
        return;

    collider_impl.collisionObject->setWorldTransform(ConvertMat4(matrix));
}

PhysicsHandle Physics::CreateRigidBody(const RigidBodyInfo& info)
{
    const auto& collider_impl = GetImpl(info.collider, g_colliders);
    RigidBodyImpl rigidBody_impl;

    btScalar mass = 1;
    btVector3 inertia(0, 0, 0);
    collider_impl.collisionShape->calculateLocalInertia(mass, inertia);

    btDefaultMotionState* motionState = new btDefaultMotionState(ConvertMat4(info.matrix));
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, collider_impl.collisionShape, inertia);
    rigidBody_impl.rigidBody = new btRigidBody(rigidBodyCI);
    PackID(rigidBody_impl.rigidBody, info.id);

    g_dynamicsWorld->addRigidBody(rigidBody_impl.rigidBody);

    static PhysicsHandle counter = 0;
    PhysicsHandle handle = counter++;
    g_rigidBodies.insert(std::make_pair(handle, rigidBody_impl));
    return handle;
}

void Physics::DestroyRigidBody(const PhysicsHandle rigidBody)
{
    auto it = g_rigidBodies.find(rigidBody);
    if (it == g_rigidBodies.end())
        return;

    auto& rigidBody_impl = it->second;
    g_dynamicsWorld->removeRigidBody(rigidBody_impl.rigidBody);
    delete rigidBody_impl.rigidBody->getMotionState();
    delete rigidBody_impl.rigidBody;

    g_rigidBodies.erase(it);
}

Mat4 Physics::GetRigidBodyMatrix(const PhysicsHandle rigidBody)
{
    const auto& rigidBody_impl = GetImpl(rigidBody, g_rigidBodies);
    const auto& trx = rigidBody_impl.rigidBody->getWorldTransform();
    return ConvertMat4(trx);
}

void Physics::SetRigidBodyMatrix(const PhysicsHandle rigidBody, const Mat4& matrix, bool forceAwake)
{
    const auto& rigidBody_impl = GetImpl(rigidBody, g_rigidBodies);
    rigidBody_impl.rigidBody->setWorldTransform(ConvertMat4(matrix));

    rigidBody_impl.rigidBody->activate(forceAwake);
}

class btOffsetCapsuleShape : public btCapsuleShape
{
    btVector3 m_offset;

public:
    btOffsetCapsuleShape(btScalar radius, btScalar height, const btVector3& offset)
        : btCapsuleShape(radius, height)
        , m_offset(offset)
    {
        m_shapeType = CUSTOM_CONVEX_SHAPE_TYPE;
    }
    
    virtual void getAabb(const btTransform& t, btVector3& aabbMin, btVector3& aabbMax) const override
    {
        btTransform offsetTransform = t;
        offsetTransform.getOrigin() += m_offset;
        btCapsuleShape::getAabb(offsetTransform, aabbMin, aabbMax);
    }

    virtual void batchedUnitVectorGetSupportingVertexWithoutMargin(const btVector3* vectors, btVector3* supportVerticesOut, int numVectors) const override
    {
        btCapsuleShape::batchedUnitVectorGetSupportingVertexWithoutMargin(vectors, supportVerticesOut, numVectors);
    
        for (int i = 0; i < numVectors; ++i)
        {
            supportVerticesOut[i] += m_offset;
        }
    }
    
    virtual btVector3 localGetSupportingVertexWithoutMargin(const btVector3& vec) const override
    {
        return btCapsuleShape::localGetSupportingVertexWithoutMargin(vec) + m_offset;
    }
    
    virtual btVector3 localGetSupportingVertex(const btVector3& vec) const override
    {
        return btCapsuleShape::localGetSupportingVertex(vec) + m_offset;
    }
    
    virtual void project(const btTransform& trans, const btVector3& dir, btScalar& minProj, btScalar& maxProj, btVector3& witnesPtMin, btVector3& witnesPtMax) const override
    {
        btTransform offsetTransform = trans;
        offsetTransform.getOrigin() += m_offset;
        btCapsuleShape::project(offsetTransform, dir, minProj, maxProj, witnesPtMin, witnesPtMax);
    }

    virtual const char* getName() const override
    {
        return "OffsetCapsuleShape";
    }
};

PhysicsHandle Physics::CreateCharacterController(const CharacterControllerInfo& info)
{
    CharacterControllerImpl characterController_impl;

    characterController_impl.collisionShape = new btOffsetCapsuleShape(info.width, info.height, ConvertVec3(info.offset));

    characterController_impl.ghostObject = new btPairCachingGhostObject();
    characterController_impl.ghostObject->setWorldTransform(ConvertMat4(info.matrix));
    characterController_impl.ghostObject->setCollisionShape(characterController_impl.collisionShape);
    characterController_impl.ghostObject->setCollisionFlags(info.colliderFlags);
    PackID(characterController_impl.ghostObject, info.id);

    characterController_impl.characterController = new btKinematicCharacterController(characterController_impl.ghostObject, characterController_impl.collisionShape, info.stepHeight, ConvertVec3(info.up));
    characterController_impl.characterController->setUseGhostSweepTest(true);
    
    g_dynamicsWorld->addCollisionObject(characterController_impl.ghostObject, info.collisionGroup, info.collisionMask);
    g_dynamicsWorld->addAction(characterController_impl.characterController);

    static PhysicsHandle counter = 0;
    PhysicsHandle handle = counter++;
    g_characterControllers.insert(std::make_pair(handle, characterController_impl));
    return handle;
}

void Physics::DestroyCharacterController(const PhysicsHandle characterController)
{
    auto it = g_characterControllers.find(characterController);
    if (it == g_characterControllers.end())
        return;

    auto& characterController_impl = it->second;

    g_dynamicsWorld->removeAction(characterController_impl.characterController);
    g_dynamicsWorld->removeCollisionObject(characterController_impl.ghostObject);
    delete characterController_impl.characterController;
    delete characterController_impl.ghostObject;
    delete characterController_impl.collisionShape;

    g_characterControllers.erase(it);
}

Mat4 Physics::GetCharacterControllerMatrix(const PhysicsHandle characterController)
{
    const auto& characterController_impl = GetImpl(characterController, g_characterControllers);
    const auto& trx = characterController_impl.ghostObject->getWorldTransform();
    return ConvertMat4(trx);
}

void Physics::SetCharacterControllerMatrix(const PhysicsHandle characterController, const Mat4& matrix)
{
    const auto& characterController_impl = GetImpl(characterController, g_characterControllers);
    characterController_impl.ghostObject->setWorldTransform(ConvertMat4(matrix));
}

void Physics::SetCharacterControllerMove(const PhysicsHandle characterController, const Vec3& move)
{
    const auto& characterController_impl = GetImpl(characterController, g_characterControllers);
    characterController_impl.characterController->setWalkDirection(ConvertVec3(move));
}

void Physics::SetCharacterControllerVelocity(const PhysicsHandle characterController, const Vec3& velocity)
{
    const auto& characterController_impl = GetImpl(characterController, g_characterControllers);
    characterController_impl.characterController->setLinearVelocity(ConvertVec3(velocity));
}

void Physics::SetCharacterControllerRotation(const PhysicsHandle characterController, const Quat& rotation)
{
    const auto& characterController_impl = GetImpl(characterController, g_characterControllers);
    btTransform& transform = characterController_impl.ghostObject->getWorldTransform();
    transform.setRotation(ConvertQuat(rotation));
}

void Physics::CharacterControllerApplyImpulse(const PhysicsHandle characterController, const Vec3& impulse)
{
    const auto& characterController_impl = GetImpl(characterController, g_characterControllers);
    characterController_impl.characterController->applyImpulse(ConvertVec3(impulse));
}