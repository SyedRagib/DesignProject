Compilation:

This program is compiled by typing: make
This program was compiled on the machine: gl11.student.cs

Manual:

My objectives were:
1. Support cylinder and cones. (done)
2. Support torus. (done)
3. Concurrency. (done)
4. Procedural texture mapping using perlin noise. (done)
5. Bump mapping using perlin noise. (done)
6. Anti aliasing (done)
7. Constructive Solid Geometry (not done)
8. Reflection (done)
9. Refraction (code request)
10. Unique scene (not done)

Did not include a unique scene, CSG or refraction.
I have edited the scene_lua.cpp to add and edit commands.

	Added Commands:
		-gr.torus: will add a new torus primitive to the scene. It takes the name of the primitive, the center of the torus, the direction which the torus is facing, the radius from the centre to the center of the ring, the radius of the ring. 
		An example is: gr.torus('name', {0,0,0}, {0,1,0}, 2, 1)

		-gr.cylinder: will add a new cylinder primitive to the scene. It takes the name of the primitive, the bottom center point of the cylinder, the direction which the cylinder is facing (top center - bottom center), the height of the cylinder, and the radius of the cylinder. 
		An example is: gr.cone('name', {0,-1,0}, {0, 1, 0}, 60, 10)

		-gr.cone: will add a new cone primitive to the scene. It takes the name of the primitive, the bottom center point of the cone, the direction which the cone is pointing towards, the opening of the cone (given by degrees), and height.
		An example is: gr.cone('name', {0,-1,0}, {0, 1, 0}, 60, 10)

		-gr.perlin_material: will create a new material which will use the perlin function to help generate textures. The command will take the following, diffuse colour 1, diffuse colour 2, specular, shininess, scale (used to alter the texture),  reflection {0 to 1}, refraction index of the material (e.g 1.0 is a vacum), texture type (marble=0, wood=1, checker=2) bumping scale, bump amout.

			The scale parameter will affect different textures in differeny ways. For example in marbles it can affect the stripes of the marble. For wood it will determine how many rings are created. For checker it will determine the size of the checker boards.

			The bumping scale parameter will change how many bumps in a given area. Higher the bumping scale, more bumps in a given area.

			The bumping amount parameter will change how much the normal is being bumped.

		An example of this command is: 
			wood = gr.perlin_material({1.0, 0.5, 0.0}, {0.5976,0.2968,0.0}, {0.5, 0.7, 0.5}, 50, 6, 0.0, 0.0, 1, 4.64, 1.0)

	Edited Commands:
		-gr.material: this command takes in additional parameters: how much reflection, refraction enabled, refraction index of the material, bumping scale, bump amount

	Code Credit Request:
	1. The code that didn't work is the a4.cpp, on line 178-185.
	2. The refraction colour is not being properly set.
	3. The probable cause is that my intersection is not returning the correct t value.
	4. I have tried the following:
		-ran the ray tracer on a scene containing only a sphere. A ray was casted at the center pixel only and was the index of refraction was set to 1.0 (same as air/vacuum). Made sure that the refracted ray and the primary ray had the same values.
		-ran the ray tracer on a scene with 2 spheres that are behind one another. Casted a single ray which I knew would hit both spheres. Made sure that the order it hit the spheres were correct: hit the front sphere first as the ray entered, hit the front sphere second as it exited, hit the back sphere third as the ray entered and finally hit the back sphere as the ray exited.
		-Calcualted the fresnel coefficient by hand to make sure they were proper values.