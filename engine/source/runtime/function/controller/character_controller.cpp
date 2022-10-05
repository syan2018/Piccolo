#include "runtime/function/controller/character_controller.h"

#include "runtime/core/base/macro.h"

#include "runtime/function/framework/component/motor/motor_component.h"
#include "runtime/function/framework/world/world_manager.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/physics/physics_scene.h"

#include "runtime/function/framework/component/motor/motor_climb.h"

namespace Piccolo
{
    CharacterController::CharacterController(const Capsule& capsule) : m_capsule(capsule)
    {
        m_rigidbody_shape                                    = RigidBodyShape();
        m_rigidbody_shape.m_geometry                         = PICCOLO_REFLECTION_NEW(Capsule);
        *static_cast<Capsule*>(m_rigidbody_shape.m_geometry) = m_capsule;

        m_rigidbody_shape.m_type = RigidBodyShapeType::capsule;

        Quaternion orientation;
        orientation.fromAngleAxis(Radian(Degree(90.f)), Vector3::UNIT_X);

        m_rigidbody_shape.m_local_transform =
            Transform(
                Vector3(0, 0, capsule.m_half_height + capsule.m_radius),
                orientation,
                Vector3::UNIT_SCALE);
    }

    Vector3 CharacterController::move(const Vector3& current_position, const Vector3& displacement)
    {
        // 获取场景
        std::shared_ptr<PhysicsScene> physics_scene =
            g_runtime_global_context.m_world_manager->getCurrentActivePhysicsScene().lock();
        ASSERT(physics_scene);

        

        std::vector<PhysicsHitInfo> hits;

        // 为啥要叠高Z？
        // 取单位在场景系中Transform
        Transform world_transform = Transform(
            current_position + 0.1f * Vector3::UNIT_Z,
            Quaternion::IDENTITY,
            Vector3::UNIT_SCALE);


        // 取位置更新向量
        Vector3 vertical_displacement   = displacement.z * Vector3::UNIT_Z;
        Vector3 horizontal_displacement = Vector3(displacement.x, displacement.y, 0.f);

        Vector3 vertical_direction   = vertical_displacement.normalisedCopy();
        Vector3 horizontal_direction = horizontal_displacement.normalisedCopy();


        // 位置更新Part
        
        Vector3 final_position = current_position;


        // 输入角色碰撞体、角色位置、检测方向、检测距离
        // 角色位置虚空抬了0.1，Z轴下探检测0.105用于检测地板

        m_is_touch_ground = physics_scene->sweep(
            m_rigidbody_shape,
            world_transform.getMatrix(),
            Vector3::NEGATIVE_UNIT_Z,
            0.105f,
            hits);

        hits.clear();
        
        world_transform.m_position -= 0.1f * Vector3::UNIT_Z;


        // 垂向触地
        // vertical pass

        bool has_vertical_hits = physics_scene->sweep(
            m_rigidbody_shape,
            world_transform.getMatrix(),
            vertical_direction,
            vertical_displacement.length(),
            hits);

        // 如果触地则卡在第一个碰撞位
        if (has_vertical_hits)
        {
            
            final_position += hits[0].hit_distance * vertical_direction;
        }
        else
        {
            final_position += vertical_displacement;
        }

        hits.clear();


        // side pass

        bool has_horizontal_hits = physics_scene->sweep(
            m_rigidbody_shape,
            world_transform.getMatrix(),
            horizontal_direction,
            horizontal_displacement.length(),
            hits);

        if (has_horizontal_hits)
        {

            Piccolo::PhysicsHitInfo hitInfo = hits[0];

            // 如果碰撞位置低于翻越Z，则保持移动并抬高
            if (hitInfo.hit_position.z < MotorClimb::GetClimbZ())
            {
                final_position += horizontal_displacement;
                final_position += Vector3::UNIT_Z * hitInfo.hit_position.z;

                m_is_touch_ground = true;
            }
            else
            {
                // 朝无碰撞方向前进
                final_position += 
                    horizontal_displacement.length() 
                    * (horizontal_direction - hitInfo.hit_normal);
            }

        }
        else
        {
            final_position += horizontal_displacement;
        }


        return final_position;
    }

} // namespace Piccolo
