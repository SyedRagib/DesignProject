#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{
	/*
	for(int i=0; i< m_jointNodes.size(); i++)
		delete m_jointNodes[i];
	*/
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{

	

	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	//My stuff to initialize
	m_matrix_stack.clear();
	m_translationMatrix = mat4();
	m_mode = PositionMode;
	m_leftClick=false;
	m_middleClick=false;
	m_rightClick=false;
	m_headRotation = 30.0f;
	//m_jointNodes.clear();
	findAllJoints(*m_rootNode);
	m_circleMode = m_zBufferMode = true;
	m_frontfaceCullingMode = m_backfaceCullingMode = false;
	m_prevMouseX = m_prevMouseY = 0.0f;

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}


void A3::findAllJoints(SceneNode &node)
{
	if (node.m_nodeType == NodeType::JointNode)
	{
		m_jointNodes.push_back(new SceneNode(node.m_name));
		m_jointNodes[m_jointNodes.size()-1] = &node;
	}
	//m_jointNodes.push_back(&node);

	for(SceneNode* childrenNode : node.children)
		findAllJoints(*childrenNode);
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.05f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

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
		if(ImGui::BeginMenu("Application"))
		{
			if(ImGui::Button("Reset Position"))
			m_translationMatrix = mat4();
		
			if(ImGui::Button("Reset Orientation"))
				m_rotationMatrix = mat4();

			if(ImGui::Button("Reset Joints"))
				resetAllJoint();

			if(ImGui::Button("Reset All"))
				resetAll();

			ImGui::EndMenu();
		}
		
		if(ImGui::BeginMenu("Edit"))
		{
			if(ImGui::Button("Undo") && m_undoStack.size() > 0)
			{
				Entry currentEntry = m_undoStack[0];
				undoRedoJoint(currentEntry,true);
			}

			if(ImGui::Button("Redo") && m_redoStack.size() > 0)
			{
				Entry currentEntry = m_redoStack[0];
				undoRedoJoint(currentEntry,false);
			}
			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Options"))
		{
			ImGui::Checkbox("Circle", &m_circleMode);
			ImGui::Checkbox("Z-buffer", &m_zBufferMode);
			ImGui::Checkbox("Backface culling", &m_backfaceCullingMode);
			ImGui::Checkbox("Frontface culling", &m_frontfaceCullingMode);
			ImGui::EndMenu();
		}

		ImGui::RadioButton("Position", &m_mode, 0);
		ImGui::RadioButton("Joints", &m_mode, 1);

		ImGui::Text("Clickable Joints");

		for(int i=0; i < m_jointNodes.size(); i++)
		{
			if(ImGui::Checkbox( (m_jointNodes[i]->m_name).c_str(), &(m_jointNodes[i]->isSelected)))
			{
				for(SceneNode * childrenNode : m_jointNodes[i]->children)
					childrenNode->isSelected = !childrenNode->isSelected;
			}
		}

		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}


void A3::resetAllJoint()
{
	for(int j=0; j < m_jointNodes.size(); j++)
	{
		JointNode * jointNode = static_cast<JointNode *>(m_jointNodes[j]);
		jointNode->m_joint_x.current = jointNode->m_joint_x.history = jointNode->m_joint_x.init;
		jointNode->m_joint_y.current = jointNode->m_joint_y.history = jointNode->m_joint_y.init;			
	}
	m_undoStack.clear();
	m_redoStack.clear();
}

void A3::resetAll()
{
	m_headRotation = 0.0f;
	m_translationMatrix = mat4();
	m_rotationMatrix = mat4();
	resetAllJoint();
}

void A3::undoRedoJoint(Entry currentEntry,bool undo)
{
	for(int i=0; i < currentEntry.size(); i++)
	{
		for(int j=0; j < m_jointNodes.size(); j++)
		{
			if( currentEntry[i].nodeId != m_jointNodes[j]->m_nodeId)
				continue;

			JointNode * jointNode = static_cast<JointNode *>(m_jointNodes[j]);
			jointNode->m_joint_x.current = jointNode->m_joint_x.history = currentEntry[i].prevX;
			jointNode->m_joint_y.current = jointNode->m_joint_y.history = currentEntry[i].prevY;		
		}
	}
	if(undo)
	{
		m_redoStack.insert(m_redoStack.begin(), currentEntry);
		m_undoStack.erase(m_undoStack.begin());
	}
	else
	{
		m_undoStack.insert(m_undoStack.begin(), currentEntry);
		m_redoStack.erase(m_redoStack.begin());
	}
	
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
void A3::updateShaderUniformsX(const ShaderProgram &shader, const GeometryNode &node, const glm::mat4 &viewMatrix)
{
	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		
		//vec3 rot_axis = vec3(0,1,0);
		//mat4 rot_matrix = rotate(degreesToRadians(30.0f), rot_axis);
		//mat4 modelView = viewMatrix * rot_matrix * node.trans;

		mat4 final_trans = mat4(node.trans);

		/*
		for(int j=0; j < m_rotMatrix_stack.size(); j++)
			final_trans = m_rotMatrix_stack[j] * final_trans;
		*/
		for(int i=0; i < m_matrix_stack.size(); i++)
			final_trans = m_matrix_stack[i] * final_trans;

		mat4 modelView = viewMatrix * m_translationMatrix * final_trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;				
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		if (!node.isSelected)
			glUniform1f(location, node.material.shininess);
		else
			glUniform1f(location, 0.0f);
		CHECK_GL_ERRORS;

	}
	shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	if(m_frontfaceCullingMode || m_backfaceCullingMode)
		glEnable(GL_CULL_FACE);

	if(m_frontfaceCullingMode && m_backfaceCullingMode)
		glCullFace(GL_FRONT_AND_BACK);
	else if(m_frontfaceCullingMode)
		glCullFace(GL_FRONT);
	else if(m_backfaceCullingMode)
		glCullFace(GL_BACK);
	else
		glDisable(GL_CULL_FACE);

	if(m_zBufferMode)
	{
		glEnable( GL_DEPTH_TEST );
		glDepthMask(true);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);
	}

	glBindVertexArray(m_vao_meshData);
	m_matrix_stack.clear();
	m_matrix_stack.insert(m_matrix_stack.begin(), m_rotationMatrix);
	renderSceneGraph(*m_rootNode);
	m_matrix_stack.erase(m_matrix_stack.begin());

	glDisable( GL_DEPTH_TEST );
	if(m_circleMode)
		renderArcCircle();
}

