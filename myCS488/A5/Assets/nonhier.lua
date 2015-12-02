-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 1.0, 0, 0, 0, 0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 1.0, 0, 0, 0, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 1.0, 0, 0, 0, 0)
mat4 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 1.0, 0, 1.5, 0, 0)
mat5 = gr.perlin_material({0.0, 1.0, 0.0}, { 1.0, 0.0, 0.0}, {0.5, 0.7, 0.5}, 50, 0.5, 0.0, 0.0, 0, 16, 0.25)
wood = gr.perlin_material({1.0, 0.5, 0.0}, {0.5976,0.2968,0.0}, {0.5, 0.7, 0.5}, 50, 0.01, 0.0, 0.0, 1, 4.64, 1.0)
checker = gr.perlin_material({0.0, 0.0, 0.0}, {1.0,1.0,1.0}, {0.5, 0.7, 0.5}, 50, 0.01, 0.0, 0.0, 2, 0, 0)


scene_root = gr.node('root')

--s1 = gr.torus('s1', {0, 0, 0}, {1.0, 25.0, 1.0}, 2, 1)
--s1 = gr.cylinder('s1', {0, 0, 0}, {0, 2, 0}, {0, -2, 0}, 2)
s1 = gr.nh_sphere('s1', {0, 0, -300}, 100)
--s1 = gr.cone('s1', {0,0,0}, {0,0,5}, 5)
--s1:translate(5,0, 0)
--s1:rotate('X', 90)
--s1:rotate('Y', 90)
--s1:scale(0.9,0.9,0.9)
scene_root:add_child(s1)
s1:set_material(mat4)

--s6 = gr.nh_sphere('s6', {0,0,0}, 2, 1)
--s6:translate(1, 0, -5)
--s6:set_material(mat3)
--scene_root:add_child(s6)

--s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
--scene_root:add_child(s2)
--s2:set_material(mat1)

--s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
--scene_root:add_child(s3)
--s3:set_material(mat5)

--b1 = gr.nh_box('b1', {-200, -125, 0}, 100)
--scene_root:add_child(b1)
--b1:set_material(wood)

--s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
--scene_root:add_child(s4)
--s4:set_material(mat3)

s5 = gr.nh_sphere('s5', {0, 100, -700}, 100)
scene_root:add_child(s5)
s5:set_material(mat3)


-- A small stellated dodecahedron.

--steldodec = gr.mesh( 'dodec', 'smstdodeca.obj' )
--steldodec:set_material(wood)
--scene_root:add_child(steldodec)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
--orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'nonhier.png', 1024, 1024,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
