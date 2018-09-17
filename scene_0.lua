require("scene")
require("gameobject")

function setup(scene, object)
    camera = scene_add_gameobject(scene, "cube.dae")
    scene_add_script(scene, camera, "camera.lua")

    other1 = scene_add_gameobject(scene, "example.dae")
    scene_add_script(scene, other1, "test.lua")
    gameobject_transform(other1, 0, 0, 0)

    other2 = scene_add_gameobject(scene, "cube.dae")
    --scene_add_script(scene, other2, "test.lua")
    gameobject_transform(other2, 6, 0, 0)
    gameobject_set_parent(other2, other1)
end
