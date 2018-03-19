all:
	cd shaders; glslangValidator -V shader.vert; glslangValidator -V shader.frag
	g++ main.cpp -lvulkan -lglfw -lfreeimage
