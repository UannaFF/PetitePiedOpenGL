#include "scene.hpp"
#include "models.hpp"
#include "texture.hpp"

#include "common.hpp"

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

	const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_CalcTangentSpace       | 
                                                   aiProcess_Triangulate            |
                                                   aiProcess_SortByPType*/);
	if( !scene) {
		DEBUG(Level::ERROR, "[Assimp] Errror while reading: %s\n", importer.GetErrorString());
		return nullptr;
	}
    
    Scene* s = new Scene;
    
    //~ for (int t = 0; t < scene->mNumTextures; t++){
        //~ unsigned char* buffer = new unsigned char[scene->mTextures[t]->mWidth * scene->mTextures[t]->mHeight * 3];
        //~ for (int i = 0; i < scene->mTextures[t]->mWidth * scene->mTextures[t]->mHeight; i++){
            //~ buffer[i * 3] = scene->mTextures[t]->pcData->r;
            //~ buffer[i * 3 + 1] = scene->mTextures[t]->pcData->g;
            //~ buffer[i * 3 + 2] = scene->mTextures[t]->pcData->b;
        //~ }
        
        //~ s->addTexture(new Texture(buffer, scene->mTextures[t]->mWidth, scene->mTextures[t]->mHeight));
    //~ }   
    
    // http://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/
    std::map<std::string, GLuint> textureIdMap;
    DEBUG(Debug::Info, "Materials: %d\n",scene->mNumMaterials); 
    for (unsigned int m=0; m<scene->mNumMaterials; ++m){
        int texIndex = 0;
        aiString path;  // filename
        aiColor3D color (0.f,0.f,0.f);
        scene->mMaterials[m]->Get(AI_MATKEY_GLOBAL_BACKGROUND_IMAGE, path);

        //~ DEBUG(Debug::Info, "-: %f %f %f\n", color.r, color.b, color.g); 
        DEBUG(Debug::Info, "-: %s\n", path.data); 

 
        aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
        while (texFound == AI_SUCCESS) {
            //fill map with textures, OpenGL image ids set to 0
            textureIdMap[path.data] = 0; 
            DEBUG(Debug::Info, "- %s\n", path.data);
            // more textures?
            texIndex++;
            texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
        }
    }
    
    int numTextures = textureIdMap.size(); 
 
    /* create and fill array with GL texture ids */
    GLuint* textureIds = new GLuint[numTextures];
    glGenTextures(numTextures, textureIds); /* Texture name generation */
 
    /* get iterator */
    std::map<std::string, GLuint>::iterator itr = textureIdMap.begin();
    int i=0;
    for (; itr != textureIdMap.end(); ++i, ++itr)
    {
        //save IL image ID
        std::string filename = (*itr).first;  // get filename
        (*itr).second = textureIds[i];    // save texture id for filename in map
 
        /* Create and load textures to OpenGL */
        printf("Mamene: %s\n", filename.c_str());
        //~ glBindTexture(GL_TEXTURE_2D, textureIds[i]); 
        //~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        //~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //~ glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
            //~ ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
            //~ ilGetData()); 
  
    }
    // end
    
    DEBUG(Debug::Info, "Meshes: %d\n", scene->mNumMeshes);
    for (int m = 0; m < scene->mNumMeshes; m++){    
        const aiMesh* mesh = scene->mMeshes[m];
        VertexArray* v = new VertexArray;

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
        s->addMesh(v);
    }
    
    // Loading materials
    //~ std::vector<Material*> materials;
    //~ textures.reserve(scene->mNumMaterials);
    
    //~ for (int m = 0; m < scene->mNumMaterials; m++){
        
        //~ textures.push_back(new Material());
    //~ }    
    
    // Loading nodes
    DEBUG(Debug::Info, "Root node has %d children\n", scene->mRootNode->mNumChildren);
    std::vector<VertexArray*> _va;
    
    for (int v = 0; v < scene->mRootNode->mNumMeshes; v++)
        _va.push_back(s->getMesh(scene->mRootNode->mMeshes[v]));
        
    Node* _root_node = new Node(scene->mRootNode->mName.data, _va, aiMatrix4x4toglmMat4(scene->mRootNode->mTransformation), s);
    s->_parseNode(_root_node, scene->mRootNode->mChildren, scene->mRootNode->mNumChildren);
    s->setRootNode(_root_node);
	// The "scene" pointer will be deleted automatically by "importer"
	return s;
}
void Scene::render(GLuint model_vert){
    _main_node->draw(model_vert);
}

void Scene::_parseNode(Node* current, aiNode ** children, unsigned int nb_child){
    for (unsigned int c = 0; c < nb_child; c++){
        aiNode* curr_child = children[c];
        std::vector<VertexArray*> _va;
        
        //~ DEBUG(Debug::Info, "Node '%s' has %d meshes\n", curr_child->mName.data, curr_child->mNumMeshes);
        for (int v = 0; v < curr_child->mNumMeshes; v++){
            //~ DEBUG(Debug::Info, "Node '%s' contains %d\n", curr_child->mName.data, curr_child->mMeshes[v]);
            _va.push_back(getMesh(curr_child->mMeshes[v]));
        }
            
        Node* n = new Node(curr_child->mName.data, _va, aiMatrix4x4toglmMat4(curr_child->mTransformation), this, current);
        _parseNode(n, curr_child->mChildren, curr_child->mNumChildren);   
        current->addChild(n);     
    }
}


Node::Node(std::string name, std::vector<VertexArray*> vertexarray, glm::mat4 transformation, Scene* scene, Node* parent):
    _name(name),
    _meshs(vertexarray),
    _transformation(transformation),
    _parent(parent),
    _scene(scene)
{
}

void Node::draw(GLuint model_vert, glm::mat4 localTransform){
    glUniformMatrix4fv(model_vert, 1, GL_FALSE, &(localTransform * _transformation)[0][0]);
    //~ DEBUG(Debug::Info, "Drawing node '%s': %d meshes\n", _name.c_str(), _meshs.size());
    for (VertexArray* mesh: _meshs){
        mesh->bind();
        mesh->draw();
    }
        
    for (Node* child: _children)
        child->draw(model_vert, localTransform * _transformation);
}

void Node::dump(int level){
    std::cout << std::setw(level * 4) << _name << " (" << _meshs.size() << " meshes)" << std::endl;
    for (Node* child: _children){
        std::cout << std::setw(level * 4) << "|--";
        child->dump(level + 1);
    }
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
