require("gameobject")
require("scene")
require("physics")

function setup(scene, object)
    scene_set_model(scene, object, "plane.dae")
    physics_init_box(scene, object, 5, 1.0, 0.25, 1.0)
    physics_set_angular_factor(object, 0.5)

    f1 = scene_add_gameobject(scene)
    gameobject_transform(f1, -0.9, 0, -0.9)
    gameobject_scale(f1, 0.1)
    gameobject_set_parent(f1, object)
    scene_set_model(scene, f1, "sphere.dae")
    scene_add_script(scene, f1, "test.lua")

    f2 = scene_add_gameobject(scene)
    gameobject_transform(f2, 0.9, 0, -0.9)
    gameobject_scale(f2, 0.1)
    gameobject_set_parent(f2, object)
    scene_set_model(scene, f2, "sphere.dae")
    scene_add_script(scene, f2, "test.lua")

    f3 = scene_add_gameobject(scene)
    gameobject_transform(f3, -0.9, 0, 0.9)
    gameobject_scale(f3, 0.1)
    gameobject_set_parent(f3, object)
    scene_set_model(scene, f3, "sphere.dae")
    scene_add_script(scene, f3, "test.lua")

    f4 = scene_add_gameobject(scene)
    gameobject_transform(f4, 0.9, 0, 0.9)
    gameobject_scale(f4, 0.1)
    gameobject_set_parent(f4, object)
    scene_set_model(scene, f4, "sphere.dae")
    scene_add_script(scene, f4, "test.lua")
end

function update(scene, object, delta)

end
