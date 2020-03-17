//
// Material class
//
#pragma once

//common
#include<string>
//common

#include <learnopengl/shader_m.h>

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class cMaterial
{
public:

    cMaterial(shader* inShader, vector<Texture> inTextures)
        :Shade(inShader),
        Textures(inTextures)
    {

    }


    vector<Texture> Textures;


    ~cMaterial();
private:
    shader* Shader;
};


