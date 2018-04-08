
#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#define TOP_TEX "top.jpg"
#define BOTTOM_TEX "bottom.jpg"
#define LEFT_TEX "left.jpg"
#define RIGHT_TEX "right.jpg"
#define FRONT_TEX "front.jpg"
#define BACK_TEX "back.jpg"


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
        Texture(bool sky);

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
        void apply(GLuint framgment_id);
        static Texture* getCubemapTexture(std::string directory, bool gamma);
        static Texture* fromFile(std::string filename, std::string directory = "", bool gamma = false);
        //void apply(GLuint framgment_id);
        static unsigned char* getDataFromFile(std::string path, GLenum*format, int *width, int *height);


        static std::map<std::string, Texture*> LOADED;

        bool _isSkybox;
        GLuint _texture_id;
    private:
        
        
        Type _type;
        std::string _path;
};

#endif
