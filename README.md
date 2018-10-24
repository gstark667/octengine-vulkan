# octengine-vulkan
This is a work in progress vulkan game engine that I've been working on, the main directory is a bit messy since I'm keeping some testing scripts in there, but those will eventually be moved to their own repository for basic game object components and examples once the engine features become more stable.

### Example Game
- https://github.com/gstark31897/racing-game

### Project Structure
- src: contains most of the engine code
- include: headers for the engine code
- lualibs: contains code for building shared objects that lua can import and use for integrating with the engine
- scene_0.lua: a script that tells the engine how to sctructure the scene (just spawns some objects, sets some models and attaches scripts)

### Features
Features that are currently implemented
- loading many model formats through assimp
- basic diffuse shading
- basic shadow mapping
- deferred rendering model
- GPU model deformation using bone data from assimp (currently disabled for ease of changes)
- bullet physics integration
- lua scripting for scene construction, input handling and gameobject manipulation
Features that I'm planning on working on in the near future
- multiple light sources (for both shading and shadows)
- physically based rendering (using textures to parameterize metalicity, roughness, ior, and illumination)
- event driven scripting
- lua driven UI scripting
- event driven animation state machines
- SSAO
- MSAA
- misc lua scripting integration that's missing

### Building
You can build the engine quite easily (refer to the libraries section to get a full list of libraries you might need) ((Note that building on windows may or may not work, I don't have a windows machine to test on, but I have tested it using mingw and have good word from a friend that the resulting build ran))
- git clone https://github.com/gstark31897/octengine-vulkan.git
- cd octengine-vulkan
- mkdir build
- cd build
- cmake ..
- make
- cp ../*.png ./; cp ../*.dae ./; cp ../*.lua ./
- ./octengine

### Libraries
Libraries that I used for this project
- vulkan (graphics): https://www.khronos.org/vulkan/
- lunarg (vulkan sdk): https://www.lunarg.com/
- assimp (model importing): https://github.com/assimp/assimp
- SDL2 (windowing/user input): https://www.libsdl.org/
- freeimage (loading textures from file): http://freeimage.sourceforge.net/
- lua (scripting): https://www.lua.org/
- bullet (physics): https://pybullet.org/
