# Gabriel Engine
Gabriel is a 2d game engine designed for pixel art games targeting desktop (and in the future, web). It is currently not feature complete.
## Structure
### Nodes, Scenes, and the Tree
Gabriel is based on Godot, and uses a nodes and scenes to build games. Each node can have an attached script which modifies the behavior of the node. Nodes can be a parent to multiple children, which they can interact with via the script. Scenes are defined as JSON objects, with a type, name, transform, script properties, and children. A main.json scene will be instanced when the game starts and further scenes can be instanced from there. The collection of nodes which are decended from the root are considered part of the tree, and will be processed each frame.
### Implementation
The engine is implemented in Go and C using OpenGL for rendering, GLFW for windowing, and Glad for function loading. This should allow for Gabriel to be compiled across platforms. WebGPU should replace OpenGL in the near future, or serve as an alternative rendering library (this may result in the unfortunate need to add JavaScript as language for the game).
## Upcoming!
- [ ] WebGPU Rendering & Web Builds
- [ ] Post processing shaders
- [ ] Audio
- [ ] Physics (Mainly just collision detection for rectangles, circles, and rays)
- [ ] \(Potentially) custom scene format
- [ ] \(Potentially) custom editor
