#include "shader.hpp"
#include "common.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

GLuint Shader::SHADER_IN_USE = 0;

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
	
	while ((err = glGetError()) != GL_NO_ERROR)
        throw new OpenGLException("Shader initialisation", err);
        
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		DEBUG(Debug::Warning, "%s\n", &VertexShaderErrorMessage[0]);
	}

	while ((err = glGetError()) != GL_NO_ERROR)
        throw new OpenGLException("Vertex compilation", err);


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

	while ((err = glGetError()) != GL_NO_ERROR)
        throw new OpenGLException("Fragment compilation", err);

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
	while ((err = glGetError()) != GL_NO_ERROR)
        throw new OpenGLException("Impossible finalyse shader loading", 0);

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
        throw new OpenGLException("Impossible to open " + vertex_file_path, 0);

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	} else
        throw new OpenGLException("Impossible to open " + fragment_file_path, 0);

	return new Shader(VertexShaderCode, FragmentShaderCode);
}


GLuint Shader::getUniformLocation(std::string id){
    if (SHADER_IN_USE != _programe_id) 
        throw new ShaderNotUseException(this);
    return getUniformLocation(id.c_str());
}

GLuint Shader::getUniformLocation(const char* id){
    if (SHADER_IN_USE != _programe_id) 
        throw new ShaderNotUseException(this);
    return glGetUniformLocation(_programe_id, id);
}

void Shader::use() {
    if (SHADER_IN_USE != _programe_id){
        SHADER_IN_USE = _programe_id;
        glUseProgram(_programe_id);
    }
}
void Shader::deuse() { 
    if (SHADER_IN_USE == _programe_id){
        SHADER_IN_USE = 0;
        glUseProgram(0); 
    }
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
	Debug::CheckOpenGLError("Set vec3");
    if (SHADER_IN_USE != _programe_id) 
        throw new ShaderNotUseException(this);
    if (glGetUniformLocation(_programe_id, name.c_str()) < 0) 
        throw new ShaderUniformNotFoundException("No uniform '"+ name + "' found in the shader");
    glUniform3fv(glGetUniformLocation(_programe_id, name.c_str()), 1, &value[0]); 
}        
void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    if (SHADER_IN_USE != _programe_id) 
        throw new ShaderNotUseException(this);
    if (glGetUniformLocation(_programe_id, name.c_str()) < 0) 
        throw new ShaderUniformNotFoundException("No uniform '"+ name + "' found in the shader");
    glUniform3f(glGetUniformLocation(_programe_id, name.c_str()), x, y, z); 
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat, bool inverse) const {
    if (SHADER_IN_USE != _programe_id) 
        throw new ShaderNotUseException(this);
    if (glGetUniformLocation(_programe_id, name.c_str()) < 0) 
        throw new ShaderUniformNotFoundException("No uniform '"+ name + "' found in the shader");
    glUniformMatrix4fv(glGetUniformLocation(_programe_id, name.c_str()), 1, inverse, &mat[0][0]);
}
void Shader::setFloat(const std::string &name, float val) const {
    if (SHADER_IN_USE != _programe_id) 
        throw new ShaderNotUseException(this);
    if (glGetUniformLocation(_programe_id, name.c_str()) < 0) 
        throw new ShaderUniformNotFoundException("No uniform '"+ name + "' found in the shader");
    glUniform1f(glGetUniformLocation(_programe_id, name.c_str()), val);
}
void Shader::setInt(const std::string &name, int val) const {
    if (SHADER_IN_USE != _programe_id) 
        throw new ShaderNotUseException(this);
    if (glGetUniformLocation(_programe_id, name.c_str()) < 0) 
        throw new ShaderUniformNotFoundException("No uniform '"+ name + "' found in the shader");
    glUniform1i(glGetUniformLocation(_programe_id, name.c_str()), val);
}
void Shader::setBool(const std::string &name, bool val) const {
    if (SHADER_IN_USE != _programe_id) 
        throw new ShaderNotUseException(this);
    if (glGetUniformLocation(_programe_id, name.c_str()) < 0) 
        throw new ShaderUniformNotFoundException("No uniform '"+ name + "' found in the shader");
    glUniform1i(glGetUniformLocation(_programe_id, name.c_str()), val);
}
