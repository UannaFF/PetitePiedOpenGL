#include "scene.hpp"
//#include "models.hpp"
#include "texture.hpp"

#include "common.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "animations.hpp"
#include "camera.hpp"

#include <map>
#include <iostream>
#include <iomanip>


Scene::Scene():
    _models(),
    _textures(),
    _main_node(nullptr),
    _cameras(),
    _active_camera(nullptr),
    _shaders(),
    _animations(),
    _current_animation(),
    _skybox(nullptr)
{
}

void Scene::displayNodeTree(){ _main_node->dump(); }

Scene* Scene::import(std::string path, Shader* shader){
    
    Assimp::Importer importer;

    //~ const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights);
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);
    if( !scene) {
        DEBUG(Debug::Error, "[Assimp] Errror while reading: %s\n", importer.GetErrorString());
        return nullptr;
    }
    
    Scene* s = new Scene;
    s->setShader(shader);
    shader->name("default_material");
    
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
            
            DEBUG(Debug::Info, "material has %d textures\n",textures.size()); 
                   
            Material* mat = new Material(ambient, diffuse, specular, shininess);
            mat->setTextures(textures);
            
            materials.push_back(mat);
        }  
    }
    
    DEBUG(Debug::Info, "Meshes: %d\n", scene->mNumMeshes);
    for (int m = 0; m < scene->mNumMeshes; m++){    
        const aiMesh* mesh = scene->mMeshes[m];
        VertexArray* v = new VertexArray;

        // Fill vertices positions
        std::vector<GLfloat> vertices;
        vertices.reserve(mesh->mNumVertices * 3);
        for(unsigned int i=0; i<mesh->mNumVertices; i++){
            aiVector3D pos = mesh->mVertices[i];
            vertices.push_back(pos.x);
            vertices.push_back(-pos.z);
            vertices.push_back(pos.y);
        }
        v->setVertex(vertices);

        // Fill vertices texture coordinates
        std::vector<GLfloat> uvsd;
        for (int channel = 0; channel < mesh->GetNumUVChannels(); channel++) {
            std::vector<GLfloat> uvs;
            uvs.reserve(mesh->mNumVertices * 3);
            for(unsigned int i=0; i<mesh->mNumVertices; i++){
                aiVector3D UVW = mesh->mTextureCoords[channel][i];
                uvs.push_back(UVW.x);
                uvs.push_back(UVW.y);
            }
            //~ v->addUV(uvs);
            v->setUV(uvs);
            uvsd = uvs;
        }

        // Fill vertices normals
        if (mesh->HasNormals()){
            std::vector<GLfloat> normals;
            normals.reserve(mesh->mNumVertices * 3);
            for(unsigned int i=0; i<mesh->mNumVertices; i++){
                aiVector3D n = mesh->mNormals[i];
                normals.push_back(n.x);
                normals.push_back(-n.z);
                normals.push_back(n.y);
            }
            v->setNormal(normals);
            
            //If scene has normals and uvs and vertices create tangent and bitangent
            //v->computeTangentBasis(vertices, uvsd, normals);
        }
        
        if(mesh->HasTangentsAndBitangents()) {
            std::vector<GLfloat> tangents;
            std::vector<GLfloat> bitangents;
            tangents.reserve(mesh->mNumVertices);
            bitangents.reserve(mesh->mNumVertices);
            for(unsigned int i=0; i<mesh->mNumVertices; i++){
                aiVector3D t = mesh->mTangents[i];
                aiVector3D b = mesh->mBitangents[i];
                tangents.push_back(t.x);
                tangents.push_back(t.y);
                tangents.push_back(t.z);
                
                bitangents.push_back(b.x);
                bitangents.push_back(b.y);
                bitangents.push_back(b.z);
            }
            
            v->setTangents(tangents);
            v->setBitangents(bitangents);
        }

        // Fill face indices
        if (mesh->HasFaces()){
            std::vector<unsigned short> indices;
            indices.reserve(3 * mesh->mNumFaces);
            // Assume the model has only triangles.
            if (mesh->mFaces->mNumIndices != 3)
                throw new SceneException("wierd number of indices to a face: " + std::to_string(mesh->mFaces->mNumIndices));
                
            for (unsigned int i=0; i<mesh->mNumFaces; i++){  
                //~ DEBUG(Debug::Info, "%d %d %d\n", mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]);

                indices.push_back(mesh->mFaces[i].mIndices[0]);
                indices.push_back(mesh->mFaces[i].mIndices[1]);
                indices.push_back(mesh->mFaces[i].mIndices[2]);
            }
            DEBUG(Debug::Info, "Mesh has %d primitive\n", mesh->mPrimitiveTypes); 
            v->setIndice(indices);
            
        }
       
        // Fill bones n
        std::vector<Bone*> bones; 
        bones.reserve(mesh->mNumBones);
        if (mesh->HasBones()){      
            DEBUG(Debug::Info, "Mesh has %d bones\n", mesh->mNumBones);      
            for (unsigned int i=0; i<mesh->mNumBones; i++){
                Bone* b = new Bone(aiMatrix4x4toglmMat4(mesh->mBones[i]->mOffsetMatrix));
                for (int w = 0; w < mesh->mBones[i]->mNumWeights; w++)
                    b->addWeight(mesh->mBones[i]->mWeights[w].mVertexId, mesh->mBones[i]->mWeights[w].mWeight);
                
                bones_to_bind.insert(std::make_pair(mesh->mBones[i]->mName.data, b));
                
                bones.push_back(b);
            }
        }
        v->setBones(bones, shader);
        
        Mesh* _m = new Mesh(shader, v, bones);
        if(mesh->mMaterialIndex >= 0)
            _m->setMaterial(materials[mesh->mMaterialIndex]);
        s->addMesh(_m);
    }
    
    // Loading nodes
    DEBUG(Debug::Info, "Root node has %d children\n", scene->mRootNode->mNumChildren);
    std::vector<Mesh*> _va;
    
    for (int v = 0; v < scene->mRootNode->mNumMeshes; v++)
        _va.push_back(s->getMesh(scene->mRootNode->mMeshes[v]));
        
    Node* _root_node = new Node(scene->mRootNode->mName.data, aiMatrix4x4toglmMat4(scene->mRootNode->mTransformation), s);
    s->_parseNode(_root_node, scene->mRootNode->mChildren, scene->mRootNode->mNumChildren);
    s->setRootNode(_root_node);
    // The "scene" pointer will be deleted automatically by "importer"
    
    for (std::pair<std::string, Bone*> p: bones_to_bind){
        Node* relatedNode = s->findNode(p.first);
        if (relatedNode)
            p.second->attach(relatedNode);
        else
            throw new SceneException("Bones '"+p.first+"' doesn't refer to any node.");
    }
    
    // Parsing animations
    DEBUG(Debug::Info, "Animation: %d\n", scene->mNumAnimations);

    std::vector<Animation*> animations;
    
    for (int a = 0; a < scene->mNumAnimations; a++){ 
        aiAnimation* animation = scene->mAnimations[a];
        Animation* anim = new Animation(animation->mName.data, animation->mDuration, animation->mTicksPerSecond);
        DEBUG(Debug::Info, "-- Animation '%s' has %d channels\n", animation->mName.data, animation->mNumChannels);
        
        for (int k = 0; k < animation->mNumChannels; k++){ 
            aiNodeAnim* channel = animation->mChannels[k];
                
            Node* relatedNode = s->findNode(channel->mNodeName.data);
            if (!relatedNode)
                throw new SceneException(std::string(channel->mNodeName.data) + " not found in the nodes hierachy");
                
                
            std::vector<Bone*> relatedBones; 
        
            for (std::pair<std::string, Drawable*> child: relatedNode->children())
                if (dynamic_cast<Mesh*>(child.second))
                    relatedBones.insert(relatedBones.end(), ((Mesh*)child.second)->bones().cbegin(), ((Mesh*)child.second)->bones().cend());
            
            std::map<std::string, Bone*>::iterator it;
            

            while ((it = bones_to_bind.find(channel->mNodeName.data)) != bones_to_bind.end()){
                relatedBones.push_back(it->second);
                bones_to_bind.erase (it);
            }
            
                              
            if (relatedBones.empty())
                continue;
                //~ throw new SceneException(std::string(channel->mNodeName.data) + " has no bones");
                
                
            Channel* c = new Channel(relatedNode, relatedBones);
                                   
            for (int j = 0; j < channel->mNumPositionKeys; j++)                    
                c->addKey(channel->mPositionKeys[j].mTime, new PositionKey(aiVector3DtoglmVec3(channel->mPositionKeys[j].mValue)));

            for (int j = 0; j < channel->mNumRotationKeys; j++){             
                c->addKey(channel->mRotationKeys[j].mTime, new RotationKey(aiQuattoglmQuat(channel->mRotationKeys[j].mValue)));  
            }
            
            // Not used yet
            //~ for (int j = 0; j < keyframe->mNumScalingKeys; j++)
                //~ c->addFrame(channel->mScalingKeys[j].mTime, 
                            //~ ScalingFrame(aiVector3DtoglmVec3(channel->mScalingKeys[j].mValue)));               
            anim->addChannel(c);
        }
        animations.push_back(anim);            
    }
    s->setAnimations(animations);
    
    
    //~ for (int b = 0; b < 100; b++)
        //~ shader->setMat4("gBones[" + std::to_string(b) + "]", glm::mat4(1.f));
    
    DEBUG(Debug::Info, "\nScene loaded\n");
    
    return s;
}
void Scene::render(){
        
    if (!_active_camera)
        throw new SceneException("No camera selected for rendering.");

    if(_light) {
        _light->setPos(glm::vec3(fmod(glfwGetTime(),20), 10.0, 1.0));
        _light->bind(defaultShader());
    }

    //Draw skybox
    if(_skybox)       
        _skybox->draw(_active_camera->projectionMatrix(), _active_camera->viewMatrix(), glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f)));
    
    process(glfwGetTime());
    
    _main_node->draw(_active_camera->projectionMatrix(), _active_camera->viewMatrix(), glm::mat4(1.f));
}

