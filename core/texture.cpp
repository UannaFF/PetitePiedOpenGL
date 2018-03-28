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

Texture::Texture():
    _type(Diffuse)
{
	glGenTextures(1, &_texture_id);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, _texture_id);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
}


Texture::Texture(Type t):
    Texture()
{
    _type = t;      
}

Texture::Texture(unsigned char* data, int width, int height):
    Texture()
{
	glGenTextures(1, &_texture_id);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, _texture_id);

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

}

Texture::~Texture(){
    glDeleteTextures(1, &_texture_id);
}

//~ Texture* Texture::fromBitmap(std::string imagepath){

	//~ DEBUG(Debug::Info, "Reading image %s\n", imagepath.c_str());

	//~ // Data read from the header of the BMP file
	//~ unsigned char header[54];
	//~ unsigned int dataPos;
	//~ unsigned int imageSize;
	//~ unsigned int width, height;
	//~ // Actual RGB data
	//~ unsigned char * data;

	//~ // Open the file
	//~ std::ifstream BitMapStream(imagepath, ios::in);
	//~ if(!BitMapStream.is_open()){
		//~ DEBUG(Debug::Error, "%s could not be opened\n", imagepath.c_str());
		//~ return nullptr;
	//~ }

	//~ // Read the header, i.e. the 54 first bytes

	//~ // If less than 54 bytes are read, problem
	//~ // A BMP files always begins with "BM"
	//~ // Make sure this is a 24bpp file
	//~ BitMapStream.read((char*)header, 54);
	//~ if ( BitMapStream.eof() || header[0]!='B' || header[1]!='M' || *(int*)&(header[0x1E])!=0 || *(int*)&(header[0x1C])!=24){
        //~ DEBUG(Debug::Error, "Not a correct BMP file\n");
        //~ BitMapStream.close(); 
        //~ return nullptr;
    //~ }

	//~ // Read the information about the image
	//~ dataPos    = *(int*)&(header[0x0A]);
	//~ imageSize  = *(int*)&(header[0x22]);
	//~ width      = *(int*)&(header[0x12]);
	//~ height     = *(int*)&(header[0x16]);

	//~ // Some BMP files are misformatted, guess missing information
	//~ if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	//~ if (dataPos==0)      dataPos=54; // The BMP header is done that way

	//~ // Create a buffer
	//~ data = new unsigned char [imageSize];

	//~ BitMapStream.read((char*)data, imageSize);
	//~ // Handle a truncated file before the actual imageSize
    //~ if ( BitMapStream.fail()){
        //~ DEBUG(Debug::Error, "BMP file is truncated\n");
        //~ BitMapStream.close(); 
        //~ return nullptr;
    //~ }

	//~ // Everything is in memory now, the file can be closed.
    //~ BitMapStream.close(); 

	//~ // Create one OpenGL texture
	//~ return new Texture(data, width, height);
//~ }

// Since GLFW 3, glfwLoadTexture2D() has been removed. You have to use another texture loading library, 
// or do it yourself (just like loadBMP_custom and loadDDS)
//GLuint loadTGA_glfw(const char * imagepath){
//
//	// Create one OpenGL texture
//	GLuint textureID;
//	glGenTextures(1, &textureID);
//
//	// "Bind" the newly created texture : all future texture functions will modify this texture
//	glBindTexture(GL_TEXTURE_2D, textureID);
//
//	// Read the file, call glTexImage2D with the right parameters
//	glfwLoadTexture2D(imagepath, 0);
//
//	// Nice trilinear filtering.
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
//	glGenerateMipmap(GL_TEXTURE_2D);
//
//	// Return the ID of the texture we just created
//	return textureID;
//}




