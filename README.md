# Advanced Topics in Computer Graphics
## Build Instructions:
* Download and build glfw and glad suitable to your own platform 
* Download and build assimp on your own platform
* Download imgui src
* Edit cmakelist for your own platform if needed
1. Add /GLFW /glad /KHR /glm /imgui /assimp to /include directory
2. Add runtime and compile-time libs(For glfw and assimp) to /lib directory
3. `mkdir build&&cd build`
4. `cmake ..`
5. Use your own ide to build project

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

## Features to be implemented:
* Deferred shading
* Real Time Local Reflections (Stochastic Screen-Space Reflections)
* Cascaded Shadows Maps && Softshadow approximation
* SSAO && SSDO

# References
* https://learnopengl.com/Advanced-Lighting/Deferred-Shading
* https://ogldev.org/www/tutorial35/tutorial35.html
* https://github.com/lettier/3d-game-shaders-for-beginners
* http://www.klayge.org/material/4_1/SSR/S2011_SecretsCryENGINE3Tech_0.pdf
* https://github.com/GPUOpen-Effects/FidelityFX-SSSR
