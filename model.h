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
    model(const std::string &path, double scale_ = 1.0, std::shared_ptr<material> mat = make_shared<lambertian>(vec3(0)) )
        : scale(scale_), mtr(mat)
    {
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
        // The model is turned 80 degrees about y; baking that into the vertices once spares
        // a ray transform at every triangle test (same geometry as wrapping each in rotate_).
        double a = radians(80), c = cos(a), s = sin(a);
        auto turn = [&](vec3 p) { p = scale * p; return vec3(c*p.x() + s*p.z(), p.y(), -s*p.x() + c*p.z()); };
        hittable_list triangles;
        for(auto m: meshes){
            for(int i = 0; i<m.indices.size(); i+=3){
                triangles.add(std::make_shared<triangle>(
                    turn(m.vertices[m.indices[i+0]]),
                    turn(m.vertices[m.indices[i+1]]),
                    turn(m.vertices[m.indices[i+2]]),
                    mtr));
            }
        }
        return triangles;
    }
private:
    std::string directory;
    std::vector<mesh> meshes;
    std::shared_ptr<material> mtr;
    double scale;
    
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
