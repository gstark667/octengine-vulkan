require("gameobject")
require("scene")
require("physics")

function setup(scene, object)
end

function update(scene, object, delta)
    x, y, z = gameobject_get_global_transform(object)
    hit, dist = physics_ray_test(scene, x, y, z, x, y - 2.0, z)

    if hit then
        force = (dist / 2.0 - 0.5) * 1
        parent = gameobject_get_parent(object)
        px, py, pz = gameobject_get_global_transform(parent)
        physics_apply_force(parent, 0, -force, 0, x-px, y-py, z-pz)
    end
end
