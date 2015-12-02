#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "grid.hpp"
#include <vector>
struct Cube
{
  GLuint vbo;
  GLuint vao;
  int x;
  int y;
  int z;
  int preset;
  float r;
  float g;
  float b;
};

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

private:
	void initGrid();
	
	//Added functionality
	void initCursor();							//Sets up the vbo and vao of the cursor
	void transformCube(float* verts, float x, float y, float z, int size);	//Transform the cube to a new translation
	void transformCursor(int z);						//Transform the cursor
	void createCube(int x, int y, int z, int presetColour);			//Create a new cube by setting up it's vbo and vao
	void removeCube(int x, int y, int z);					//Delete the cube
	void copyCubes(int offsetX, int offsetY, int offsetZ);			//Copy properties of the cube and translate it
	void initPresetColours();						//Initializes the preset colours
	
	std::vector<struct Cube> m_cubes;		//Container to keep track of all the info of the cubes created
	std::vector<glm::vec3> m_presetColours;		//Container to keep track of the 8 preset colours
	const int numPresetColors=8;			//Number of preset colours			
	float m_posX;					//Used to rotate the camera
	float m_lastPosX;				//Used to remember where to rotate from
	float m_posY;					//How far off the ground is the camera
	const float m_distance = 32.0f;			//How far is the camera from the centre
	
	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object
	
	// Fields related to cube gemoetry.
	GLuint m_cube_vbo; // Vertex Buffer object for cube?
	GLuint m_cube_vao; // Vertex Array Object
	GLuint vbo_cubeTransforms;
	
	GLuint m_cursor_vbo;	//vertex array buffer for the cursor
	GLuint m_cursor_vao;	//vertex array object for the cursor
	
	GLuint m_numInstances;	//keeps track of the number of cubes
	GLuint m_currentX;	//keeps track of what the cursor's x position is
	GLuint m_currentY;	//keeps track of what the cursor's y position is

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;
	
	float colour[8][3];
	int current_col;
	float current_proj;
};
