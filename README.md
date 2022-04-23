# Advanced Topics in Computer Graphics
## Style Guidance:
* Function Name: Either Lower Camel Case or using '_'
* Class Name: Upper Camel Case
* avoid `using namespace std` in global scope;(Instead use "using std::vector;" in your own function!)
* Using standard libraries(e.g. smart pointers) properly for safety.(RAII design is better)
* Avoid nested `if-else`, using if return instead.(nested if-else maybe substitude by switch, which runs much faster)
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
