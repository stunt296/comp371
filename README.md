# FINAL PROJECT COMP 371: Walk Through a Procedurally Modeled World

Erin Rainville, Alexander Rainbow, Amin Kadawala, Henry Li
#
![image](https://github.com/user-attachments/assets/1a788e74-01d8-4f41-880c-47c8eb18b89a)
![image](https://github.com/user-attachments/assets/660ce6b1-ede7-41bd-bdfd-3e947e6c0ae6)

The goal of this project is to create a graphics program using OpenGL for
walking through a procedurally created virtual world, in this case a forest inspired by Mincraft's cube-based world. All repeating
items such as the trees and pools of water are procedurally created into random shapes and procedurally placed in the world. The terrain is procedurally
created using Perlin noise to have varied elevations that smoothly transition between hills and dips. There is a camera that can
be used to travel through the world and view it from different angles. During navigation,
there is collision detection to prevent the user from passing through objects. 

# Implementation
* The terrain is generated by going through a 2D float array (a heightmap) that holds the <x,z> positions of the map and the y value defines the height of the cubes.
* To reduce processing power, it calls glDrawArrays only for the faces that are visible, as such it renders all the faces of the cubes that are above the height of the neighboring cubes.
* The number of pools generated, the size of each pool, and the center position of each pool are all procedurally generated using a loop that checks pool vertices (starting at the center vertex) and decides if the adjacent vertices should also be water.
* The water is generated by adding a blue, translucent top face above every top face that is below the designated water level.
* The trees are generated by going over the heightmap, checking if the <x,y,z> is above water level, if yes then it calculates a 1% chance of placing a tree in that area, and if it is true then it checks to make sure the tree placement would not collide with another tree’s placement. If this is all true, it adds the position to a position vector. In the main while loop, it draws the trees based on the position vector.
* Navigation was set up with the wasd keys for easy camera control/movement.
* The general background was implemented as a textured skybox with an initial blue color and a global light to reflect sunlight.
* The different texture is illustrated by changing the color of the cement texture from lab 04.




