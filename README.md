# Globular Physics Simulator
The globular physics simulator is my submission for the final project of Dr. Wyvill's CSC 578B. Its purpose is to simulate the behaviour of viscous fluid spouting from a fountain. The system uses an implicit mesher developed at the University of Toronto and can be found [here](http://www.dgp.toronto.edu/~rms/software/ImplicitMesher/index.html)

A full speed demo can be seen [here](https://youtu.be/NTm2uciIXeA) and a slow motion version [here](https://youtu.be/8yMykBon_8I)
## Design
The simulator begins by reading and loading a mesh of a water fountain from an .Obj file. The fountain data is stored in a Mesh object defined in the class of the same name.
This object and a horizontal plane are handled by the "Structure" class. The simulator then creates a vector of point particles which are managed by the "ParticleSet" class.
The "ParticleSet" object maintains position, velocity and forces acting on each of the particles. At every step of the simulation interglobular forces are computed according to the
according to "Globular Dynamics: A Connected Particle System for Animating Viscous Fluids" by Gavin Miller and Andrew Pearce which can be found [here](http://www.sciencedirect.com/science/article/pii/0097849389900782).

The implicit mesher system computes and draws a spherical glob around each particle in the simulation. All of the seperate meshes neeed to draw each glob are treated
as a single mesh. If two globules come within a specified distance of one another the system computes and draws an amalgamated mesh to simulate the joining of two viscous fluid droplets.

## Controls
W - Move Forward  
S - Move Backward  
A - Move Left  
D - Move Right  
R - Move Up  
F - Move Down  
Q - Move Tilt Left  
E - Move Tile Right  
Enter - Toggle Globular Simulation On/Off  
Spacebar - Step Globular Simulation  
CapsLock - Toggle Camera follow spline path  
Backspace - Reset Particles  
Mouse - Look At