void Scene::playAnimation( int anim){
    if (_animations.size() <= anim){
        DEBUG(Debug::Warning, "No animation found. Skipping\n");
        return;
    }
    
    Animation* a = _animations[anim];
        
    if (a && _current_animation.find(a) == _current_animation.end()){
        DEBUG(Debug::Info, "Playing animation %s\n", a->name().c_str());
        _current_animation.insert(a);        
    }
}

void Scene::addMesh(Mesh* m){
    _models.push_back(m);
}

void Scene::setSkybox(std::string folder, std::string vertex_name, std::string fragment_name) {

    _skybox = new Skybox(Shader::fromFiles(vertex_name, fragment_name));
    
    Material* skybox_mat = new Material();
    skybox_mat->addTexture(Texture::getCubemapTexture(folder, false));
    _skybox->setMaterial(skybox_mat);
}

Node* Scene::findNode(std::string n) const {
    return _main_node->find(n);
}

void Scene::_parseNode(Node* current, aiNode ** children, unsigned int nb_child){
    for (unsigned int c = 0; c < nb_child; c++){
        aiNode* curr_child = children[c];        
            
        Node* n = new Node(curr_child->mName.data, aiMatrix4x4toglmMat4(curr_child->mTransformation), this, current);
        //~ DEBUG(Debug::Info, "Node '%s' has %d meshes\n", curr_child->mName.data, curr_child->mNumMeshes);
        for (int v = 0; v < curr_child->mNumMeshes; v++){
            //~ DEBUG(Debug::Info, "Node '%s' contains Mesh#%d\n", curr_child->mName.data, curr_child->mMeshes[v]);
            n->addChild("", getMesh(curr_child->mMeshes[v]));
        }
        _parseNode(n, curr_child->mChildren, curr_child->mNumChildren);   
        current->addChild(curr_child->mName.data, n);     
    }
}


