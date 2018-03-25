
#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

class Texture {
    public:
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
        ~Texture();
        
        /**!
         * \param buffer The byte buffer of the MipMap
         * \param width The MipMap width
         * \param height The MipMap height
         * 
         * \return the size in byte written
         */
        unsigned int addMipMap(unsigned char* buffer, unsigned int width, unsigned int height, unsigned int level, unsigned int format);
        void set(GLuint framgment_id);
        
        inline GLuint id() const { return _texture_id; }
        
        static Texture* fromBitmap(std::string);
        static Texture* fromDDS(std::string);
        
    private:
        GLuint _texture_id;
};

#endif
