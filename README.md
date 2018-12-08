# QuadtreeTerrain
Quadtree Terrain Rendering System with Simple Features: C++, OpenGL, SDL

YouTube Video: https://youtu.be/CqmJ9Szk7-o

### Topic: Object-Oriented C++ integration with SDL and OpenGL for Agent-Based Modelling (ABM)
### Author: Eugene Ch'ng [Academic Blog](http://www.complexity.io/)
### Purpose: Prepared for the ERC Advanced Research Grant 'Lost Frontiers' Scalable Agent-Based Modelling Team Training.
### Audience: For students and researchers needing a quick approach to learning agent-based modelling foundation for C++ and OpenGL

## INTRODUCTION
The code presented here is now a 'systems-level' development, as a continuation of the series of C++ code in my repositories for the ERC 'Lost Frontiers' Advanced Research Grant. The Quadtree Terrain System is composed of a set of decoupled classes, each for managing objects within the ABM system:

- main.cpp - main code tying everything together
- QTTerrain.h/cpp - a terrain rendering system
- TerrainQuadTree.h/cpp - a quadtree datastructure used for managing the procedural terrain
- Camera.h/cpp - a simple camera for moving around the virtual space
- Grid.h/cpp - a simple grid used for orientation
- MoveableOnQTTerrain.h/cpp - an agent used for skating on the surface of the quadtree terrain

## Quadtree Terrain Features
- Procedura terrain rendering based on a viewrange of the camera, and level of detailing based on distance
- Terrain functions for calculating surface normals, raycast intersects of planes, etc.
- A quadtree datastructure built for generating boundaries, quadtree layering, vertice indices, etc.

## [INSTALLATION](https://github.com/drecuk/ABM-Basics-Installation)
## [C++ SDL FOUNDATION](https://github.com/drecuk/ABM-Basics-SDL)
## [C++ OPENGL BASICS](https://github.com/drecuk/ABM-Basics-OpenGL)
## [C++ OBJECT ORIENTED PROGRAMMING BASIC INTEGRATING SDL and OPENGL](https://github.com/drecuk/ABM-Basics-OOP)

## SYSTEMS INTEGRATION AND SCALABILITY
- [Quadtree Terrain](https://github.com/drecuk/QuadtreeTerrain)
- [Quadtree Agent-Interaction](https://github.com/drecuk/QuadtreeAgentInteraction)
- GPU Accelerated Interaction Processing with CUDA (to be developed)

## ACKWNOWLEDGEMENTS
“This project has received funding from the European Research Council (ERC) under the European Union’s Horizon 2020 research and innovation programme ( ERC funded project No. 670518 LOST FRONTIERS)”
