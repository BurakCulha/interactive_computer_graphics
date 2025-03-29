#ifndef MY_MESH_HPP
#define MY_MESH_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "shaderProgram.hpp"



using namespace std;

struct Vertex {

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;

};

struct Texture {
    string type;
    // string path;
    GLuint id;
    GLuint textureUnit;
};


class Mesh {
    private:
        GLuint vboId;
        void myObjLoader(const string& path);
        float maxX, minX, maxY, minY, maxZ, minZ;
        void createObjTangents();
        void setupMesh();
        unordered_map<string, GLuint> meshAttribLocMap;
        glm::mat4 modelMatrix;
        static int active_texture_unit_count;
    
    public:
        GLuint vaoId;
        vector<Vertex> vertices;
        vector<Texture> textures;
        // Texture texture;
        Mesh(const string& objPath);
        void draw(ShaderProgram& shader);
        // GLuint getVaoId();
        void myTextureLoader(const string& path, const string& type);
        void centerTheMesh();
        glm::mat4 getModelMatrix();
        void addTexture(Texture& tex);

        static Texture textureCreator(int width, int height, const string& type, GLenum format);

};

#endif