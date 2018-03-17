all:
	g++ main.cpp -lvulkan -lglfw
	cd shaders; glslangValidator -V shader.vert; glslangValidator -V shader.frag
