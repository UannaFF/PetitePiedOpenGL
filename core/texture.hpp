
#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h> 

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#define TOP_TEX "top.bmp"
#define BOTTOM_TEX "bottom.bmp"
#define LEFT_TEX "front.bmp"
#define RIGHT_TEX "back.bmp"
#define FRONT_TEX "right.bmp"
#define BACK_TEX "left.bmp"


class Texture {
    public:
        enum Type {Diffuse, Specular, Normal, Height, Cube};
    
        /**!
         * \short Create a texture out of a bitmap buffer
         * \param buffer The bitmap buffer
         * \param width The bitmap width
         * \param height The bitmap height
         */
        Texture(unsigned char* buffer, int width, int height);

        /**!
         * \short Create an empty texture of type \ref Type
         */
        Texture(Type t = Diffuse);
        ~Texture();
        
        inline GLuint id() const { return _texture_id; }
        inline GLuint texId() const {return _id;}
        inline Type type() const { return _type; }
        inline void type(Type t) { _type = t; }
        inline void bind() { glBindTexture((_type == Cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D), _texture_id); }
        inline void unbind() { glBindTexture((_type == Cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D), 0); }
        
        inline void activate() {  glActiveTexture(GL_TEXTURE0 + _id); }
        inline void deactivate() {  glActiveTexture(GL_TEXTURE0); }
        void apply(GLuint framgment_id);
        void deapply(GLuint framgment_id);
        
        static Texture* getCubemapTexture(std::string directory, bool gamma);
        static Texture* fromFile(std::string filename, std::string directory = "", Type t = Type::Diffuse);

        static unsigned char* getDataFromFile(std::string path, GLenum*format, int *width, int *height);


        static std::map<std::string, Texture*> LOADED;
    private:              
        static uint LAST_ID;
        
        uint _id;
        
        GLuint _texture_id;
        Type _type;
        std::string _path;
};

#endif
