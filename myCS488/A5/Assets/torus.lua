-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0, 0, 8, 0, 0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0.50, 0, 8, 0, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.70, 0, 8, 0, 0)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.50, 0, 16, 0, 0)
mat5 = gr.perlin_material({0.0, 1.0, 0.0}, { 1.0, 0.0, 0.0}, {0.5, 0.7, 0.5}, 50, 0, 0.0, 0.0, 0, 16, 0.25)
wood = gr.perlin_material({1.0, 0.5, 0.0}, {0.5976,0.2968,0.0}, {0.5, 0.7, 0.5}, 50, 0.01, 0.0, 0.0, 1, 4.64, 1.0)
checker = gr.perlin_material({0.0, 0.0, 0.0}, {1.0,1.0,1.0}, {0.5, 0.7, 0.5}, 50, 0.01, 0.0, 0.0, 2, 0, 0)


scene_root = gr.node('root')

s1 = gr.torus('s1', {0, 0, 0}, {0, 1, 0}, 10, 5)
s1:rotate("X", 45)
scene_root:add_child(s1)
s1:set_material(wood)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'nonhier.png', 256, 256,
	  {0, 0, 60}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})