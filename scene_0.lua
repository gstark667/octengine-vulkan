require("scene")
require("gameobject")
require("physics")

function setup(scene, object)
    camera = scene_add_gameobject(scene, "")
    scene_add_script(scene, camera, "camera.lua")
    gameobject_transform(camera, 0, 2, 10)

    for i = 1, 1000 do
        cube = scene_add_gameobject(scene, "cube.dae")
        gameobject_scale(cube, 1)
        gameobject_transform(cube, math.random(-500, 500)/100, math.random(4000, 10000)/100, math.random(-500, 500)/100)
        physics_init_box(scene, cube, 1, 1, 1, 1)
    end

    plane = scene_add_gameobject(scene, "plane.dae")
    gameobject_transform(plane, 0, 0, 0)
    gameobject_scale(plane, 100)
    physics_init_box(scene, plane, 0, 100, 0.01, 100)

    --other2 = scene_add_gameobject(scene, "cube.dae")
    --scene_add_script(scene, other2, "test.lua")
    --gameobject_transform(other2, 6, 0, 0)
    --gameobject_set_parent(other2, other1)
end
