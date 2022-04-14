# OpenGL Installation Process

#### Some Pointers
* Make sure everything you configure is set for **_x64_** in your IDE.
* *$(SolutionDir)* is the directory containing the *.sln* file for the Visual Studio Solution
* *$(ProjectDir)* is the directory containing the *.vcxproj* file for the Visual Studio Project

## Initial
1. Create project in IDE (These directions are for Visual Studio 2019)
2. Create directory named 'Linking' in *$(SolutionDir)
3. Set Project Properties
    * Linker -> Input
        * Additional Dependencies
            \+ opengl32.lib
    * VC++ Directories
        * Library Directories
            \+ $(SolutionDir)\Linking\lib;
        * Include Directories
            \+ $(SolutionDir)\Linking\include;
4. Create directories *src*, *lib*, and *assets* in *$(ProjectDir)*

## GLFW
1. Download pre-compiled binaries from [GLFW](https://www.glfw.org/download.html)
2. Unzip download and add files to linking directory
    * *GLFW\include* -> *$(SolutionDir)\Linking\include*
    * all *.lib* files from corresponding VS folder -> *Linking\lib\GLFW*
3. Add files to project directory
    * *glfw3.dll* from corresponding VS folder -> *$(ProjectDir)*
4. Set Project Properties
    * Linker -> Input
        * Additional Dependencies
            \+ GLFW\glfw3.lib

## GLAD
1. Download package (*.zip* file) from [their site](https://glad.dav1d.de/)
    * Only change these default settings:
        * API -> gl: Version 3.3
        * Options -> Generate a Loader: checked **(required)**
2. Unzip download and add files to project
	* *glad\include* -> *$(SolutionDir)\Linking\include*
		* this should add two folders (*glad*, *KHR*) to your include directory
	* *glad\src\glad.c* -> *$(ProjectDir)\lib*
3. Include the glad.c source file with the project in Visual Studio

## GLM
1. Download the latest package from [GLM](https://github.com/g-truc/glm/tags)
    * Click on the package and download the .zip file named *glm-x.x.x.x.zip*
2. Unzip download and add files to linking directory
	* Drag the *glm\glm* directory (containing many *.hpp* files) to *$(SolutionDir)\Linking\include*

## OpenGLTutorial Files
1. Create the directories *io*, *rendering* in *$(ProjectDir)*
2. Go to the OpenGLTutorial repository on [GitHub](https://github.com/michaelg29/OpenGLTutorial)
3. Download the following files and place them in the *io* directory
    - OpenGLTutorial/src/io/camera.h, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/io/camera.h)
    - OpenGLTutorial/src/io/camera.cpp, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/io/camera.cpp)
    - OpenGLTutorial/src/io/keyboard.h, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/io/keyboard.h)
    - OpenGLTutorial/src/io/keyboard.cpp, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/io/keyboard.cpp)
    - OpenGLTutorial/src/io/mouse.h, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/io/mouse.h)
    - OpenGLTutorial/src/io/mouse.cpp, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/io/mouse.cpp)
4. Download the following files and place them in the *rendering* directory
    - OpenGLTutorial/src/graphics/memory/uniformmemory.hpp, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/graphics/memory/uniformmemory.hpp)
    - OpenGLTutorial/src/graphics/memory/vertexmemory.hpp, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/graphics/memory/vertexmemory.hpp)
    - OpenGLTutorial/src/graphics/rendering/shader.h, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/graphics/rendering/shader.h)
    - OpenGLTutorial/src/graphics/rendering/shader.cpp, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/graphics/rendering/shader.cpp)
    - OpenGLTutorial/src/graphics/rendering/material.h, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/graphics/rendering/material.h)
    - OpenGLTutorial/src/graphics/rendering/material.cpp, [Source](https://raw.githubusercontent.com/michaelg29/OpenGLTutorial/master/OpenGLTutorial/src/graphics/rendering/material.cpp)
5. Make sure to include all files in the project in Visual Studio