//Copied from the trackball example we were given access too
void A3::vCalcRotVec(float fNewX, float fNewY, float fOldX, float fOldY, float fDiameter, float *fVecX, float *fVecY, float *fVecZ)
{
   long  nXOrigin, nYOrigin;
   float fNewVecX, fNewVecY, fNewVecZ,        /* Vector corresponding to new mouse location */
         fOldVecX, fOldVecY, fOldVecZ,        /* Vector corresponding to old mouse location */
         fLength;

   /* Vector pointing from center of virtual trackball to
    * new mouse position
    */
   fNewVecX    = fNewX * 2.0 / fDiameter;
   fNewVecY    = fNewY * 2.0 / fDiameter;
   fNewVecZ    = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fNewVecZ < 0.0) {
      fLength = sqrt(1.0 - fNewVecZ);
      fNewVecZ  = 0.0;
      fNewVecX /= fLength;
      fNewVecY /= fLength;
   } else {
      fNewVecZ = sqrt(fNewVecZ);
   }

   /* Vector pointing from center of virtual trackball to
    * old mouse position
    */
   fOldVecX    = fOldX * 2.0 / fDiameter;
   fOldVecY    = fOldY * 2.0 / fDiameter;
   fOldVecZ    = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);
 
   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fOldVecZ < 0.0) {
      fLength = sqrt(1.0 - fOldVecZ);
      fOldVecZ  = 0.0;
      fOldVecX /= fLength;
      fOldVecY /= fLength;
   } else {
      fOldVecZ = sqrt(fOldVecZ);
   }

   /* Generate rotation vector by calculating cross product:
    * 
    * fOldVec x fNewVec.
    * 
    * The rotation vector is the axis of rotation
    * and is non-unit length since the length of a crossproduct
    * is related to the angle between fOldVec and fNewVec which we need
    * in order to perform the rotation.
    */
   *fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
   *fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
   *fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;
}