Node::Node(std::string name, glm::mat4 transformation, Scene* scene, Node* parent):
    _name(name),
    _transformation(transformation),
    _world_transformation(1.f),
    _parent(parent),
    _scene(scene)
{
}

void Node::dump(int level){
    std::cout << _name << std::endl;
    for (auto child: _children){
        std::cout << std::setw(level * 4) << "|--";
        child.second->dump(level + 1);
    }
}

Node* Node::find(std::string n){
    if (!_name.compare(n))
        return this;
        
    for (auto child: _children){
        if (dynamic_cast<Node*>(child.second)){
            Node* f = ((Node*)child.second)->find(n);
            if (f) return f;
        }
    }
    
    return nullptr;
}

glm::mat4 aiMatrix4x4toglmMat4(aiMatrix4x4t<float>& ai_mat){
    glm::mat4 mat;
    
    mat[0].x = ai_mat.a1;
    mat[0].y = ai_mat.a2;
    mat[0].z = ai_mat.a3;
    mat[0].w = ai_mat.a4;
    mat[1].x = ai_mat.b1;
    mat[1].y = ai_mat.b2;
    mat[1].z = ai_mat.b3;
    mat[1].w = ai_mat.b4;
    mat[2].x = ai_mat.c1;
    mat[2].y = ai_mat.c2;
    mat[2].z = ai_mat.c3;
    mat[2].w = ai_mat.c4;
    mat[3].x = ai_mat.d1;
    mat[3].y = ai_mat.d2;
    mat[3].z = ai_mat.d3;
    mat[3].w = ai_mat.d4;
    
    return mat;
}


glm::vec3 aiColor3DtoglmVec3(aiColor3D& ai_col){
    
    glm::vec3 col;
    
    col.x = ai_col.r;
    col.y = ai_col.g;
    col.z = ai_col.b;
    
    return col;
}

glm::vec3 aiVector3DtoglmVec3(aiVector3D& ai_col){
    
    glm::vec3 col;
    
    col.x = ai_col.x;
    col.y = ai_col.y;
    col.z = ai_col.z;
    
    return col;
}

glm::quat aiQuattoglmQuat(aiQuaternion& ai_q){
    
    glm::quat q;
    
    q.x = ai_q.x;
    q.y = ai_q.y;
    q.z = ai_q.z;
    q.w = ai_q.w;
    
    return q;
}

void Node::draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model){
    _world_transformation = model * transformation();
    
    //~ std::cout << _name << _transformation;
    
    //~ if (_scene->defaultShader(Scene::BonesDebugShader)){
        //~ VertexArray* va = new VertexArray;
        //~ glm::vec4 p = (_world_transformation * glm::vec4(0.f, 2.f, 0.f, 0));
        //~ va->setVertex({0., 0., 0., p.x, p.y, p.z});
        //~ Mesh* m = new Mesh(_scene->defaultShader(Scene::BonesDebugShader), va);
        //~ m->draw(projection, view, model);
        //~ delete m;
    //~ }
        
    for (auto child: _children)
        child.second->draw(projection, view, _world_transformation);
}



void Scene::process(float timeInSeconds){
    
    //~ defaultShader()->use();
    for (Animation* a: _current_animation){
        a->applyBones(fmod(a->timeInTick(timeInSeconds), a->duration()), this);
    }
}
