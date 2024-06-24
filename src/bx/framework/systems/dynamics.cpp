#include "bx/framework/systems/dynamics.hpp"

#include "bx/framework/components/transform.hpp"
#include "bx/framework/components/collider.hpp"
#include "bx/framework/components/rigidbody.hpp"
#include "bx/framework/components/character_controller.hpp"

void Dynamics::Initialize()
{
}

void Dynamics::Shutdown()
{
}

void Dynamics::Update()
{
    EntityManager::ForEach<Transform>(
        [&](Entity entity, Transform& trx)
        {
            trx.Update();
        });

    EntityManager::ForEach<Transform, Collider>(
        [&](Entity entity, Transform& trx, Collider& coll)
        {
            bool hasRigidBody = entity.HasComponent<RigidBody>();
            coll.Build(!hasRigidBody, trx.GetMatrix());

            if (hasRigidBody)
            {
                auto& rb = entity.GetComponent<RigidBody>();
                if (rb.GetCollider() != coll.GetCollider())
                {
                    rb.SetCollider(coll.GetCollider());
                }

                rb.Build(trx.GetMatrix());
            }
        });

    EntityManager::ForEach<Transform, CharacterController>(
        [&](Entity entity, Transform& trx, CharacterController& cc)
        {
            cc.Build(trx.GetMatrix());
            Physics::SetCharacterControllerMatrix(cc.GetCharacterController(), trx.GetMatrix());
        });

    Physics::Tick();

    EntityManager::ForEach<Transform, RigidBody>(
        [&](Entity entity, Transform& trx, const RigidBody& rb)
        {
            trx.SetMatrix(Physics::GetRigidBodyMatrix(rb.GetRigidBody()));
        });

    EntityManager::ForEach<Transform, Collider>(
        [&](Entity entity, Transform& trx, Collider& coll)
        {
            Physics::SetColliderMatrix(coll.GetCollider(), trx.GetMatrix());
        });

    EntityManager::ForEach<Transform, CharacterController>(
        [&](Entity entity, Transform& trx, CharacterController& cc)
        {
            trx.SetMatrix(Physics::GetCharacterControllerMatrix(cc.GetCharacterController()));
        });
}

void Dynamics::Render()
{
	Physics::DebugDraw();
}