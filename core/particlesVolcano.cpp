
#include "particlesVolcano.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;


#include "shader.hpp"

// CPU representation of a particle
int ParticuleManager::MAX_PARTICLES = 10000;

ParticuleManager::ParticuleManager(Shader* s):
    Mesh(s, nullptr),
    _last_time(0),
    _particles_container(),
    _position_size_data(new GLfloat[MAX_PARTICLES * 4]),
    _color_data(new GLubyte[MAX_PARTICLES * 4]),
    _vertex_buffer_data(new GLfloat[12]{
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	})
{
    _particles_container.reserve(MAX_PARTICLES);
	for(int i = 0; i < MAX_PARTICLES; i++){
		Particle p;
        p.life = -2.0f;
		p.cameradistance = -3.0f;        
        _particles_container.push_back(std::move(p));
	}
    
    _shader->use();
    
    glGenVertexArrays(1, &_vertex_array_id);
    
    glBindVertexArray(_vertex_array_id);    
    
	glGenBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, _vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	glGenBuffers(1, &_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _position_buffer);
	// Initialize with empty (nullptr) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	glGenBuffers(1, &_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _color_buffer);
	// Initialize with empty (nullptr) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);
    glBindVertexArray(0); 

    // Use our shader
    _shader->deuse();
    
}

int ParticuleManager::update(glm::mat4& view){
    double currentTime = glfwGetTime();
    double delta = currentTime - _last_time;
    _last_time = currentTime;

    // Generate 10 new particule each millisecond,
    // but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
    // newparticles will be huge and the next frame even longer.
    int newparticles = (int)(delta*1000.0);
    if (newparticles > (int)(0.016f*1000.0))
        newparticles = (int)(0.016f*1000.0);
        
    if (MAX_PARTICLES - _particles_container.size() < newparticles)
        newparticles = MAX_PARTICLES - _particles_container.size();
        
        
    for (auto particule = _particles_container.begin(); particule != _particles_container.end();){
        if (particule->life < 0)
            particule = _particles_container.erase(particule);
        else
            ++particule;
    }

    for(int i=0; i<newparticles; i++){
        Particle p;
        p.life = 7.0f; // This particle will live 5 seconds.
        p.pos = glm::vec3(-20.0f,0,0);

        float spread = 1.5f;
        glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
        glm::vec3 randomdir = glm::vec3(
            (rand()%2000 - 1000.0f)/1000.0f,
            (rand()%2000 - 1000.0f)/1000.0f,
            (rand()%2000 - 1000.0f)/1000.0f
        );

        p.speed = maindir + randomdir*spread;

        p.r = (160)+ rand() % 56;
        p.g = rand() % 64;
        p.b = 0;
        p.a = 128 + (rand() % 128);

        p.size = (rand()%1000)/2000.0f + 0.1f;
        _particles_container.push_back(std::move(p));
    }

    // Simulate all particles
    int nb_particles = 0;
    glm::vec3 CameraPosition(glm::inverse(view)[3].x, glm::inverse(view)[3].y, glm::inverse(view)[3].z);
    for (auto particule = _particles_container.begin(); particule != _particles_container.end(); ++particule){
        Particle& p = *particule;
        
        if(p.life > 0.0f){

            // Decrease life
            p.life -= delta;
            if (p.life > 0.0f){

                // Simulate simple physics : gravity only, no collisions
                float x=(rand() % 20)-10;
                p.speed += glm::vec3(x,-9.81f, 0) * (float)delta * 0.5f;
                p.pos += p.speed * (float)delta;
                p.cameradistance = glm::length2( p.pos - CameraPosition );
                p.pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

                // Fill the GPU buffer
                _position_size_data[4 * nb_particles + 0] = p.pos.x;
                _position_size_data[4 * nb_particles + 1] = p.pos.y;
                _position_size_data[4 * nb_particles + 2] = p.pos.z;

                _position_size_data[4 * nb_particles + 3] = p.size;

                _color_data[4 * nb_particles + 0] = p.r;
                _color_data[4 * nb_particles + 1] = p.g;
                _color_data[4 * nb_particles + 2] = p.b;
                _color_data[4 * nb_particles + 3] = p.a;
                nb_particles++;

            } else{
                // Particles that just died will be put at the end of the buffer in SortParticles();
                p.cameradistance = -5.0f;
            }
        }
    }
    return nb_particles;
}

void ParticuleManager::draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model){
    _shader->use();
    int nb_particles = update(view);
    
    _shader->setVec3("CameraRight_worldspace", view[0][0], view[1][0], view[2][0]);
    _shader->setVec3("CameraUp_worldspace", view[0][1], view[1][1], view[2][1]);
    _shader->setMat4("P", projection);
    _shader->setMat4("V", view);
    _shader->setMat4("M", model, GL_TRUE);
    
    glBindVertexArray(_vertex_array_id);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, nb_particles * sizeof(GLfloat) * 4, _position_size_data);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, _color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, nb_particles * sizeof(GLubyte) * 4, _color_data);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);

    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad   
    
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, nb_particles); 
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);   
    _shader->deuse();
    
}

ParticuleManager::~ParticuleManager(){
    
	glDeleteBuffers(1, &_color_buffer);
	glDeleteBuffers(1, &_position_buffer);
	glDeleteBuffers(1, &_vertex_buffer);
    
	glDeleteVertexArrays(1, &_vertex_array_id);
    
    delete _shader;
    delete _vertex_buffer_data;
    delete _position_size_data;
    delete _color_data;
}
