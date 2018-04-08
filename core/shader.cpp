#include "shader.hpp"
#include "common.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

Shader::Shader(string VertexShaderCode, string FragmentShaderCode){
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLint Result = GL_FALSE;
	int InfoLogLength;

GLenum err;
	// Compile Vertex Shader
	DEBUG(Debug::Info, "Compiling shader\n");
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	//printf("Vertex source path: %s\n", VertexSourcePointer);
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "before vertex init OpenGL error: " << err << std::endl;
    }
	glCompileShader(VertexShaderID);

	/*while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "RIGHT after vertex init OpenGL error: " << err << std::endl;
    }*/

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		DEBUG(Debug::Warning, "%s\n", &VertexShaderErrorMessage[0]);
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "after vertex init OpenGL error: " << err << std::endl;
    }


	// Compile Fragment Shader
	DEBUG(Debug::Info, "Compiling shader\n");
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		DEBUG(Debug::Warning, "%s\n", &FragmentShaderErrorMessage[0]);
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "after fragment compileOpenGL error: " << err << std::endl;
    }

	// Link the program
	DEBUG(Debug::Info, "Linking program\n");
	_programe_id = glCreateProgram();
	glAttachShader(_programe_id, VertexShaderID);
	glAttachShader(_programe_id, FragmentShaderID);
	glLinkProgram(_programe_id);

	// Check the program
	glGetProgramiv(_programe_id, GL_LINK_STATUS, &Result);
	glGetProgramiv(_programe_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(_programe_id, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		DEBUG(Debug::Warning, "%s\n", &ProgramErrorMessage[0]);
	}

	
	glDetachShader(_programe_id, VertexShaderID);
	glDetachShader(_programe_id, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "after everytgubg OpenGL error: " << err << std::endl;
    }

}

Shader::~Shader(){
    glDeleteProgram(_programe_id);
}

Shader* Shader::fromFiles(string vertex_file_path, string fragment_file_path){

	// Create the shaders
	

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	} else
        throw new OpenGLException("Impossible to open " + vertex_file_path);

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	} else
        throw new OpenGLException("Impossible to open " + fragment_file_path);

	return new Shader(VertexShaderCode, FragmentShaderCode);
}
