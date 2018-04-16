
#ifndef PARTICULES_H
#define PARTICULES_H

#include <vector>
#include <algorithm>
#include <iostream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "models.hpp"

class Shader;

struct Particle {
	glm::vec3 pos, speed;
	unsigned char r,g,b,a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

class ParticuleManager : public Mesh {
    public:
        ParticuleManager(Shader*);
        ~ParticuleManager();
        int update(glm::mat4& view);
        virtual void draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
        
        void dump(int level){
            std::cout << "Particule engine." << std::endl;
        }
        
        inline int findUnusedParticle(){
            return (_particles_container.size() < MAX_PARTICLES ? _particles_container.size() : 0); // All particles are taken, override the first one
        }
    private:    
        static int MAX_PARTICLES;
        
        double _last_time;
        std::vector<Particle> _particles_container;	
        
        GLuint _vertex_array_id;
    
        GLuint _vertex_buffer;
        GLuint _position_buffer;
        GLuint _color_buffer;
        
        GLfloat* _vertex_buffer_data;
        GLfloat* _position_size_data;
        GLubyte* _color_data;
};
#endif
