#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "general.h"
#include "triangle.h"
#include "hittable_list.h"
#include "material.h"
#include <string.h>

struct mesh{
    mesh(std::vector<vec3> &vert, std::vector<int> &ind) {
        vertices =vert;
        indices = ind;
    }
    
    std::vector<vec3> vertices;
    std::vector<int> indices;
};
//basic model loading without materials or textures;
class model{
public:
    model(const std::string &path){
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                std::cerr << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
                return;
            }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }
    hittable_list getHittableList(){
        hittable_list triangles;
        auto shade = vec3(0.8, 0.85, 0.88);
        for(auto m: meshes){
            for(int i = 0; i<m.indices.size(); i+=3){
                triangles.add(std::make_shared<triangle>(7*m.vertices[m.indices[i+0]],
                                                         7*m.vertices[m.indices[i+1]],
                                                         7*m.vertices[m.indices[i+2]],
                                                         std::make_shared<metal>(shade)));
            }
        }
        return triangles;
    }
private:
    std::string directory;
    std::vector<mesh> meshes;
    
    void processNode(aiNode *node, const aiScene *scene){
        for(int i=0;i<node->mNumMeshes;i++){
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }
        for(int i=0;i<node->mNumChildren;i++){
            processNode(node->mChildren[i], scene);
        }
    }
    void processMesh(aiMesh *aimesh, const aiScene *scene){
        
        std::vector<vec3> vertices;
        std::vector<int> indices;
        
        for(int i=0;i<aimesh->mNumVertices;i++){
            
            vec3 pos((double)aimesh->mVertices[i].x,
                     (double)aimesh->mVertices[i].y,
                     -(double)aimesh->mVertices[i].z);
            
            vertices.push_back(pos);
        }
        for(int i = 0; i < aimesh->mNumFaces; i++)
        {
            aiFace face = aimesh->mFaces[i];
            for(int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        meshes.push_back(mesh(vertices, indices));
    }
};

#endif
