#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


//Define constants
#define X_Axis 0
#define Y_Axis 1
#define Z_Axis 2

#define RotateView 0
#define TranslateView 1
#define Perspective 2
#define RotateModel 3
#define TranslateModel 4
#define ScaleModel 5

// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

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

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	//My members
	float m_fieldOfView;
	float m_nearPlane;
	float m_farPlane;
	float m_aspectRatio;
	float m_prevX;

	std::vector<glm::vec4> m_cubePoints;
	std::vector<glm::vec4> m_gnomon;
	std::vector<glm::vec3> m_viewport;
	//Keep track of the matrices
	glm::mat4 m_translationMatrix;

	glm::mat4 m_viewMatrix;
	glm::mat4 m_translateMatrix;
	glm::mat4 m_scalingMatrix;
	glm::mat4 m_perspectiveMatrix;
	int m_currentMode;

	//My methods
	glm::mat4 createRotationMatrix(int axis, float theta);
	glm::mat4 createTranslationMatrix(float deltaX,float deltaY, float deltaZ);
	glm::mat4 createScalingMatrix(float scaleX, float scaleY, float scaleZ);
	glm::mat4 createIdentityMatrix();

	void updatePerspectiveMatrix(float fov, float nearPlane, float farPlane, float alpha);
	void updatePerspectiveMatrixHelper();

	bool clip(glm::vec4 &pointA, glm::vec4 &pointB, glm::vec4 point, glm::vec4 normal);

	void initCube();
	void drawCube();
	void drawCubeLines(std::vector<glm::vec4> points);
	void drawCubeLinesHelper(glm::vec4 a, glm::vec4 b);
	glm::vec4 homogenization(glm::vec4 point);
	
	void drawModelAxes();
	void drawWorldAxes();
	void drawGnomon(glm::vec4 a, glm::vec4 b, glm::vec3 colour);

	void drawViewport();

	void scaleModel(bool left, bool middle, bool right, float delta);
	void translateModel(bool left, bool middle, bool right, float delta);
	void rotateModel(bool left, bool middle, bool right, float theta);

	void rotateView(bool left, bool middle, bool right, float theta);
	void translateView(bool left, bool middle, bool right, float delta);
	void changePerspective(bool left, bool middle, bool right, float delta);

};
