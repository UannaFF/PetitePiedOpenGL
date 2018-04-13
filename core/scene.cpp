#include "scene.hpp"
//#include "models.hpp"
#include "texture.hpp"

#include "common.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "animations.hpp"

#include <map>
#include <iostream>
#include <iomanip>

Scene::Scene():
    _models(),
    _textures(),
    _main_node(nullptr),
    _camera(nullptr),
    _shaders(),
    _animations(),
    _current_animation(),
    _lights()
   // _hasSkybox(false)
{
    //_hasSkybox = false;
    //_skybox_shader = 0;
}

void Scene::displayNodeTree(){ _main_node->dump(); }

Scene* Scene::import(std::string path, Shader* shader){

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    if( !scene) {
        DEBUG(Level::ERROR, "[Assimp] Errror while reading: %s\n", importer.GetErrorString());
        return nullptr;
    }
    
    Scene* s = new Scene;
    s->setShader(shader);
    s->defaultShader()->use();
    
    std::multimap<std::string, Bone*> bones_to_bind;
    
    std::string directory = path.substr(0, path.find_last_of('/'));
    
    DEBUG(Debug::Info, "Materials: %d\n",scene->mNumMaterials); 
    
    std::vector<Material*> materials;
    materials.reserve(scene->mNumMaterials);
    if (scene->HasMaterials()){
        // Loading materials
        
        for (int m = 0; m < scene->mNumMaterials; m++){ 
            const aiMaterial* material = scene->mMaterials[m];
            
            glm::vec3 ambient, diffuse, specular;
            float shininess;
            std::vector<Texture*> textures;
            
            aiColor3D color;
            
            material->Get(AI_MATKEY_COLOR_AMBIENT, color);
            ambient = aiColor3DtoglmVec3(color);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            diffuse = aiColor3DtoglmVec3(color);
            material->Get(AI_MATKEY_COLOR_SPECULAR, color);
            specular = aiColor3DtoglmVec3(color);
            
            std::vector<Texture*> diffuseMaps = Material::loadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::Diffuse, directory);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            std::vector<Texture*> specularMaps = Material::loadMaterialTextures(material, aiTextureType_SPECULAR, Texture::Specular, directory);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<Texture*> normalMaps = Material::loadMaterialTextures(material, aiTextureType_AMBIENT, Texture::Normal, directory);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<Texture*> heightMaps = Material::loadMaterialTextures(material, aiTextureType_HEIGHT, Texture::Height, directory);
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
            
            material->Get(AI_MATKEY_SHININESS, shininess);        
            
            aiShadingMode shadingType;
            material->Get(AI_MATKEY_SHADING_MODEL, shadingType);
            DEBUG(Debug::Info, "material has %d textures\n",textures.size()); 
                   
            Material* mat = new Material(ambient, diffuse, specular, shininess);
            mat->setShadingMode(shadingType);
            mat->setTextures(textures);
            
            materials.push_back(mat);
        }  
    }
    
    DEBUG(Debug::Info, "Meshes: %d\n", scene->mNumMeshes);
    for (int m = 0; m < scene->mNumMeshes; m++){    
        const aiMesh* mesh = scene->mMeshes[m];
        Mesh* v = new Mesh;

        // Fill vertices positions
        std::vector<GLfloat> vertices;
        vertices.reserve(mesh->mNumVertices);
        for(unsigned int i=0; i<mesh->mNumVertices; i++){
            aiVector3D pos = mesh->mVertices[i];
            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);
        }
        v->setVertex(vertices);

        // Fill vertices texture coordinates
        for (int channel = 0; channel < mesh->GetNumUVChannels(); channel++) {
            std::vector<GLfloat> uvs;
            uvs.reserve(mesh->mNumVertices);
            for(unsigned int i=0; i<mesh->mNumVertices; i++){
                aiVector3D UVW = mesh->mTextureCoords[channel][i];
                uvs.push_back(UVW.x);
                uvs.push_back(UVW.y);
            }
            //~ v->addUV(uvs);
            v->setUV(uvs);
        }

        // Fill vertices normals
        if (mesh->HasNormals()){
            std::vector<GLfloat> normals;
            normals.reserve(mesh->mNumVertices);
            for(unsigned int i=0; i<mesh->mNumVertices; i++){
                aiVector3D n = mesh->mNormals[i];
                normals.push_back(n.x);
                normals.push_back(n.y);
                normals.push_back(n.z);
            }
            v->setNormal(normals);
        }


        // Fill face indices
        if (mesh->HasFaces()){
            std::vector<unsigned short> indices;
            indices.reserve(3*mesh->mNumFaces);
            for (unsigned int i=0; i<mesh->mNumFaces; i++){
                // Assume the model has only triangles.
                indices.push_back(mesh->mFaces[i].mIndices[0]);
                indices.push_back(mesh->mFaces[i].mIndices[1]);
                indices.push_back(mesh->mFaces[i].mIndices[2]);
            }
            v->setIndice(indices);
        }
       
        // Fill bones n
        if (mesh->HasBones()){      
            std::vector<Bone*> bones; 
            DEBUG(Debug::Info, "mesh has %d bone\n", mesh->mNumBones);      
            for (unsigned int i=0; i<mesh->mNumBones; i++){
                Bone* b = new Bone(aiMatrix4x4toglmMat4(mesh->mBones[i]->mOffsetMatrix));
                for (int w = 0; w < mesh->mBones[i]->mNumWeights; w++)
                    b->addWeight(mesh->mBones[i]->mWeights[w].mVertexId, mesh->mBones[i]->mWeights[w].mWeight);
                
                bones_to_bind.insert(std::make_pair(mesh->mBones[i]->mName.data, b));
                
                bones.push_back(b);
            }
            v->setBones(bones, shader);
        }
        
        if(mesh->mMaterialIndex >= 0)
            v->setMaterial(materials[mesh->mMaterialIndex]);
        s->addMesh(v);
    }
    
    // Loading nodes
    DEBUG(Debug::Info, "Root node has %d children\n", scene->mRootNode->mNumChildren);
    std::vector<Mesh*> _va;
    
    for (int v = 0; v < scene->mRootNode->mNumMeshes; v++)
        _va.push_back(s->getMesh(scene->mRootNode->mMeshes[v]));
        
    Node* _root_node = new Node(scene->mRootNode->mName.data, _va, aiMatrix4x4toglmMat4(scene->mRootNode->mTransformation), s);
    s->_parseNode(_root_node, scene->mRootNode->mChildren, scene->mRootNode->mNumChildren);
    s->setRootNode(_root_node);
    // The "scene" pointer will be deleted automatically by "importer"
    
    // Parsing animations
    DEBUG(Debug::Info, "Animation: %d\n", scene->mNumAnimations);

    std::vector<Animation*> animations;
    
    for (int a = 0; a < scene->mNumAnimations; a++){ 
        aiAnimation* animation = scene->mAnimations[a];
        Animation* anim = new Animation(animation->mName.data, animation->mDuration, animation->mTicksPerSecond);
        DEBUG(Debug::Info, "-- Animation '%s' has %d channels\n", animation->mName.data, animation->mNumChannels);
        
        for (int k = 0; k < animation->mNumChannels; k++){ 
            aiNodeAnim* channel = animation->mChannels[k];
            
            DEBUG(Debug::Info, "Bone for the node '%s'...\n", channel->mNodeName.data);
                
            Node* relatedNode = s->findNode(channel->mNodeName.data);
            if (!relatedNode)
                throw new SceneException(std::string(channel->mNodeName.data) + " not found in the nodes hierachy");
                
                
            std::vector<Bone*> relatedBones;
            
            std::multimap<std::string, Bone*>::iterator it;
            while ((it = bones_to_bind.find(channel->mNodeName.data)) != bones_to_bind.end()){
                relatedBones.push_back(it->second);
                bones_to_bind.erase (it);
            }
            
                              
            if (relatedBones.empty())
                throw new SceneException(std::string(channel->mNodeName.data) + " has no bones");
            else
                DEBUG(Debug::Info, "%s has %d bones\n", channel->mNodeName.data, relatedBones.size());
                
                
            Channel* c = new Channel(relatedNode, relatedBones);
                                   
            for (int j = 0; j < channel->mNumPositionKeys; j++)                    
                c->addKey(channel->mPositionKeys[j].mTime, new PositionKey(aiVector3DtoglmVec3(channel->mPositionKeys[j].mValue)));

            for (int j = 0; j < channel->mNumRotationKeys; j++)               
                c->addKey(channel->mRotationKeys[j].mTime, new RotationKey(Quaternion::fromAi(channel->mRotationKeys[j].mValue)));  
            
            // Not used yet
            //~ for (int j = 0; j < keyframe->mNumScalingKeys; j++)
                //~ c->addFrame(channel->mScalingKeys[j].mTime, 
                            //~ ScalingFrame(aiVector3DtoglmVec3(channel->mScalingKeys[j].mValue)));               
            anim->addChannel(c);
        }
        animations.push_back(anim);            
    }
    s->setAnimations(animations);
    
    DEBUG(Debug::Info, "\nScene loaded\n");
    
    return s;
}
void Scene::render(){
   // printf("Llego al render\n");
    glm::mat4 mat = glm::mat4(1.f); //this changes the state somewhow


    for(Light* light : _lights) {
        light->draw(mat);
    }

    //Draw skybox
    if(_hasSkybox) {
        _skybox->shader()->use();
        _skybox->bind();
        _skybox->texture()->apply(_skybox->shader()->getUniformLocation("cube_texture"));
        
        _skybox->shader()->setMat4("model", mat);
        _skybox->draw(_skybox->shader());
    }
    

    defaultShader()->use();
    defaultShader()->setMat4("model", mat);
    
    glm::mat4 bone(0.592056, -0.750088, -0.15035, -0.0665687,
      0.576882, 0.622695, -0.452484, -0.0276781,
      0.435611, 0.271241, 0.803574, -0.640044,
      5.05164e-38, -0.0995924, 3.02157e-39, 1);
    defaultShader()->setMat4("gBones[1]", bone);
    
    process(glfwGetTime());
    
    _main_node->draw();
}

