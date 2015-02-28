TARGETS=broadside

CC=g++
#CC=clang++
#OPTIMISE=-O2
OPTIMISE=-O0
DEBUG=-ggdb3
CFLAGS= -fno-strict-aliasing -DUNIV_LINUX \
	-Werror -Wall -Wextra $(OPTIMISE) $(DEBUG) \
	-I "/usr/local/include"
CFLAGSNOWARN= -fno-strict-aliasing -DUNIV_LINUX \
	$(OPTIMISE) $(DEBUG) \
	-I "/usr/local/include"
LIBS=-Wl,-Bsymbolic-functions \
	-rdynamic -lrt -lpthread \
	-lGL -lSDL2main -lSDL2 \
	-L "/usr/local/lib"

RAW_COMMONOBJECTS=CApp.o render.o \
	SimpleMesh.o util.o assetloader.o stbi_image.o \
	BadMesh.o geom.o FontRender.o

RAW_TARGETOBJECTS=$(TARGETS:%=%.o)
COMMONOBJECTS=$(RAW_COMMONOBJECTS:%=build/%)
TARGETOBJECTS=$(RAW_TARGETOBJECTS:%=build/%)
ALLOBJECTS=$(COMMONOBJECTS) $(TARGETOBJECTS)

recurse:
	@make -j$(shell getconf _NPROCESSORS_ONLN) all

all: $(TARGETS)

build:
	mkdir -p build


-include $(ALLOBJECTS:.o=.d)

clean:
	rm -f $(TARGETS)
	rm -f build/*

build/stbi_image.o: stbi_image.cpp | build
	$(CC) $(CFLAGSNOWARN) -c stbi_image.cpp -o build/stbi_image.o
	$(CC) $(CFLAGSNOWARN) -MM stbi_image.cpp > build/stbi_image.d
	sed -i '1s/^/build\//' build/stbi_image.d

build/%.o: %.cpp | build
	$(CC) $(CFLAGS) -c $*.cpp -o build/$*.o
	$(CC) $(CFLAGS) -MM $*.cpp > build/$*.d
	sed -i '1s/^/build\//' build/$*.d

# testing
broadside: build/broadside.o $(COMMONOBJECTS) $(PROCEDURALOBJECTS) $(CORE) $(LUA) $(IMGUI)
	$(CC) build/broadside.o $(COMMONOBJECTS) $(PROCEDURALOBJECTS) $(CORE) $(LUA) $(IMGUI) -o broadside $(LIBS)

compilers: basepackages

basepackages:
	sudo apt-get update -y
	sudo apt-get upgrade -y
	sudo apt-get -y install g++ libglfw-dev xorg-dev libglu1-mesa-dev

run:
	./broadside
