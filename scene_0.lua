require("scene")
require("gameobject")

function setup(scene, object)
    camera = scene_add_gameobject(scene, "")
    scene_add_script(scene, camera, "camera.lua")

    other = scene_add_gameobject(scene, "example.dae")
    scene_add_script(scene, other, "test.lua")
    gameobject_transform(other, 1, 0, 0)

    other = scene_add_gameobject(scene, "cube.dae")
    scene_add_script(scene, other, "test.lua")
    gameobject_transform(other, 2, 0, 0)
end
