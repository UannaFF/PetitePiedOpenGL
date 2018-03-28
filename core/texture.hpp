
#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

class Texture {
    public:
        enum Type {Diffuse, Specular, Normal, Height};
    
        /**!
         * \short Create a texture out of a bitmap buffer
         * \param buffer The bitmap buffer
         * \param width The bitmap width
         * \param height The bitmap height
         */
        Texture(unsigned char* buffer, int width, int height);
        /**!
         * \short Create an empty texture
         */
        Texture();
        /**!
         * \short Create an empty texture of type \ref Type
         */
        Texture(Type);
        ~Texture();
        
        inline GLuint id() const { return _texture_id; }
        inline Type type() const { return _type; }
        inline void type(Type t) { _type = t; }
        
        //~ static Texture* fromBitmap(std::string);
        //~ static Texture* fromDDS(std::string);
        static Texture* fromFile(std::string filename, std::string directory = "", bool gamma = false);
        
        static std::map<std::string, Texture*> LOADED;
    private:
        GLuint _texture_id;
        Type _type;
        std::string _path;
};

#endif