//~ Texture* Texture::fromDDS(std::string imagepath){

	//~ unsigned char header[124];
    
	//~ /* try to open the file */ 
	//~ std::ifstream DDSStream(imagepath, ios::in);
	//~ if(!DDSStream.is_open()){
		//~ DEBUG(Debug::Error, "%s could not be opened\n", imagepath.c_str());
		//~ return nullptr;
	//~ }
   
	//~ /* verify the type of file */ 
	//~ char filecode[4]; 
    //~ DDSStream.read(filecode, 4);
	//~ if (strncmp(filecode, "DDS ", 4) != 0) {
		//~ DEBUG(Debug::Error, "%s is not a DDS file\n", imagepath.c_str()); 
		//~ DDSStream.close(); 
		//~ return nullptr; 
	//~ }
	
	//~ /* get the surface desc */ 
    //~ DDSStream.read((char*)header, 124);

	//~ unsigned int height      = *(unsigned int*)&(header[8 ]);
	//~ unsigned int width	     = *(unsigned int*)&(header[12]);
	//~ unsigned int linearSize	 = *(unsigned int*)&(header[16]);
	//~ unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	//~ unsigned int fourCC      = *(unsigned int*)&(header[80]);

 
	//~ unsigned char * buffer;
	//~ unsigned int bufsize;
	//~ /* how big is it going to be including all mipmaps? */ 
	//~ bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
	//~ buffer = new unsigned char[bufsize]; 
    //~ DDSStream.read((char*)buffer, bufsize);
	//~ /* close the file stream */ 
	//~ DDSStream.close();

	//~ unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
	//~ unsigned int format;
	//~ switch(fourCC) 
	//~ { 
	//~ case FOURCC_DXT1: 
		//~ format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
		//~ break; 
	//~ case FOURCC_DXT3: 
		//~ format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
		//~ break; 
	//~ case FOURCC_DXT5: 
		//~ format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
		//~ break; 
	//~ default: 
		//~ delete [] buffer; 
		//~ return nullptr; 
	//~ }

	//~ // Create one OpenGL texture
	//~ Texture* t = new Texture();
	//~ unsigned int offset = 0;

	//~ /* load the mipmaps */ 
	//~ for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
	//~ { 
		//~ offset += t->addMipMap(buffer + offset, width, height, level, format);; 
		//~ width  /= 2; 
		//~ height /= 2; 

		//~ // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		//~ if(width < 1) width = 1;
		//~ if(height < 1) height = 1;
	//~ } 

    //~ delete [] buffer; 

	//~ return t;
//~ }

//~ void Texture::set(GLuint framgment_id){    
    //~ glActiveTexture(GL_TEXTURE0);
    //~ glBindTexture(GL_TEXTURE_2D, _texture_id);
    //~ // Set our framgment_id sampler to use Texture Unit 0
    //~ glUniform1i(framgment_id, 0);
//~ }

//~ void Texture::draw(){
    //~ glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
    
    //~ // retrieve texture number (the N in diffuse_textureN)
    //~ switch(_type){
    //~ case Diffuse:
        //~ break;
    //~ case Specular:
        //~ break;
    //~ case Normal:
        //~ break;
    //~ case Height:
        //~ break;
    //~ }
    //~ string name = textures[i].type;
    //~ if(name == "texture_diffuse")
        //~ number = std::to_string(diffuseNr++);
    //~ else if(name == "texture_specular")
        //~ number = std::to_string(specularNr++); // transfer unsigned int to stream
    //~ else if(name == "texture_normal")
        //~ number = std::to_string(normalNr++); // transfer unsigned int to stream
     //~ else if(name == "texture_height")
        //~ number = std::to_string(heightNr++); // transfer unsigned int to stream

                                             //~ // now set the sampler to the correct texture unit
    //~ glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
    //~ // and finally bind the texture
    //~ glBindTexture(GL_TEXTURE_2D, textures[i].id);
//~ }

Texture* Texture::fromFile(std::string filename, std::string directory, bool gamma)
{
    if (!directory.empty())
        filename = directory + '/' + filename;
    
    Texture* t = new Texture;

    glGenTextures(1, &(t->_texture_id));

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

        glBindTexture(GL_TEXTURE_2D, t->id());
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
        DEBUG(Debug::Error, "Texture failed to load at path: %s\n", filename.c_str());
        
    stbi_image_free(data);

    return t;
}