void Scene::playAnimation( int anim){
    Animation* a = _animations[anim];
        
    if (_current_animation.find(a) == _current_animation.end()){
        DEBUG(Debug::Info, "Playing animation %s\n", a->name().c_str());
        _current_animation.insert(a);        
    }
}

void Scene::addMesh(Mesh* m){
    _models.push_back(m);
}

void Scene::setSkybox(std::string folder, std::string vertex_name, std::string fragment_name) {
    _hasSkybox = true;
    //_skybox_shader = Shader::fromFiles(vertex_name, fragment_name);

   // _skybox_shader->use();
    _skybox = new Skybox();
    _skybox->setShader(Shader::fromFiles(vertex_name, fragment_name));
    _skybox->shader()->use();

    _skybox->setEverything();

    _skybox->setTexture(Texture::getCubemapTexture(folder, false));
    //_skybox_texture = Texture::getCubemapTexture(folder, false);
}

Node* Scene::findNode(std::string n) const {
    return _main_node->find(n);
}

void Scene::_parseNode(Node* current, aiNode ** children, unsigned int nb_child){
    for (unsigned int c = 0; c < nb_child; c++){
        aiNode* curr_child = children[c];
        std::vector<Mesh*> _va;
        
        DEBUG(Debug::Info, "Node '%s' has %d meshes\n", curr_child->mName.data, curr_child->mNumMeshes);
        for (int v = 0; v < curr_child->mNumMeshes; v++){
            //~ DEBUG(Debug::Info, "Node '%s' contains %d\n", curr_child->mName.data, curr_child->mMeshes[v]);
            _va.push_back(getMesh(curr_child->mMeshes[v]));
        }
            
        Node* n = new Node(curr_child->mName.data, _va, aiMatrix4x4toglmMat4(curr_child->mTransformation), this, current);
        _parseNode(n, curr_child->mChildren, curr_child->mNumChildren);   
        current->addChild(curr_child->mName.data, n);     
    }
}


