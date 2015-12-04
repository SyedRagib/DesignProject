-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0, 0, 8, 0, 0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0.50, 0, 8, 0, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.70, 0, 8, 0, 0)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.50, 0, 16, 0, 0)
mat5 = gr.perlin_material({0.0, 1.0, 0.0}, { 1.0, 0.0, 0.0}, {0.5, 0.7, 0.5}, 50, 0.5, 0.0, 0.0, 0, 16, 0.25)
wood = gr.perlin_material({1.0, 0.5, 0.0}, {0.5976,0.2968,0.0}, {0.5, 0.7, 0.5}, 50, 0.01, 0.0, 0.0, 1, 0, 0)
orange = gr.perlin_material({1.0, 0.6274, 0.0}, {1.0, 0.6274, 0.0}, {0.5, 0.7, 0.5}, 50, 0.01, 0.0, 0.0, 1, 4.64, 0.5)
checker = gr.perlin_material({0.0, 0.0, 0.0}, {1.0,1.0,1.0}, {0.5, 0.7, 0.5}, 50, 0.01, 0.0, 0.0, 2, 0, 0)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {250, 0, 0}, 100)
--s1:rotate("X", 20)
scene_root:add_child(s1)
s1:set_material(mat5)


s2 = gr.nh_sphere('s2', {-250, 0, 0}, 100)
--s2:rotate("X", 20)
scene_root:add_child(s2)
s2:set_material(wood)

s3 = gr.nh_sphere('s3', {0,0,0},100)
scene_root:add_child(s3)
s3:set_material(orange)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'perlin.png', 256, 256,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})