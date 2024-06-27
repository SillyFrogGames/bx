#pragma once

#include "bx/engine/core/type.hpp"
#include "bx/engine/core/math.hpp"
#include "bx/engine/core/macros.hpp"

using PhysicsHandle = u64;
constexpr PhysicsHandle PHYSICS_INVALID_HANDLE = -1;

ENUM(ColliderShape, PLANE, BOX, SPHERE, CAPSULE, MESH);
ENUM(ColliderAxis, AXIS_X, AXIS_Y, AXIS_Z);

ENUM(ColliderFlags,
	DYNAMIC = BX_BIT(0),
	STATIC = BX_BIT(1),
	KINEMATIC = BX_BIT(2),
	CHARACTER = BX_BIT(5),
);

ENUM(CollisionFlags,
	DEFAULT = BX_BIT(1),
	STATIC = BX_BIT(2),
	KINEMATIC = BX_BIT(3),
	DEBRIS = BX_BIT(4),
	TRIGGER = BX_BIT(5),
	CHARACTER = BX_BIT(6),
	ALL = -1
);

struct CastHitResult
{
	bool hasHit = false;

	u64 id = 0;
	Vec3 point = Vec3(0, 0, 0);
	Vec3 normal = Vec3(0, 0, 0);
};

struct ColliderInfo
{
	// User data
	u64 id = 0;

	// Common
	ColliderFlags colliderFlags = ColliderFlags::STATIC;
	CollisionFlags collisionGroup = CollisionFlags::STATIC;
	CollisionFlags collisionMask = CollisionFlags::ALL ^ CollisionFlags::STATIC;

	Mat4 matrix = Mat4::Identity();
	bool isObject = false;

	ColliderShape shape = ColliderShape::BOX;
	
	// Plane info
	Vec3 normal = Vec3(0, 1, 0);
	f32 constant = 1;

	// Box info
	Vec3 center = Vec3(0, 0, 0);
	Vec3 size = Vec3(1, 1, 1);

	// Sphere info
	f32 radius = 1;

	// Capsule info
	f32 height = 1;
	ColliderAxis axis = ColliderAxis::AXIS_Y;

	// Mesh info
	Vec3 scale = Vec3(1, 1, 1);
	bool isConcave = false;
	List<Vec3> vertices;
};

struct RigidBodyInfo
{
	// User data
	u64 id = 0;

	Mat4 matrix = Mat4::Identity();
	PhysicsHandle collider = PHYSICS_INVALID_HANDLE;
};

struct CharacterControllerInfo
{
	// User data
	u64 id = 0;

	ColliderFlags colliderFlags = ColliderFlags::STATIC;
	CollisionFlags collisionGroup = CollisionFlags::CHARACTER;
	CollisionFlags collisionMask = CollisionFlags::DEFAULT | CollisionFlags::STATIC | CollisionFlags::CHARACTER;

	Mat4 matrix = Mat4::Identity();

	Vec3 offset = Vec3(0, 0, 0);
	Vec3 up = Vec3(0, 1, 0);

	f32 width = 0.5f;
	f32 height = 1.8f;

	f32 stepHeight = 0.35f;
};

class Physics
{
public:
	static void Tick();

	static void SetDebugDraw(bool enabled);
	static void DebugDraw();

	static CastHitResult RayCast(const Vec3& origin, const Vec3& direction, f32 distance, CollisionFlags group = CollisionFlags::DEFAULT, CollisionFlags mask = CollisionFlags::ALL);
	static List<CastHitResult> RayCastAll(const Vec3& origin, const Vec3& direction, f32 distance, CollisionFlags group = CollisionFlags::DEFAULT, CollisionFlags mask = CollisionFlags::ALL);
	static CastHitResult SphereCast(const Vec3& origin, const Vec3& direction, f32 distance, f32 radius, CollisionFlags group = CollisionFlags::DEFAULT, CollisionFlags mask = CollisionFlags::ALL);
	static List<CastHitResult> SphereCastAll(const Vec3& origin, const Vec3& direction, f32 distance, f32 radius, CollisionFlags group = CollisionFlags::DEFAULT, CollisionFlags mask = CollisionFlags::ALL);

	static PhysicsHandle CreateCollider(const ColliderInfo& info);
	static void DestroyCollider(const PhysicsHandle collider);
	static Mat4 GetColliderMatrix(const PhysicsHandle collider);
	static void SetColliderMatrix(const PhysicsHandle collider, const Mat4& matrix);

	static PhysicsHandle CreateRigidBody(const RigidBodyInfo& info);
	static void DestroyRigidBody(const PhysicsHandle rigidBody);
	static Mat4 GetRigidBodyMatrix(const PhysicsHandle rigidBody);
	static void SetRigidBodyMatrix(const PhysicsHandle rigidBody, const Mat4& matrix, bool forceAwake = true);

	static PhysicsHandle CreateCharacterController(const CharacterControllerInfo& info);
	static void DestroyCharacterController(const PhysicsHandle characterController);
	static Mat4 GetCharacterControllerMatrix(const PhysicsHandle characterController);
	static void SetCharacterControllerMatrix(const PhysicsHandle characterController, const Mat4& matrix);
	static void SetCharacterControllerMove(const PhysicsHandle characterController, const Vec3& move);
	static void SetCharacterControllerVelocity(const PhysicsHandle characterController, const Vec3& velocity);
	static void SetCharacterControllerRotation(const PhysicsHandle characterController, const Quat& rotation);
	static void CharacterControllerApplyImpulse(const PhysicsHandle characterController, const Vec3& impulse);

private:
	friend class Runtime;

	static void Initialize();
	static void Shutdown();
};

// Proposition to encapsulate handles in wrapper class for ease of use and more OO friendly code
// The following code would work exactly as calling them from Physics
//class PhysicsCollider
//{
//public:
//	void Create(const ColliderInfo& info) const { Physics::CreateCollider(info); }
//	void Destroy(PhysicsCollider collider) const;
//
//	const Mat4& GetMatrix() const;
//  void SetMatrix(const Mat4& matrix) const;
//	
//private:
//	PhysicsHandle m_collider;
//};