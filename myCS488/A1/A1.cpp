#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

const float PI = 3.14159265f;
static const size_t DIM = 16;
int zCubeCounter[DIM][DIM];

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 )
{
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
  
	current_proj = 45.0f;
	m_numInstances = 0;
	m_currentX = 0;
	m_currentY = 0;
	m_cubes.clear();
	m_presetColours.clear();
	
	//Set up the colours
	initPresetColours();
	
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );
	
	//Clear the zCubeCounter
	for(int i=0; i < DIM; i++)
	  for(int j=0; j < DIM; j++)
	    zCubeCounter[i][j] = 0;
	
	//Add dummy vec3
	for(int i=0; i < 8; i++)
	  m_presetColours.push_back(glm::vec3(0));
	
	//Initialize the grid and cursor
	initGrid();
	initCursor();
	
	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	float y = float(DIM)*2.0*M_SQRT1_2;
	m_lastPosX = 180.0f;
	m_posX = 180.0f;
	float camX = m_distance * -sinf(180.0f*(PI/180)) * cosf((y)*(PI/180));
	float camZ = -m_distance * cosf((180.0f)*(PI/180)) * cosf((y)*(PI/180));
	view = glm::lookAt( 
		glm::vec3( camX, y, camZ ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );
	proj = glm::perspective( 
		glm::radians( current_proj ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initCursor()
{
	//This array holds the vertex coordinates for displaying the cursor
	float verts[] = {
		    0.0f,0.0f,0.0f, //A
		    1.0f,0.0f,0.0f,
		    0.0f,0.0f,1.0f,
		    1.0f,0.0f,1.0f, //B
		    0.0f,0.0f,1.0f,
		    1.0f,0.0f,0.0f};
	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_cursor_vao );
	glBindVertexArray( m_cursor_vao );

	// Create the cursor vertex buffer
	glGenBuffers( 1, &m_cursor_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_cursor_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(verts),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );	    
}

void A1::initPresetColours()
{
  colour[0][0] = 1.0f;
  colour[0][1] = 1.0f;
  colour[0][2] = 1.0f;
  
  colour[1][0] = 1.0f;
  colour[1][1] = 0.0f;
  colour[1][2] = 0.0f;
  
  colour[2][0] = 0.0f;
  colour[2][1] = 1.0f;
  colour[2][2] = 0.0f;
  
  colour[3][0] = 0.0f;
  colour[3][1] = 0.0f;
  colour[3][2] = 1.0f;
  
  colour[4][0] = 1.0f;
  colour[4][1] = 1.0f;
  colour[4][2] = 0.0f;
  
  colour[5][0] = 0.0f;
  colour[5][1] = 1.0f;
  colour[5][2] = 1.0f;
  
  colour[6][0] = 1.0f;
  colour[6][1] = 0.0f;
  colour[6][2] = 1.0f;
  
  colour[7][0] = 0.0f;
  colour[7][1] = 0.0f;
  colour[7][2] = 0.0f;
}

//This method is called to translate the cube by offseting it's vertex array
void A1::transformCube(float* verts, float x, float y, float z, int size)
{
  for(int index = 0; index < size; index++)
  {
    if(index % 3 == 0)
      verts[index]+=x;
    if(index % 3 == 1)
      verts[index]+=y;
    if(index % 3 == 2)
      verts[index]+=z;
  }
}

//Removes the cube located at the translation x,y,z
void A1::removeCube(int x, int y, int z)
{
  for(int i=0; i < m_cubes.size(); i++)
  {
    struct Cube current_cube = (struct Cube) m_cubes.at(i);
    if(current_cube.x == x && current_cube.y == y && current_cube.z == z)
      m_cubes.erase(m_cubes.begin()+i);
  }
}

//Generate another cube with the properties of the current cube
void A1::copyCubes(int offsetX, int offsetY, int offsetZ)
{
  //Delete all the cubes in this location
  for(int j=0; j < zCubeCounter[m_currentX+offsetX][m_currentY+offsetY]; j++)
  {
    removeCube(m_currentX+offsetX,m_currentY+offsetY,j);
    m_numInstances--;
  }
  zCubeCounter[m_currentX+offsetX][m_currentY+offsetY] = 0;
	  
  // have a for loop that iterates through from m_currentX,m_currentY and generate a cube
  // transform the cursor by the z_transform
  for(int i=0; i < offsetZ; i++)
  {
    int preset = -1;
    for(int z=0; z < m_cubes.size(); z++)
    {
      struct Cube current_cube = (struct Cube) m_cubes.at(z);
      if(current_cube.x == m_currentX && current_cube.y == m_currentY && current_cube.z == i)
	preset = current_cube.preset;
    }
    createCube(m_currentX+offsetX, m_currentY+offsetY, i, preset);
    zCubeCounter[m_currentX+offsetX][m_currentY+offsetY]=zCubeCounter[m_currentX+offsetX][m_currentY+offsetY]+1;
    m_numInstances++;
  }
  m_currentX+=offsetX;
  m_currentY+=offsetY;
  transformCursor(offsetZ);
}

//Create a new cube
void A1::createCube(int x, int y, int z, int presetColour = -1)
{
  float verts[] = {
				0.0f,0.0f,0.0f, //A
				1.0f,0.0f,0.0f,
				0.0f,0.0f,1.0f,
				1.0f,0.0f,1.0f,	//B
				0.0f,0.0f,1.0f,
				1.0f,0.0f,0.0f,
				0.0f,0.0f,0.0f, //C
				0.0f,1.0f,0.0f,
				0.0f,0.0f,1.0f,
				0.0f,1.0f,0.0f, //D
				0.0f,1.0f,1.0f,
				0.0f,0.0f,1.0f,
				1.0f,1.0f,0.0f, //E
				1.0f,0.0f,0.0f,
				0.0f,1.0f,0.0f,
				0.0f,0.0f,0.0f, //F
				0.0f,1.0f,0.0f,
				1.0f,0.0f,0.0f,
				1.0f,1.0f,0.0f, //G
				1.0f,0.0f,0.0f,
				1.0f,0.0f,1.0f,
				1.0f,1.0f,0.0f, //H
				1.0f,0.0f,1.0f,
				1.0f,1.0f,1.0f,
				1.0f,1.0f,1.0f, //I
				1.0f,0.0f,1.0f,
				0.0f,0.0f,1.0f,
				1.0f,1.0f,1.0f, //J
 				0.0f,1.0f,1.0f,
				0.0f,0.0f,1.0f,
				1.0f,1.0f,1.0f,	//K
				0.0f,1.0f,1.0f,
				1.0f,1.0f,0.0f,
				0.0f,1.0f,1.0f, //L
				0.0f,1.0f,0.0f,
				1.0f,1.0f,0.0f};
				
	transformCube(verts, x, z, y, sizeof(verts)/sizeof(float));
	// Create the vertex array to record buffer assignments.
	
	struct Cube newCube;
	newCube.x = x;
	newCube.y = y;
	newCube.z = z;
	if(presetColour == -1)
	  newCube.preset = current_col;
	else
	  newCube.preset = presetColour;
	glGenVertexArrays( 1, &(newCube.vao));
	glBindVertexArray( newCube.vao );
	
	// Create the cube vertex buffer
	glGenBuffers( 1, &(newCube.vbo) );
	glBindBuffer( GL_ARRAY_BUFFER, newCube.vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(verts),
		verts, GL_STATIC_DRAW );
	
	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
	
	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	
	m_cubes.push_back(newCube);
}

//Used to move the cursor;
void A1::transformCursor(int z)
{
	  float verts[] = {
		    0.0f,0.0f,0.0f, //A
		    1.0f,0.0f,0.0f,
		    0.0f,0.0f,1.0f,
		    1.0f,0.0f,1.0f, //B
		    0.0f,0.0f,1.0f,
		    1.0f,0.0f,0.0f};
	  transformCube(verts, m_currentX, z, m_currentY, sizeof(verts)/sizeof(float));
	  glBindBuffer(GL_ARRAY_BUFFER, m_cursor_vbo);
	  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
	  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.
		
		for(int i=0;i<8;i++)
		{
		    ImGui::PushID( i );
		    ImGui::ColorEdit3( "##Colour", colour[i] );
		    ImGui::SameLine();
		    if( ImGui::RadioButton( "##Col", &current_col, i ) ) {
			// Select this colour.
			//Update the colour of all cubes with this preset
			for(int z=0; z < m_cubes.size(); z++)
			{
			  struct Cube current_cube = (struct Cube) m_cubes.at(z);
			  if(current_cube.x == m_currentX && current_cube.y == m_currentY)
			    m_cubes.at(z).preset = current_col;
			}
		    }
		    ImGui::PopID();
		}
		
		//Set the colour of the current preset
		m_presetColours.at(current_col) = glm::vec3(colour[current_col][0],
							    colour[current_col][1],
							    colour[current_col][2]);
/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
    

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		for(int i=0; i < m_numInstances; i++)
		{
		  //Set the colour before drawing
		  glm::vec3 cubeColour =  m_presetColours.at( m_cubes.at(i).preset );
		  glUniform3f( col_uni, cubeColour.x, cubeColour.y, cubeColour.z);
		  //Draw the actual cube
		  glBindVertexArray(m_cubes.at(i).vao);
		  glDrawArrays(GL_TRIANGLES,0,32);
		}
		
		// Draw the cursor
		glBindVertexArray(m_cursor_vao);
		glUniform3f(col_uni, 0, 1, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// Highlight the active square.
	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//---------------------------(key == GLFW_KEY_BACKSPACE-------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
	  if(ImGui::IsMouseDragging())
	  {
	    //Which way is the cursor dragging
	    if(m_lastPosX > xPos)
	      m_posX +=0.75f;
	    else if(m_lastPosX < xPos)
	      m_posX -=0.75f;
	    //Update the last point
	    m_lastPosX = xPos;
	    
	    //Rotate the camera by the y-axis by figuring out how much cursor dragged
	    m_posY = float(DIM)*2.0*M_SQRT1_2;
	    float camX = m_distance * -sinf(m_posX*(PI/180)) * cosf((m_posY)*(PI/180));
	    float camZ = -m_distance * cosf((m_posX)*(PI/180)) * cosf((m_posY)*(PI/180));
	    view = glm::lookAt( 
		    glm::vec3( camX, m_posY, camZ ),
		    glm::vec3( 0.0f, 0.0f, 0.0f ),
		    glm::vec3( 0.0f, 1.0f, 0.0f ) );
		
	    eventHandled = true;
	  } 
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out based on movement of the scroll wheel
	current_proj -= (float)yOffSet;
	proj = glm::perspective( 
			glm::radians( current_proj ),
				float( m_framebufferWidth ) / float( m_framebufferHeight ),
				1.0f, 1000.0f );
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
    int z_transform = zCubeCounter[m_currentX][m_currentY];
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_SPACE) {
	    //Generate a cube, transform the cursor above the cube and increment number of cubes and stack of cubes
	    createCube(m_currentX, m_currentY, z_transform);
	    transformCursor(z_transform+1);
	    zCubeCounter[m_currentX][m_currentY]=z_transform+1;
	    m_numInstances++;
	    eventHandled = true;
        }
        else if ( key == GLFW_KEY_RIGHT && m_currentX < DIM-1 && (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT)))
	{
	  copyCubes(1,0,z_transform);
	}
	else if ( key == GLFW_KEY_LEFT && m_currentX > 0 && (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT)))
	{
	  copyCubes(-1,0,z_transform);
	}
	else if ( key == GLFW_KEY_UP && m_currentY > 0 && (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT)))
	{
	  copyCubes(0,-1,z_transform);
	}
	else if ( key == GLFW_KEY_DOWN && m_currentY < DIM-1 && (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT)))
	{
	  copyCubes(0,1,z_transform);
	}
        else if (key == GLFW_KEY_RIGHT && m_currentX < DIM-1)
	{
	  m_currentX+=1;
	  z_transform = zCubeCounter[m_currentX][m_currentY];
	  transformCursor(z_transform);
	  eventHandled = true;
	}
	else if (key == GLFW_KEY_LEFT && m_currentX > 0)
	{
	  m_currentX-=1;
	  z_transform = zCubeCounter[m_currentX][m_currentY];
	  transformCursor(z_transform);
	  eventHandled = true;
	}
	else if (key == GLFW_KEY_UP && m_currentY > 0)
	{
	  m_currentY-=1;
	  z_transform = zCubeCounter[m_currentX][m_currentY];
	  transformCursor(z_transform);
	  eventHandled = true;
	}
	else if (key == GLFW_KEY_DOWN && m_currentY < DIM-1)
	{
	  m_currentY+=1;
	  z_transform = zCubeCounter[m_currentX][m_currentY];
	  transformCursor(z_transform);
	  eventHandled = true;
	}
	else if (key == GLFW_KEY_BACKSPACE && z_transform > 0 && m_numInstances > 0)
	{
	  //Remove the topmost cube on this stack, decrement various counters
	  removeCube(m_currentX, m_currentY, z_transform-1);
	  m_numInstances--;
	  transformCursor(z_transform-1);
	  zCubeCounter[m_currentX][m_currentY] = z_transform - 1;
	  eventHandled=true;
	}
	else if (key == GLFW_KEY_Q)
	{
	  //exit
	  glfwSetWindowShouldClose(m_window, GL_TRUE);
	  eventHandled = true;
	}
	else if (key == GLFW_KEY_R)
	{
	  //Reset everything
	  
	  //Re-initialize the member data
	  m_cubes.clear();
	  m_numInstances = 0;
	  m_currentX = 0;
	  m_currentY = 0;
	  transformCursor(0);
	  
	  //Reset the preset colours
	  initPresetColours();
	  
	  //Reset stack counter
	  for(int i=0; i < DIM; i++)
	    for(int j=0; j < DIM; j++)
	      zCubeCounter[i][j] = 0;
	    
	  //Reset the zoom
	  current_proj = 45.0f;
	  proj = glm::perspective( 
		glm::radians( current_proj ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
	  
	  //Reset the rotation
	  m_lastPosX = 180.0f;
	  m_posX = 180.0f;
	  float y = float(DIM)*2.0*M_SQRT1_2;
	  float camX = m_distance * -sinf(180.0f*(PI/180)) * cosf((y)*(PI/180));
	  float camZ = -m_distance * cosf((180.0f)*(PI/180)) * cosf((y)*(PI/180));
	  view = glm::lookAt( 
		glm::vec3( camX, y, camZ ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );
	  eventHandled = true;
	}
    }

    return eventHandled;
}
