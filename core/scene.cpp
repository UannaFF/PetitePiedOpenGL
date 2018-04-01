#include "scene.hpp"
#include "models.hpp"
#include "texture.hpp"

#include "common.hpp"
#include "shader.hpp"
#include "light.hpp"
#include "material.hpp"
#include "animations.hpp"

#include <map>
#include <iostream>
#include <iomanip>

Scene::Scene():
    _models(),
    _textures(),
    _main_node(nullptr),
    _camera(nullptr)
{
}

void Scene::displayNodeTree(){ _main_node->dump(); }

Scene* Scene::fromOBJ(std::string path){

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if( !scene) {
		DEBUG(Level::ERROR, "[Assimp] Errror while reading: %s\n", importer.GetErrorString());
		return nullptr;
	}
    
    Scene* s = new Scene;
    
    std::string directory = path.substr(0, path.find_last_of('/'));
    
    DEBUG(Debug::Info, "Materials: %d\n",scene->mNumMaterials); 
    
    if (scene->HasMaterials()){
        // Loading materials
        std::vector<Material*> materials;
        materials.reserve(scene->mNumMaterials);
        
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
       
        // Fill face indices 
        if (mesh->HasBones()){            
            
            std::map<std::string, VertexWeight> bones;
            bones.reserve(mesh->mNumBones);
            
            for (unsigned int i=0; i<mesh->mNumBones; i++){
                aiBone* b = mesh->mBones[i];
                bones.insert(std::pair<std::string, VertexBoneData>(b->mName.data, {b->mNumWeights, b->, b->}));
            }
            v->setBones(bones);
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
    if (scene->HasAnimations()){
        // Loading materials
        
        /*
         * **AssImp     -> Our          <- ThinMatrix's implem**
         * 
         * aiAnimation  -> Animation    <- Animation
         * aiNodeAnim   -> KeyFrame         <- KeyFrame
         * N/D          -> AnimatedMesh <- AnimatedModel
         * 
         */
        std::vector<Animation*> animations;
        animations.reserve(scene->mNumAnimations);
        
        for (int a = 0; a < scene->mNumAnimations; a++){ 
            aiAnimation* animation = scene->mAnimations[a];
            Animation* a = new Animation(animation->mDuration, animation->mTicksPerSecond);
            
            std::map<double, KeyFrame*> keyframes;
            for (int k = 0; k < animation->mNumChannels; k++){ 
                aiNodeAnim* channel = animation->mChannels[k];
                
                if (!relatedNode)
                    throw new SceneException(std::string(channel->mNodeName.data) + " not found in the nodes hierachy");
                NodeAnimator* c = new NodeAnimator(relatedNode);
                                       
                for (int j = 0; j < channel->mNumPositionKeys; j++)                    
                    c->addFrame(channel->mPositionKeys[j].mTime, aiVector3DtoglmVec3(channel->mPositionKeys[j].mValue));

                for (int j = 0; j < channel->mNumRotationKeys; j++)               
                    c->addFrame(channel->mRotationKeys[j].mTime, Quaternion::fromAi(channel->mRotationKeys[j].mValue));  
                
                // Not used yet
                //~ for (int j = 0; j < keyframe->mNumScalingKeys; j++)
                    //~ c->addFrame(channel->mScalingKeys[j].mTime, 
                                //~ ScalingFrame(aiVector3DtoglmVec3(channel->mScalingKeys[j].mValue)));               
                a->addChannel(c);
            }
            animations.insert(std::pair<std::string, Animation*>(animation->mName, a));            
        }
        s->setAnimations(animations);
    }
    
	return s;
}
void Scene::render(){
    _main_node->draw();
}

void Scene::addMesh(Mesh* m){
    _models.push_back(m);
}

Node* Scene::findNode(std::string n) const {
    _main_node->find(n);
}

void Scene::_parseNode(Node* current, aiNode ** children, unsigned int nb_child){
    for (unsigned int c = 0; c < nb_child; c++){
        aiNode* curr_child = children[c];
        Mesh* m;
        
        DEBUG(Debug::Info, "Node '%s' has %d meshes\n", curr_child->mName.data, curr_child->mNumMeshes);
        for (int v = 0; v < curr_child->mNumMeshes; v++){
            //~ DEBUG(Debug::Info, "Node '%s' contains %d\n", curr_child->mName.data, curr_child->mMeshes[v]);
            m = getMesh(curr_child->mMeshes[v]);
        }
            
        Node* n = new Node(curr_child->mName.data, m, aiMatrix4x4toglmMat4(curr_child->mTransformation), this, current);
        _parseNode(n, curr_child->mChildren, curr_child->mNumChildren);   
        current->addChild(curr_child->mName.data, n);     
    }
}


Node::Node(std::string name, Mesh* m, glm::mat4 transformation, Scene* scene, Node* parent):
    _name(name),
    _mesh(m),
    _transformation(transformation),
    _parent(parent),
    _scene(scene)
{
}

void Node::dump(int level){
    std::cout << std::setw(level * 4) << _name << std::endl;
    for (Node* child: _children){
        std::cout << std::setw(level * 4) << "|--";
        child->dump(level + 1);
    }
}

void Node::find(std::string n){
    if (_name == n)
        return this;
        
    auto it = _children.find(n);
    if (it == _children.end()) return nullptr;
    
    return it->second;
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

void Node::draw(glm::mat4 localTransform){
    glm::mat4 t = applyTransformation(scene()->defaultShader()->getUniformLocation("model"), localTransform);
    
    scene()->defaultShader()->use();
    
    if (_mesh){
        _mesh->bind();
        _mesh->draw(scene()->defaultShader());
    }
        
    for (Node* child: _children)
        child->draw(localTransform * _transformation);
}
