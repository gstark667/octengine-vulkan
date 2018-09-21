require("scene")
require("gameobject")
require("physics")

function setup(scene, object)
    player = scene_add_gameobject(scene, "capsule.dae")
    scene_add_script(scene, player, "player.lua")
    physics_set_position(player, 0, 2, 5)

    for x = 0, 10 do
        for y = 0, 10 do
          for i = 1, 2 do
            cube = scene_add_gameobject(scene, "scifi_cube.dae")
            gameobject_scale(cube, 1)
            gameobject_transform(cube, x*1, i, y*1)
            physics_init_box(scene, cube, 10, 0.5, 0.5, 0.5)
          end
        end
    end

    cube = scene_add_gameobject(scene, "cube.dae")
    gameobject_transform(cube, 5, 1, 0)
    physics_init_box(scene, cube, 1, 0.5, 0.5, 0.5)

    cube = scene_add_gameobject(scene, "scifi_cube.dae")
    gameobject_transform(cube, 5, 2, 0)
    physics_init_box(scene, cube, 1, 0.5, 0.5, 0.5)

    --cube = scene_add_gameobject(scene, "cube.dae")
    --gameobject_transform(cube, 0, 1, 5)

    plane = scene_add_gameobject(scene, "plane.dae")
    gameobject_transform(plane, 0, 0, 0)
    gameobject_scale(plane, 50)
    physics_init_box(scene, plane, 0, 50, 0.1, 50)

    --other2 = scene_add_gameobject(scene, "cube.dae")
    --scene_add_script(scene, other2, "test.lua")
    --gameobject_transform(other2, 6, 0, 0)
    --gameobject_set_parent(other2, other1)
end
