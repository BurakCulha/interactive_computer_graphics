#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

// #include <GL/glew.h>
#include "glad/glad.h"
#include <iostream>
#include <string>
#include <fstream>
#include <glm/glm.hpp>


using namespace std;

class ShaderProgram {



    private:
        void linkProgram();
        bool inUse{false};
        bool isBound();

    public:
        enum shaderTypes{vertexShader, fragmentShader, geometryShader, tesControlShader, tesEvaluationShader};
        void createShader(const string& path, ShaderProgram::shaderTypes type);
        void linkShaderObject();
        GLuint programId;
        GLuint vsId, fsId, gsId, tesId, tcsId;

        ShaderProgram(const string& vsName, const string& fsName);
        void setBool(const string& name, bool value);
        void setFloat(const string& name, float value);
        void setInt(const string& name, int value);
        void setMat4(const string& name, const glm::mat4& val);
        void setMat3(const string& name, const glm::mat3& val);
        void setVec3(const string& name, const glm::vec3& val);
        void setVec2(const string& name, const glm::vec2& val);
        void setVec4(const string& name, const glm::vec4& val);
        void setVec4int(const string& name, const glm::ivec4& val);
        void setVec3int(const string& name, const glm::ivec3& val);
        void setVec2int(const string& name, const glm::ivec2& val);
        void useProgram();
        string getShaderFromFile(string name);

        GLuint getAttribLocation(const string& name);





};


#endif