require("scene")
require("gameobject")
require("physics")

function setup(scene, object)
    player = scene_add_gameobject(scene)
    scene_add_script(scene, player, "player.lua")
    physics_set_position(player, 0, 10, 5)

    for x = 0, 10 do
        for y = 0, 10 do
          for i = 1, 2 do
            cube = scene_add_gameobject(scene)
            scene_set_model(scene, cube, "scifi_cube.dae")
            gameobject_scale(cube, 1)
            gameobject_transform(cube, x*1, i, y*1)
            physics_init_box(scene, cube, 10, 0.5, 0.5, 0.5)
          end
        end
    end

    plane = scene_add_gameobject(scene)
    scene_set_model(scene, plane, "plane.dae")
    scene_set_texture(scene, plane, "normal.png")
    gameobject_transform(plane, 0, 0, 0)
    gameobject_scale(plane, 50)
    physics_init_box(scene, plane, 0, 50, 0.1, 50)
end
