#ifndef SCENE_H
#define SCENE_H

class Scene {
    public:
        Scene();
        
        
    private:
        std::vector<Model*> _models;
        Camera* _cameras;
        
};

#endif
