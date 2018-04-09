#include "light.hpp"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>

#include "common.hpp"
#include "material.hpp"

using namespace std;

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

Light::Light():
	_pos(0,0,0),
	_color(1,1,1)
{

    _shader = Shader::fromFiles( "shaders/vertexshader_light.glsl", "shaders/fragment_light.glsl");
	 Assimp::Importer importer;
	 std::string path = "./res/sphere.dae";
	 std::string directory = path.substr(0, path.find_last_of('/'));
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    if( !scene) {
        DEBUG(Level::ERROR, "[Assimp] Errror while reading: %s\n", importer.GetErrorString());
        _mesh =  nullptr;
    }

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
        _mesh = new Mesh;

        // Fill vertices positions
        std::vector<GLfloat> vertices;
        vertices.reserve(mesh->mNumVertices);
        for(unsigned int i=0; i<mesh->mNumVertices; i++){
            aiVector3D pos = mesh->mVertices[i];
            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);
        }
        _mesh->setVertex(vertices);

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
            _mesh->setUV(uvs);
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
            _mesh->setNormal(normals);
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
            _mesh->setIndice(indices);
        }
       
        if(mesh->mMaterialIndex >= 0)
            _mesh->setMaterial(materials[mesh->mMaterialIndex]);
        //s->addMesh(v);
    }
}