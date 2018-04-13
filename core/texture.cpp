#include "texture.hpp"
#include "common.hpp"
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

map<string, Texture*> Texture::LOADED = map<string, Texture*>();


uint Texture::LAST_ID = 0;

Texture::Texture(Type t):
    _type(t), _id(LAST_ID++)
{
	glGenTextures(1, &_texture_id);     
}

Texture::Texture(unsigned char* data, int width, int height):
    Texture()
{	
	// "Bind" the newly created texture : all future texture functions will modify this texture	
    bind();

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete [] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);
    unbind();

}

Texture::~Texture(){
    glDeleteTextures(1, &_texture_id);
}

void Texture::apply(GLuint framgment_id) {
    activate();
    glUniform1i(framgment_id, 0);
    bind();

    // Set our framgment_id sampler to use Texture Unit 0

}


unsigned char* Texture::getDataFromFile(std::string path, GLenum*format, int *width, int *height) {
	int nrComponents;
	printf("String for file: %s\n", path.c_str());
    unsigned char* data = stbi_load(path.c_str(), width, height, &nrComponents, 0);
    
    if (data){       
        switch (nrComponents){
        case 1:
            *format = GL_RED;
            break;
        case 3:
            *format = GL_RGB;
            break;
        case 4:
            *format = GL_RGBA;
            break;
        default:
        	*format = GL_RGB;
        }
    } else {
    	printf("Problem reading data\n");
    }

    return data;

}



Texture* Texture::getCubemapTexture(std::string directory, bool gamma) {

	Texture* t = new Texture(Cube);

	int width, height;
	GLenum format;
	unsigned char*data;


	std::string final_path = "./res/"+directory+"/";
    
	t->bind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    Debug::CheckOpenGLError("Texture::getCubemapTexture");
    
	//load sides
	data = Texture::getDataFromFile(final_path+TOP_TEX,  &format,&width, &height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0,GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	width = 0;
	height = 0;
	data = nullptr;

    data = Texture::getDataFromFile(final_path+BOTTOM_TEX,  &format,&width, &height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0,GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	width = 0;
	height = 0;
	data = nullptr;


	data = Texture::getDataFromFile(final_path+FRONT_TEX , &format, &width, &height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0,GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	width = 0;
	height = 0;
	data = nullptr;

	data = Texture::getDataFromFile( final_path+BACK_TEX, &format,&width, &height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	width = 0;
	height = 0;
	data = nullptr;

	data = Texture::getDataFromFile( final_path+LEFT_TEX, &format,&width, &height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0,GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	width = 0;
	height = 0;
	data = nullptr;


	data = Texture::getDataFromFile(final_path+RIGHT_TEX, &format,&width, &height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0,GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	width = 0;
	height = 0;

	// OpenGL has now copied the data. Free our own version
	

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	
	
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	t->unbind();

	return t;
};

Texture* Texture::fromFile(std::string filename, std::string directory, bool gamma)
{
    if (!directory.empty())
        filename = directory + '/' + filename;
    
    Texture* t = new Texture;

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    
    if (data){
        GLenum format;
        
        switch (nrComponents){
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        }

        t->bind();
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        t->unbind();
    }
    else
        DEBUG(Debug::Error, "Texture failed to load at path: %s\n", filename.c_str());
        
    stbi_image_free(data);

    return t;
}
