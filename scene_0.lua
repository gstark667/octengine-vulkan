require("scene")
require("gameobject")
require("physics")

function setup(scene, object)
    player = scene_add_gameobject(scene, "capsule.dae")
    scene_add_script(scene, player, "player.lua")
    physics_set_position(player, 0, 2, 5)

    --for x = 0, 10 do
    --    for y = 0, 10 do
    --        cube = scene_add_gameobject(scene, "cube.dae")
    --        gameobject_scale(cube, 1)
    --        gameobject_transform(cube, x*2, y*2, 0)
    --        physics_init_box(scene, cube, 10, 1, 1, 1)
    --    end
    --end

    --cube = scene_add_gameobject(scene, "cube.dae")
    --gameobject_transform(cube, 5, 1, 0)
    --physics_init_box(scene, cube, 1, 1, 1, 1)

    --cube = scene_add_gameobject(scene, "cube.dae")
    --gameobject_transform(cube, 0, 1, 5)

    plane = scene_add_gameobject(scene, "plane.dae")
    gameobject_transform(plane, 0, 0, 0)
    gameobject_scale(plane, 1000)
    physics_init_box(scene, plane, 0, 1000, 0.1, 1000)

    --other2 = scene_add_gameobject(scene, "cube.dae")
    --scene_add_script(scene, other2, "test.lua")
    --gameobject_transform(other2, 6, 0, 0)
    --gameobject_set_parent(other2, other1)
end