Node::Node(std::string name, std::vector<Mesh*> m, glm::mat4 transformation, Scene* scene, Node* parent):
    _name(name),
    _meshs(m),
    _transformation(transformation),
    _parent(parent),
    _scene(scene)
{
}

void Node::dump(int level){
    std::cout << std::setw(level * 4) << _name << std::endl;
    for (auto child: _children){
        std::cout << std::setw(level * 4) << "|--";
        child.second->dump(level + 1);
    }
}

Node* Node::find(std::string n){
    if (!_name.compare(n))
        return this;
        
    for (auto child: _children){
        Node* f = child.second->find(n);
        if (f) return f;
    }
    
    return nullptr;
}


void Node::draw(glm::mat4 localTransform){
    //~ glm::mat4 t = applyTransformation(scene()->defaultShader()->getUniformLocation("model"), localTransform);
    
    //~ scene()->defaultShader()->use();
    
    for (Mesh* _mesh: _meshs){
        _mesh->bind();
        _mesh->draw(scene()->defaultShader());
    }
        
    for (auto child: _children)
        child.second->draw(localTransform * _transformation);
}



void Scene::process(float timeInSeconds){
    
    defaultShader()->use();
    for (Animation* a: _current_animation){
        glm::mat4 t(1.f);

        a->applyBones(fmod(a->timeInTick(timeInSeconds), a->duration()), this);
    }
}
