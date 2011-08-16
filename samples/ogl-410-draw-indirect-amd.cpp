//**********************************
// OpenGL Multi draw indirect
// 22/06/2011 - 22/06/2011
//**********************************
// Christophe Riccio
// ogl-samples@g-truc.net
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

#include <glf/glf.hpp>

namespace
{
	std::string const SAMPLE_NAME = "OpenGL Multi draw indirect";
	std::string const VERTEX_SHADER_SOURCE(glf::DATA_DIRECTORY + "410/draw-indirect.vert");
	std::string const FRAGMENT_SHADER_SOURCE(glf::DATA_DIRECTORY + "410/draw-indirect.frag");
	int const SAMPLE_SIZE_WIDTH(640);
	int const SAMPLE_SIZE_HEIGHT(480);
	int const SAMPLE_MAJOR_VERSION(4);
	int const SAMPLE_MINOR_VERSION(1);

	glf::window Window(glm::ivec2(SAMPLE_SIZE_WIDTH, SAMPLE_SIZE_HEIGHT));

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(glm::uint32);
	glm::uint32 const ElementData[ElementCount] =
	{
		0, 1, 2,
		0, 2, 3
	};

	GLsizei const VertexCount(8);
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec4);
	glm::vec4 const PositionData[VertexCount] =
	{
		glm::vec4(-1.0f,-1.0f, 0.5f, 1.0f),
		glm::vec4( 1.0f,-1.0f, 0.5f, 1.0f),
		glm::vec4( 1.0f, 1.0f, 0.5f, 1.0f),
		glm::vec4(-1.0f, 1.0f, 0.5f, 1.0f),
		glm::vec4(-0.5f,-1.0f,-0.5f, 1.0f),
		glm::vec4( 0.5f,-1.0f,-0.5f, 1.0f),
		glm::vec4( 1.5f, 1.0f,-0.5f, 1.0f),
		glm::vec4(-1.5f, 1.0f,-0.5f, 1.0f)
	};

    struct DrawArraysIndirectCommand
	{
		GLuint count;
		GLuint primCount;
		GLuint first;
		GLuint baseInstance;
    };

    struct DrawElementsIndirectCommand
	{
		GLuint count;
		GLuint primCount;
		GLuint firstIndex;
		GLint  baseVertex;
		GLuint baseInstance;
    };

	GLuint VertexArrayName(0);
	GLuint ProgramName(0);
	GLuint ArrayBufferName(0);
	GLuint IndirectBufferName(0);
	GLuint ElementBufferName(0);
	GLint UniformMVP(0);
	GLint UniformDiffuse(0);

}//namespace

bool initProgram()
{
	bool Validated = true;
	
	if(Validated)
	{
		GLuint VertShaderName = glf::createShader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
		GLuint FragShaderName = glf::createShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);

		ProgramName = glCreateProgram();
		glAttachShader(ProgramName, VertShaderName);
		glAttachShader(ProgramName, FragShaderName);
		glDeleteShader(VertShaderName);
		glDeleteShader(FragShaderName);
		glLinkProgram(ProgramName);
		Validated = glf::checkProgram(ProgramName);
	}

	// Get variables locations
	if(Validated)
	{
		UniformMVP = glGetUniformLocation(ProgramName, "MVP");
		UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
	}

	return Validated && glf::checkError("initProgram");
}

bool initIndirectBuffer()
{
	DrawElementsIndirectCommand Command[2];
	Command[0].count = ElementCount;
	Command[0].primCount = 1;
	Command[0].firstIndex = 0;
	Command[0].baseVertex = 0;
	Command[0].baseInstance = 0;
	Command[1].count = ElementCount;
	Command[1].primCount = 1;
	Command[1].firstIndex = 0;
	Command[1].baseVertex = 4;
	Command[1].baseInstance = 0;

	glGenBuffers(1, &IndirectBufferName);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IndirectBufferName);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(Command), Command, GL_STATIC_READ);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	return glf::checkError("initIndirectBuffer");
}

bool initArrayBuffer()
{
	glGenBuffers(1, &ArrayBufferName);
    glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
    glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ElementBufferName);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferName);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return glf::checkError("initArrayBuffer");
}

bool initVertexArray()
{
	glGenVertexArrays(1, &VertexArrayName);
    glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ARRAY_BUFFER, ArrayBufferName);
			glVertexAttribPointer(glf::semantic::attr::POSITION, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glEnableVertexAttribArray(glf::semantic::attr::POSITION);
	glBindVertexArray(0);

	return glf::checkError("initVertexArray");
}

bool begin()
{
	bool Validated = true;
	Validated = Validated && glf::checkGLVersion(SAMPLE_MAJOR_VERSION, SAMPLE_MINOR_VERSION);
	Validated = Validated && glf::checkExtension("GL_AMD_multi_draw_indirect");

	if(Validated)
		Validated = initProgram();
	if(Validated)
		Validated = initArrayBuffer();
	if(Validated)
		Validated = initIndirectBuffer();
	if(Validated)
		Validated = initVertexArray();
	return Validated && glf::checkError("begin");
}

bool end()
{
	// Delete objects
	glDeleteBuffers(1, &ArrayBufferName);
	glDeleteBuffers(1, &IndirectBufferName);
	glDeleteBuffers(1, &ElementBufferName);
	glDeleteProgram(ProgramName);
	glDeleteVertexArrays(1, &VertexArrayName);

	return glf::checkError("end");
}

void display()
{
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Window.TranlationCurrent.y));
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Window.RotationCurrent.y, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 View = glm::rotate(ViewRotateX, Window.RotationCurrent.x, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	glViewportIndexedfv(0, &glm::vec4(0, 0, Window.Size.x, Window.Size.y)[0]);
	glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f)[0]);

	glUseProgram(ProgramName);
	glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);
	glUniform4fv(UniformDiffuse, 1, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

	glBindVertexArray(VertexArrayName);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferName); //!\ Need to be called after glBindVertexArray...
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IndirectBufferName);
	glMultiDrawElementsIndirectAMD(GL_TRIANGLES, GL_UNSIGNED_INT, 0, 2, sizeof(DrawElementsIndirectCommand));

	glf::checkError("display");
	glf::swapBuffers();
}

int main(int argc, char* argv[])
{
	return glf::run(
		argc, argv,
		glm::ivec2(::SAMPLE_SIZE_WIDTH, ::SAMPLE_SIZE_HEIGHT), 
		::SAMPLE_MAJOR_VERSION, 
		::SAMPLE_MINOR_VERSION);
}
