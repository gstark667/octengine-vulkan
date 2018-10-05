require("gameobject")
require("scene")
require("physics")

function setup(scene, object)
    scene_set_model(scene, object, "sphere.dae")
    gameobject_set_scale(object, 0.5)
    physics_init_sphere(scene, object, 5, 0.5)
end

function update(scene, object, delta)
    x, y, z = gameobject_get_global_transform(object)
    hit, dist = physics_ray_test(scene, x, y, z, x, y - 2.0, z)

    if hit then
        force = (dist / 2.0 - 0.5) * 4
        physics_apply_force(object, 0, -force, 0, 0, 0, 0)
    end
end