//Copied from the trackball example we were given access too
void A3::vAxisRotMatrix(float fVecX, float fVecY, float fVecZ)
{
	float fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;

    /* Find the length of the vector which is the angle of rotation
     * (in radians)
     */
    fRadians = sqrt(fVecX * fVecX + fVecY * fVecY + fVecZ * fVecZ);

    /* If the vector has zero length - return the identity matrix */
    if (fRadians > -0.000001 && fRadians < 0.000001) {
        m_rotationMatrix = mat4();
        return;
    }

    /* Normalize the rotation vector now in preparation for making
     * rotation matrix. 
     */
    fInvLength = 1 / fRadians;
    fNewVecX   = fVecX * fInvLength;
    fNewVecY   = fVecY * fInvLength;
    fNewVecZ   = fVecZ * fInvLength;

    /* Create the arbitrary axis rotation matrix */
    double dSinAlpha = sin(fRadians);
    double dCosAlpha = cos(fRadians);
    double dT = 1 - dCosAlpha;

    m_rotationMatrix[0][0] = dCosAlpha + fNewVecX*fNewVecX*dT;
    m_rotationMatrix[0][1] = fNewVecX*fNewVecY*dT - dSinAlpha*fNewVecZ;
    m_rotationMatrix[0][2] = fNewVecZ*fNewVecX*dT + dSinAlpha*fNewVecY;
    m_rotationMatrix[0][3] = 0;

    m_rotationMatrix[1][0] = fNewVecX*fNewVecY*dT + fNewVecZ*dSinAlpha;
    m_rotationMatrix[1][1] = dCosAlpha + fNewVecY*fNewVecY*dT;
    m_rotationMatrix[1][2] = fNewVecZ*fNewVecY*dT - dSinAlpha*fNewVecX;
    m_rotationMatrix[1][3] = 0;

    m_rotationMatrix[2][0] = fNewVecX*fNewVecZ*dT - fNewVecY*dSinAlpha;
    m_rotationMatrix[2][1] = fNewVecY*fNewVecZ*dT + dSinAlpha*fNewVecX;
    m_rotationMatrix[2][2] = dCosAlpha + fNewVecZ*fNewVecZ*dT;
    m_rotationMatrix[2][3] = 0;

    m_rotationMatrix[3][0] = 0;
    m_rotationMatrix[3][1] = 0;
    m_rotationMatrix[3][2] = 0;
    m_rotationMatrix[3][3] = 1;
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	//glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.
	glBindVertexArray(m_vao_meshData);

	/*
	if(root.m_name == "head")
	{
		mat4 temp_matrix = glm::rotate(degreesToRadians(m_headRotation), vec3(0,1,0));
		m_matrix_stack.insert(m_matrix_stack.begin(),temp_matrix);
	}
	*/

	if(root.m_nodeType == NodeType::GeometryNode)
	{
		if(root.m_name == "head" && root.isSelected)
			m_matrix_stack.insert(m_matrix_stack.begin(),glm::rotate(degreesToRadians(m_headRotation), vec3(0,1,0)));

		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(&root);
		updateShaderUniformsX(m_shader, *geometryNode, m_view);
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();

		m_matrix_stack.insert(m_matrix_stack.begin(), root.trans);
	}
	
	if(root.m_nodeType == NodeType::JointNode)
	{
		const JointNode * jointNode = static_cast<const JointNode *>(&root);
		mat4 XrotMatrix = glm::rotate(degreesToRadians((float)jointNode->m_joint_x.current), vec3(1.0,0.0,0.0));
		mat4 YrotMatrix = glm::rotate(degreesToRadians((float)jointNode->m_joint_y.current), vec3(0.0,1.0,0.0));
		m_matrix_stack.insert(m_matrix_stack.begin(), root.trans*XrotMatrix*YrotMatrix);
	}

	if(!root.children.empty())
		for(const SceneNode * childrenNode : root.children)
			renderSceneGraph(*childrenNode);
		
	
	if(root.m_nodeType == NodeType::GeometryNode || root.m_nodeType == NodeType::JointNode)
		m_matrix_stack.erase(m_matrix_stack.begin());
	
	if(root.m_name == "head" && root.isSelected)
		m_matrix_stack.erase(m_matrix_stack.begin());
	

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

void A3::translateModel(float diffX, float diffY)
{
	vec3 amount = vec3(m_leftClick ? diffX : 0.0f, m_leftClick ? diffY : 0.0f, m_middleClick ? diffY : 0.0f);
	m_translationMatrix = glm::translate(amount) * m_translationMatrix;
}

void A3::rotateModel(float diffX)
{
	m_rotationMatrix = glm::rotate(degreesToRadians(diffX), vec3(0,1,0)) * m_rotationMatrix;
}


void A3::rotateJoint(double xPos, double yPos)
{
	float differenceX, differenceY;
	for(int i=0; i < m_jointNodes.size(); i++)
	{
		differenceX = differenceY = 0.0f;
		if(!m_jointNodes[i]->isSelected)
			continue;

		JointNode * jointNode = static_cast<JointNode *>(m_jointNodes[i]);
		differenceX = xPos - jointNode->m_joint_x.prev;
		differenceY = yPos - jointNode->m_joint_y.prev;

		jointNode->m_joint_x.current += differenceX;
		jointNode->m_joint_y.current += differenceY;

		if(jointNode->m_joint_x.current > jointNode->m_joint_x.max)
			jointNode->m_joint_x.current = jointNode->m_joint_x.max;
		else if(jointNode->m_joint_x.current < jointNode->m_joint_x.min)
			jointNode->m_joint_x.current = jointNode->m_joint_x.min;

		if(jointNode->m_joint_y.current > jointNode->m_joint_y.max)
			jointNode->m_joint_y.current = jointNode->m_joint_y.max;
		else if(jointNode->m_joint_y.current < jointNode->m_joint_y.min)
			jointNode->m_joint_y.current = jointNode->m_joint_y.min;

		jointNode->m_joint_x.prev = xPos;
		jointNode->m_joint_y.prev = yPos;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	m_leftClick = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
	m_middleClick = glfwGetMouseButton(m_window, 2);
	m_rightClick = glfwGetMouseButton(m_window, 1);

	float differenceX;
	float differenceY;

	differenceX = xPos-m_prevX;
	differenceY = m_prevY-yPos;
	m_prevX = xPos;
	m_prevY = yPos;

	differenceY = m_prevZ-yPos;
	m_prevZ = yPos;

	if(m_mode == PositionMode)
		translateModel(differenceX*0.01,differenceY*0.01);
	if(m_mode == PositionMode && m_rightClick)
		rotateModel(differenceX);
	if(m_mode == JointMode && m_leftClick)
		rotateJoint(xPos, yPos);
	if(m_mode == JointMode && m_rightClick)
		m_headRotation += differenceX;
	
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if(glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && m_mode == JointMode)
	{
		Entry newEntry;
		newEntry.clear();
		for(int i=0; i < m_jointNodes.size(); i++)
		{
			if(m_jointNodes[i]->isSelected)
			{
				JointNode * jointNode = static_cast<JointNode *>(m_jointNodes[i]);
				if(jointNode->m_joint_x.history == jointNode->m_joint_x.current && jointNode->m_joint_y.history == jointNode->m_joint_y.current)
					continue;
				struct Record newRecord;
				newRecord.nodeId = jointNode->m_nodeId;
				newRecord.prevX = jointNode->m_joint_x.history;
				newRecord.prevY = jointNode->m_joint_y.history;
				newEntry.push_back(newRecord);

				jointNode->m_joint_x.history = jointNode->m_joint_x.current;
				jointNode->m_joint_y.history = jointNode->m_joint_y.current;
			}
		}

		if(newEntry.size() != 0)
			m_undoStack.insert(m_undoStack.begin(),newEntry);
		

		//cout << m_undoStack.size() << endl;
			
	}
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		else if( key == GLFW_KEY_I)
		{
			m_translationMatrix = mat4();
			eventHandled = true;
		}
		else if (key == GLFW_KEY_O)
		{
			m_rotationMatrix = mat4();
			eventHandled = true;
		}
		else if (key == GLFW_KEY_N)
		{
			resetAllJoint();
			eventHandled = true;
		}
		else if (key == GLFW_KEY_A)
		{
			resetAll();
			eventHandled = true;
		}
		else if (key == GLFW_KEY_Q)
		{
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		else if (key == GLFW_KEY_U && m_undoStack.size() > 0)
		{
			Entry currentEntry = m_undoStack[0];
			undoRedoJoint(currentEntry,true);
			eventHandled = true;
		}
		else if (key == GLFW_KEY_R && m_redoStack.size() > 0)
		{
			Entry currentEntry = m_redoStack[0];
			undoRedoJoint(currentEntry,false);
			eventHandled = true;
		}
		else if (key == GLFW_KEY_C)
		{
			m_circleMode = !m_circleMode;
			eventHandled = true;
		}
		else if (key == GLFW_KEY_Z)
		{
			m_zBufferMode = !m_zBufferMode;
			eventHandled = true;
		}
		else if (key == GLFW_KEY_B)
		{
			m_backfaceCullingMode = !m_backfaceCullingMode;
			eventHandled = true;
		}
		else if (key == GLFW_KEY_F)
		{
			m_frontfaceCullingMode = !m_frontfaceCullingMode;
			eventHandled = true;
		}
		else if (key == GLFW_KEY_P)
		{
			m_mode = PositionMode;
			eventHandled = true;
		}
		else if (key == GLFW_KEY_J)
		{
			m_mode = JointMode;
			eventHandled = true;
		}
	}


	// Fill in with event handling code...

	return eventHandled;
}
