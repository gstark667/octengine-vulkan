# octengine-vulkan
This code can currently render a model, apply texturing, shading and skeletal animation to it.
These are my next steps to make it usable as a game engine.
1. make lighting be controled by the ubo instead of being hard coded in the shader
2. implement some basic physically based rendering (ie dielectrics and metallics)
3. implement more advanced pbr with subsurface scattering
4. get instanced rendering working
5. make automated pipeline creation so we just need to call a single function to load a model/texture/shader
6. add a scripting engine to the engine (probably LUA) and expose some transform, glfw input and time data to it
7. add an audio engine (probably OpenAL) and expose the functions to the LUA environment
8. make a physics engine and expose its functions to LUA
9. make a graphics toolkit for in game menus
10. make a game
