# Graphics Engine Project

## Purpose
This project was built with the intent of exapnding my knowledge of graphics engine's and production code I might see in industry! While certainly not a perfect project, what I hope to learn and implement in this project are these key ideas:

## Goals
### Directory Design
For general production purposes. Code that can run on any machine is an important idea to maintain as a developer! Its important to take this into account when working in a cooperative environment and not just by yourself.
### Static Libraries
For general production purposes. static libraries offer developers a pre-compiled library that hides any proprietary code and, very importantly, SAVES DEVS FROM THEMSELVES! A library may be rearranged at any time for optimization purposes or general architecture updates, but the behavior should still be the same. As a user of the library, you don't want to use aspects of the library that may change in any given update, or that are not guaranteed to work.
### RAII
To encourage myself to think about how I handle my programs resources! RAII (Resource Acquisition Is Initialization) is an important idiom to help prevent memory leaks and unnesecary resource creation. I wish to be able to manage my resources wisely (not just through a singleton ResourceManager).

## Directories
As a part of trying to design a directory that is usable across systems, I also want to ensure that any game which exists in this main directory can use the graphics engine.

### Engine (PRODUCTION)
The engine itself. This is the most important aspect of the entire project. Here, the complete implementation of the engine can be found with its .h/.cpp files, along with some default shaders that any game within the directory can use.

Future work to be done:
- Implement more advanced rendering techniques (double/triple buffering, cube maps, reflections, etc.)
- Move context to the by the general namespace rather then the renderer '.h' file.
- Basic quality of life changes to make the API even easier to use.
- Switching the ECS structs from an 'Array of Structs' design to a 'Struct of Arrays' design.
