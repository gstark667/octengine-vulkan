all:
	cd shaders; glslangValidator -V shader.vert; glslangValidator -V shader.frag
	g++ -g main.cpp -lvulkan -lglfw -lfreeimage -lassimp
