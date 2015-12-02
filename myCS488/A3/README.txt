Compilation:
This program is compiled by typing in make
This program was compiled on gl11.student.cs


Manual:
Picking wasn't implemented, instead a list of checkboxes can be used to choose joints
Arcball rotation wasn't implemented
When joint is picked it takes away the shininess of the geometry node 
Added an extra puppet called spongebob.lua

In the assignment it had asked us to rotate the head if it's selected, since I don't have picking working it rotates the head if the joint connected to the head is clicked, I could of added a checkbox which represents the head but that would complicate interaction if the head is a child of a joint
