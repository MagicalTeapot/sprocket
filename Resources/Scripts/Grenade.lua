function Init(entity)
    TIME = 0.0
    EXPLODE = false
end

function OnUpdate(entity, dt)
    TIME = TIME + dt

    local scene = Scene()
    
    if EXPLODE == false and TIME > 1 then
        local pc = GetParticleComponent(entity)
        pc.interval = 0.0001
        pc.scale = Vec3(5, 5, 5)
        pc.velocity = GetRigidBody3DComponent(entity).velocity
        pc.velocityNoise = 3
        pc.acceleration = Vec3(0, -9.81, 0)
        SetParticleComponent(entity, pc)

        EXPLODE = true
        return
        --DeleteEntity(entity)
    end
    
    if EXPLODE == true then
        for e in scene:Each() do
            local pos = GetTransformComponent(entity).position
            if HasRigidBody3DComponent(e) then
                local rbc = GetRigidBody3DComponent(e)
                if rbc.frozen == false then
                    local e_pos = GetTransformComponent(e).position
                    if Mag(e_pos - pos) < 10 then
                        rbc.velocity = rbc.velocity + 5 * (e_pos - pos)
                    end
                end
                SetRigidBody3DComponent(e, rbc)
            end
        end
        DeleteEntity(entity)
    end

end