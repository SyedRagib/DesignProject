#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

const float PI = 3.14159265f;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.reserve(kMaxVertices);
	colours.reserve(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called to return a 4x4 rotation matrix on given axis 
 */
glm::mat4 A2::createRotationMatrix(int axis, float theta)
{
	glm::mat4 rotationMatrix;
	rotationMatrix[3] = vec4(0.0f,0.0f,0.0f,1.0f);
	float cosValue = cos(theta);
	float sineValue = sin(theta);
	switch(axis)
	{
		case X_Axis:
			rotationMatrix[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);
			rotationMatrix[1] = vec4(0.0f, cosValue, sineValue, 0.0f);
			rotationMatrix[2] = vec4(0.0f, -1.0f*sineValue, cosValue, 0.0f);
		break;
		
		case Y_Axis:
			rotationMatrix[0] = vec4(cosValue, 0.0f, -1.0f*sineValue, 0.0f);
			rotationMatrix[1] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
			rotationMatrix[2] = vec4(sineValue, 0.0f, cosValue, 0.0f);
		break;
		
		case Z_Axis:
			rotationMatrix[0] = vec4(cosValue, sineValue, 0.0f, 0.0f);
			rotationMatrix[1] = vec4(-1.0f*sineValue, cosValue, 0.0f, 0.0f);
			rotationMatrix[2] = vec4(0.0f,0.0f, 1.0f, 0.0f);
		break;
	}

	return rotationMatrix;
}

void A2::rotateModel(bool left, bool middle, bool right, float theta)
{
	if(left)
		m_translateMatrix = m_translateMatrix*createRotationMatrix(X_Axis,theta);
	
	if(middle)
		m_translateMatrix = m_translateMatrix*createRotationMatrix(Y_Axis,theta);

	if(right)
		m_translateMatrix = m_translateMatrix*createRotationMatrix(Z_Axis,theta);
}

void A2::rotateView(bool left, bool middle, bool right, float theta)
{
	if(left)
		m_viewMatrix = m_viewMatrix*createRotationMatrix(X_Axis, theta);
	
	if(middle)
		m_viewMatrix = m_viewMatrix*createRotationMatrix(Y_Axis, theta);

	if(right)
		m_viewMatrix = m_viewMatrix*createRotationMatrix(Z_Axis, theta);
}

//----------------------------------------------------------------------------------------
/*
 * Called to return a 4x4 translation matrix
 */
glm::mat4 A2::createTranslationMatrix(float deltaX,float deltaY, float deltaZ)
{
	glm::mat4 translationMatrix;
	translationMatrix[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	translationMatrix[1] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	translationMatrix[2] = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	translationMatrix[3] = vec4(deltaX, deltaY, deltaZ, 1.0f);

	return translationMatrix;
}

void A2::translateModel(bool left, bool middle, bool right, float delta)
{
	m_translateMatrix = m_translateMatrix*createTranslationMatrix(left?delta:0.0f,middle?delta:0.0f,right?delta:0.0f);
}

void A2::translateView(bool left, bool middle, bool right, float delta)
{
	m_viewMatrix = m_viewMatrix*createTranslationMatrix(left?delta:0.0f,middle?delta:0.0f,right?delta:0.0f);
}

//----------------------------------------------------------------------------------------
/*
 * Called to return a 4x4 scale matrix 
 */
glm::mat4 A2::createScalingMatrix(float scaleX, float scaleY, float scaleZ)
{
	glm::mat4 scaleMatrix;
	scaleMatrix[0] = vec4(scaleX, 0.0f, 0.0f, 0.0f);
	scaleMatrix[1] = vec4(0.0f, scaleY, 0.0f, 0.0f);
	scaleMatrix[2] = vec4(0.0f, 0.0f, scaleZ, 0.0f);
	scaleMatrix[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	return scaleMatrix;
}

void A2::scaleModel(bool left, bool middle, bool right, float delta)
{
	m_scalingMatrix = m_scalingMatrix*createScalingMatrix(left?1.0+delta:1.0f, middle?1.0+delta:1.0f, right?1.0+delta:1.0f);
}

glm::mat4 A2::createIdentityMatrix()
{
	glm::mat4 identityMatrix;
	identityMatrix[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	identityMatrix[1] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	identityMatrix[2] = vec4(0.0f, 0.0f, 1.0f, 0.0f);
	identityMatrix[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return identityMatrix;
}

void A2::updatePerspectiveMatrixHelper()
{
	float cotan = 1.0f/tan(m_fieldOfView/2);
	glm::mat4 temp_perspectiveMatrix;
	temp_perspectiveMatrix[0] = vec4(cotan/m_aspectRatio, 0.0f, 0.0f, 0.0f);
	temp_perspectiveMatrix[1] = vec4(0.0f, cotan, 0.0f, 0.0f);
	temp_perspectiveMatrix[2] = vec4(0.0f, 0.0f, -1.0f*(m_farPlane+m_nearPlane)/(m_farPlane-m_nearPlane), -1.0f);	//Since we're looking down the z axis
	temp_perspectiveMatrix[3] = vec4(0.0f, 0.0f,  -2.0f*m_farPlane*m_nearPlane/(m_farPlane-m_nearPlane), 0.0f);

	m_perspectiveMatrix = temp_perspectiveMatrix;
}

void A2::changePerspective(bool left, bool middle, bool right, float delta)
{
	if(left)
		m_fieldOfView += delta*0.05;
	if(middle)
		m_nearPlane += delta*0.05;
	if(right)
		m_farPlane += delta*0.05;

	if (m_farPlane < 0.0f)
		m_farPlane = 0.0f;
	if(m_nearPlane < 0.0f)
		m_nearPlane = 0.0f;

	updatePerspectiveMatrixHelper();
}

void A2::updatePerspectiveMatrix(float fov, float nearPlane, float farPlane, float alpha)
{
	m_fieldOfView = fov;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	m_aspectRatio = alpha;

	updatePerspectiveMatrixHelper();
}

void A2::initCube()
{
	
	//Initialize the member variables
	m_fieldOfView = 30.0f*PI/180.0f;
	m_nearPlane = 2.0f;
	m_farPlane = 100.0f;
	m_aspectRatio = 1;
	m_currentMode = 3;
	m_prevX = 0.0f;

	m_cubePoints.clear();
	m_cubePoints.push_back(vec4(-1.0,  1.0f, -1.0f, 1.0f));
	m_cubePoints.push_back(vec4( 1.0,  1.0f, -1.0f, 1.0f));
	m_cubePoints.push_back(vec4( 1.0,  1.0f,  1.0f, 1.0f));
	m_cubePoints.push_back(vec4(-1.0,  1.0f,  1.0f, 1.0f));
	m_cubePoints.push_back(vec4(-1.0, -1.0f, -1.0f, 1.0f));
	m_cubePoints.push_back(vec4( 1.0, -1.0f, -1.0f, 1.0f));
	m_cubePoints.push_back(vec4( 1.0, -1.0f,  1.0f, 1.0f));
	m_cubePoints.push_back(vec4(-1.0, -1.0f,  1.0f, 1.0f));

	//Setup the matrices
	updatePerspectiveMatrixHelper();
	//Camera coordinates
	// same thing as lookAt(vec3(0.0f,0.0f,12.0f),vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));
	m_viewMatrix = createIdentityMatrix();
	m_viewMatrix[3][2]=-12.0f;

	m_translateMatrix = createIdentityMatrix();
	m_scalingMatrix = createIdentityMatrix();

	//Setup gnomon
	m_gnomon.push_back(vec4(0.0f,0.0f,0.0f,1.0f));
	m_gnomon.push_back(vec4(0.5f,0.0f,0.0f,1.0f));
	m_gnomon.push_back(vec4(0.0f,0.5f,0.0f,1.0f));
	m_gnomon.push_back(vec4(0.0f,0.0f,0.5f,1.0f));
}

void A2::drawCubeLines(vector<vec4> points)
{
	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	
	for(int i=0; i<4; i++)
	{
		drawCubeLinesHelper(points[i], points[(i+1)%4]);
		drawCubeLinesHelper(points[i+4], points[4+((i+1)%4)]);
		drawCubeLinesHelper(points[i], points[i+4]);
	}
}

vec4 A2::homogenization(vec4 point)
{
	return point = vec4( point.x/point.z, point.y/point.z, point.z/point.z, 1);
}

//Draws black lines for the viewport
void A2::drawViewport()
{

	float width = ImGui::GetWindowWidth();
	float height = ImGui::GetWindowHeight();
	m_viewport.clear();
	m_viewport.push_back(vec3(width*0.05,height*0.05, 1.0));
	m_viewport.push_back(vec3(width*0.95,height*0.95, 1.0));
	setLineColour(vec3(0.0,0.0f,0.0f));
	drawLine(vec2(2.0f*0.05f-1,2.0f*0.05f-1), vec2(2.0f*0.95f-1,2.0f*0.05f-1));
	drawLine(vec2(2.0f*0.05f-1,2.0f*0.05f-1), vec2(2.0f*0.05f-1,2.0f*0.95f-1));
	drawLine(vec2(2.0f*0.95f-1,2.0f*0.05f-1), vec2(2.0f*0.95f-1,2.0f*0.95f-1));
	drawLine(vec2(2.0f*0.95f-1,2.0f*0.95f-1), vec2(2.0f*0.05f-1,2.0f*0.95f-1));

}

void A2::drawCubeLinesHelper(vec4 a, vec4 b)
{
	bool nearPlaneClipped = true;// && clip(a,b, vec4(0.0f,0.0f, m_nearPlane,1.0f),vec4(0.0f,0.0f,-1.0f,0.0f));
	if(!nearPlaneClipped)
		return;

	a = m_perspectiveMatrix * a;
	b = m_perspectiveMatrix * b;
	a = homogenization(a);
	b = homogenization(b);

	drawLine(vec2(a.x, a.y), vec2(b.x, b.y));
}

//line clip algorithm
bool A2::clip(vec4 &pointA, vec4 &pointB, vec4 point, vec4 normal)
{
	double cA = dot((pointA - point), normal);
	double cB = dot((pointB - point), normal);
	if( cA < 0 && cB < 0)
		return false;
	else if( cA >= 0 && cB >= 0)
	{
		return true;
	}
	else
	{
		float t = cA/(cA-cB);
		assert(t > 0.0f && t < 1.0f);
		if (cA < 7)
			pointA = pointA - (pointB-pointA)*t;
		else if (cB < 7)
			pointB = pointB - (pointB-pointA)*t;
	}

	return true;
}

void A2::drawCube()
{
	vector<vec4> new_vertices;
	for(int i=0; i < m_cubePoints.size(); i++)
		new_vertices.push_back(m_viewMatrix*m_translateMatrix*m_scalingMatrix*m_cubePoints[i]);
	
	drawCubeLines(new_vertices);
}

void A2::drawGnomon(vec4 a, vec4 b, vec3 colour)
{
	bool nearPlaneClipped = true; // && clip(a,b, vec4(0.0f,0.0f, -1*m_nearPlane,1.0f),vec4(0.0f,0.0f,-1.0f,0.0f));
	
	if(!nearPlaneClipped)
		return;

	a = m_perspectiveMatrix * a;
	b = m_perspectiveMatrix * b;
	a = homogenization(a);
	b = homogenization(b);

	
	bool farPlaneClipped = true; // && clip(a,b,vec4(0.0f,0.0f,1.0f,1.0f), vec4(0.0f,0.0f,1.0f,0.0f));

	if(!farPlaneClipped)
		return;
	
	setLineColour(colour);
	drawLine(vec2(a.x, a.y), vec2(b.x, b.y));
}

void A2::drawWorldAxes()
{
	vector<vec4> modelGnomon;

	for(int i=0; i < 4; i++)
		modelGnomon.push_back(m_viewMatrix*m_gnomon[i]);

	//Draw the world axes in secondary colours
	drawGnomon(modelGnomon[0],modelGnomon[1],vec3(0.0f, 1.0f, 0.0f));
	drawGnomon(modelGnomon[0],modelGnomon[2],vec3(1.0f, 0.64f, 0.0f));
	drawGnomon(modelGnomon[0],modelGnomon[3],vec3(0.5f, 0.0f, 5.0f));
}

void A2::drawModelAxes()
{
	vector<vec4> modelGnomon;

	for(int i=0; i < 4; i++)
		modelGnomon.push_back(m_viewMatrix*m_translateMatrix*m_gnomon[i]);

	//Draw the model axes in primary colours
	drawGnomon(modelGnomon[0],modelGnomon[1],vec3(1.0f, 0.0f, 0.0f));
	drawGnomon(modelGnomon[0],modelGnomon[2],vec3(1.0f, 1.0f, 0.0f));
	drawGnomon(modelGnomon[0],modelGnomon[3],vec3(0.0f, 0.0f, 1.0f));
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	initCube();

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);


	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//----------------------------------------------------------------------------------------
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & v0,   // Line Start (NDC coordinate)
		const glm::vec2 & v1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = v0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = v1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();
	drawCube();
	drawModelAxes();
	drawWorldAxes();
	drawViewport();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}
	
	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...
		if( ImGui::Button( "Restart" ) ) {
			initCube();
		}

		ImGui::RadioButton("Rotate View", &m_currentMode, 0);
		ImGui::RadioButton("Translate View", &m_currentMode, 1);
		ImGui::RadioButton("Perspective View", &m_currentMode, 2);
		ImGui::RadioButton("Rotate Model", &m_currentMode, 3);
		ImGui::RadioButton("Translate Model", &m_currentMode, 4);
		ImGui::RadioButton("Scale Model", &m_currentMode, 5);

		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text("Near plane: %.1f", m_nearPlane);
		ImGui::Text("Far plane: %.1f", m_farPlane);
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	bool leftClick = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
	bool middleClick = glfwGetMouseButton(m_window, 2);
	bool rightClick = glfwGetMouseButton(m_window, 1);
	float difference = xPos-m_prevX;
	m_prevX = xPos;

	if(!leftClick && !rightClick && !middleClick)
		return eventHandled;

	if(m_currentMode == ScaleModel)
		scaleModel(leftClick, middleClick, rightClick, difference/1000.0f);
	else if(m_currentMode == TranslateModel)
		translateModel(leftClick, middleClick, rightClick, difference*0.1f);
	else if(m_currentMode == RotateModel)
		rotateModel(leftClick, middleClick, rightClick, difference*0.05f);
	else if(m_currentMode == RotateView)
		rotateView(leftClick, middleClick, rightClick, difference*0.05f);
	else if(m_currentMode == TranslateView)
		translateView(leftClick, middleClick, rightClick, difference*0.05);
	else if(m_currentMode == Perspective)
		changePerspective(leftClick, middleClick, rightClick, difference);

	eventHandled = true;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if(action == GLFW_PRESS)
	{
		if(key == GLFW_KEY_A)
		{
			initCube();
			eventHandled = true;
		}
		else if (key == GLFW_KEY_Q)
		{
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		else if (key == GLFW_KEY_O)
		{
			m_currentMode = 0;
			eventHandled = true;
		}
		else if (key == GLFW_KEY_N)
		{
			m_currentMode = 1;
			eventHandled = true;
		}
		else if (key == GLFW_KEY_P)
		{
			m_currentMode = 2;
			eventHandled = true;
		}
		else if (key == GLFW_KEY_R)
		{
			m_currentMode = 3;
			eventHandled = true;
		}
		else if (key == GLFW_KEY_T)
		{
			m_currentMode = 4;
			eventHandled = true;
		}
		else if (key == GLFW_KEY_S)
		{
			m_currentMode = 5;
			eventHandled = true;
		}
	}
	return eventHandled;
}
