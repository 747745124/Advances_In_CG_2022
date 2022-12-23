# Advanced Topics in Computer Graphics
## Build Instructions:

#### For Mac Users (Recommend to build with VSCode)

1. Download CMake 

> https://cmake.org/download/

2. Execute below shell script in terminal

```shell
mkdir build
cd build
cmake ..
```



#### For Windows Users

> Header files and dependencies are compatible with Mac OS, however, these have not been tested with Windows platform. A prebuilt dependency for Windows is provided compatible with VS 2022. If any build error occurs, make sure you download the correct version of pre-compiled library for GLFW and assimp. Modify CMakeLists.txt when necessary.

1. Download CMake
2. Download GLFW pre-compiled library for Windows
   1. Extract the `glfw3.lib` file to the `/lib` folder
   2. Replace the files in `/include/GLFW` with corresponding extracted files

>  https://www.glfw.org/download

3. Download assimp library 
   1. Extract the pre-compiled `.lib` file to the `/lib` folder
   2. Replace the files in `/include/assimp` with corresponding extracted files
4. Execute below shell script in terminal

```shell
mkdir build
cd build
cmake ..
```


## Style Guidance:
* Function Name: Either Lower Camel Case or using '_'
* Class Name: Upper Camel Case
* Avoid `using namespace std` in global scope;(Instead use "using std::vector;" in your own function!)
* Using standard libraries(e.g. smart pointers) properly for safety.(RAII design is better)
* Avoid nested `if-else`, using if return instead.(nested if-elses code may be substitude by switch-cases code, which runs much faster)
* Using `nullptr` instead of `NULL`
* Using `""` to include self-defined headers and libraries in directory `/lib` , `< >` for system libraries.
* Using `const` as often as possible

## Codes need to be refactored:
* Render.h
* Entity.h
* GameController.h
* GameObject.h

## Features implemented:
* Deferred shading
* Real Time Local Reflections (Stochastic Screen-Space Reflections)
* Cascaded Shadows Maps && Softshadow approximation
* SSAO && SSDO
* Caustics
* Post Processings (Tone Mapping & DoF)

# References
* https://learnopengl.com/Advanced-Lighting/Deferred-Shading
* https://ogldev.org/www/tutorial35/tutorial35.html
* https://github.com/lettier/3d-game-shaders-for-beginners
* http://www.klayge.org/material/4_1/SSR/S2011_SecretsCryENGINE3Tech_0.pdf
* https://github.com/GPUOpen-Effects/FidelityFX-SSSR
