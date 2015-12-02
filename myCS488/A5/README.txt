Compilation:

This program is compiled by typing: make
This program was compiled on the machine: gl11.student.cs

Manual:
Hierarchical transformations transformations are not working correctly.
A unique scene was not added/created.

I have added Blinn-Phong illumination model as my extra objective.

Code Credit Request:
1. The code that didn't work is the Mesh::checkIntersection, on line 118-151.

2. It is not getting the proper colour value if the mesh is a child of Geometry node.

3. I believe that the probable cause is how the ray and the origin is being adjusted 
   at each each iteration within GeometryNode::checkIntersection and SceneNode::checkIntersection.

4. I have tried to follow the notes closely by transforming the ray and eye by the invtrans on 
   its way down and computing the new normal by taking trans and transverse of the upper 3x3 matrix of invtrans