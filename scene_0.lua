require("scene")
require("gameobject")
require("physics")
require("light")
require("camera")

function setup(scene, object)
    player = scene_add_gameobject(scene)
    scene_set_model(scene, player, "capsule.dae")
    scene_add_script(scene, player, "player.lua")
    physics_set_position(player, 0, 10, 5)

    --for x = 0, 10 do
    --    for y = 0, 10 do
    --      for i = 1, 2 do
    --        cube = scene_add_gameobject(scene)
    --        scene_set_model(scene, cube, "scifi_cube.dae")
    --        gameobject_scale(cube, 1)
    --        gameobject_transform(cube, x*1, i, y*1)
    --        physics_init_box(scene, cube, 10, 0.5, 0.5, 0.5)
    --      end
    --    end
    --end

    plane = scene_add_gameobject(scene)
    scene_set_model(scene, plane, "plane.dae")
    gameobject_transform(plane, 0, 0, 0)
    gameobject_scale(plane, 50)
    physics_init_box(scene, plane, 0, 50, 0.01, 50)

    example = scene_add_gameobject(scene)
    scene_set_model(scene, example, "example.dae")
    gameobject_rotate(example, math.pi, 0, 0)
    gameobject_scale(example, 0.5)

    sun = scene_add_light(scene)
    sun_cam = light_get_camera(sun)
    camera_set_fov(sun, -1.0)
    sun_obj = camera_get_gameobject(sun_cam)
    gameobject_rotate(sun_obj, -math.pi/4.0, math.pi/4.0, 0.0)
    gameobject_transform(sun_obj, 20, 20, 20)
end
