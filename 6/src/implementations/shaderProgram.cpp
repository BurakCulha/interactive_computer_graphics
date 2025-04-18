#include "shaderProgram.hpp"


ShaderProgram::ShaderProgram(const string& vsPath, const string& fsPath) {

    this->programId = glCreateProgram();

    this->createShader(vsPath, shaderTypes::vertexShader);
    this->createShader(fsPath, shaderTypes::fragmentShader);
    this->linkProgram();

}


void ShaderProgram::createShader(const string& path, ShaderProgram::shaderTypes type) {

    string sourceCode = this->getShaderFromFile(path);
    char* sourcePtr = &sourceCode[0];
    int success{};
    char infolog[1024]{};
    GLuint shaderTypeInProcess{};

    if(type == vertexShader) {
        this->vsId = glCreateShader(GL_VERTEX_SHADER);
        shaderTypeInProcess = this->vsId;
    } else if(type == fragmentShader) {
        this->fsId = glCreateShader(GL_FRAGMENT_SHADER);
        shaderTypeInProcess = this->fsId;
    } else if(type == geometryShader) {
        this->gsId = glCreateShader(GL_GEOMETRY_SHADER);
        shaderTypeInProcess = this->gsId;
    } else if(type == tesControlShader) {
        this->tcsId = glCreateShader(GL_TESS_CONTROL_SHADER);
        shaderTypeInProcess = this->tcsId;
    } else if(type == tesEvaluationShader) {
        this->tesId = glCreateShader(GL_TESS_EVALUATION_SHADER);
        shaderTypeInProcess = this->tesId;
    } else {
        cout << "unknown type of shader exiting... " << endl;
        exit(-1);
    }
    glShaderSource(shaderTypeInProcess, 1, &sourcePtr, nullptr);
    glCompileShader(shaderTypeInProcess);
    glGetShaderiv(shaderTypeInProcess, GL_COMPILE_STATUS, &success);

    if(!success) {

        if(type == vertexShader) {
            cout << "vertex shader could not be compiled. " << endl;
        } else if(type == fragmentShader) {
            cout << "fragment shader could not be compiled. " << endl;
        } else if(type == geometryShader) {
            cout << "geometry shader could not be compiled. " << endl;
        } else if(type == tesControlShader) {
            cout << "tessellation control shader could not be compiled. " << endl;
        } else if(type == tesEvaluationShader) {
            cout << "tessellation evaluation shader could not be compiled. " << endl;
        }
        cout << "shader path: " << path << endl;
        glGetShaderInfoLog(shaderTypeInProcess, 1000, nullptr, infolog);
        cout << "infolog: " << infolog << endl;
        exit(-4);
    }

    glAttachShader(this->programId, shaderTypeInProcess);

}


string ShaderProgram::getShaderFromFile(string fileName) {
    string res{};

    fstream file(fileName);
    if(!file.is_open()) {
        cout << fileName << " could not be opened. " << endl;
        exit(-2);
    }

    string line;
    while(getline(file, line)) {
        res += line;
        res += '\n';
    }

    file.close();
    return res;
}

bool ShaderProgram::isBound() {
    GLint prog{};
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    if (prog != this->programId) {
        this->inUse = false;

        cout << "not in use: " << endl;
    }
    return this->inUse;
}

void ShaderProgram::setBool(const string& name, bool val) {
    if(this->isBound()) {
        glUniform1i(glGetUniformLocation(this->programId, name.c_str()), (val ? 1 : 0) );
    } else {
        cout << "program object is not bound." << endl;
        cout << "in setBool() " << endl;
        cout << "trying to set: " << name << endl;
    }
}

void ShaderProgram::setFloat(const string& name, float val) {
    if(this->isBound()) {
        glUniform1f(glGetUniformLocation(this->programId, name.c_str()), val);
    } else {
        cout << "program object is not bound." << endl;
        cout << "in setfloat() " << endl;
        cout << "trying to set: " << name << endl;
    }
}

void ShaderProgram::setInt(const string& name, int val) {
    if(this->isBound()) {
        glUniform1i(glGetUniformLocation(this->programId, name.c_str()), val);
    } else {
        cout << "program object is not bound." << endl;
        cout << "in setint() " << endl;
        cout << "trying to set: " << name << endl;
    }
}

void ShaderProgram::setMat4(const string& name, const glm::mat4& val) {
    if(this->isBound()) {
        GLuint loc = glGetUniformLocation(this->programId, name.c_str());
        if(loc != -1) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, &val[0].x);
        } else {
            cout << "there is no mat4 uniform var named: " << name << endl;
            cout << "exiting... " << endl;
            exit(-5);
        }
    } else {
        cout << "program object is not bound." << endl;
        cout << "in setmat4() " << endl;
        cout << "trying to set: " << name << endl;
    }

}
void ShaderProgram::setMat3(const string& name, const glm::mat3& val) {
    if(this->isBound()) {
        GLuint loc = glGetUniformLocation(this->programId, name.c_str());
        if(loc != -1) {
            glUniformMatrix3fv(loc, 1, GL_FALSE, &val[0].x);
        } else {
            cout << "there is no mat4 uniform var named: " << name << endl;
            cout << "exiting... " << endl;
            exit(-5);
        }

    } else {
        cout << "program object is not bound. " << endl;
        cout << "in setMat3() " << endl;
        cout << "trying to set: " << name << endl;
    }
}

void ShaderProgram::setVec2(const string& name, const glm::vec2& val) {
    if(this->isBound()) {
        GLuint loc = glGetUniformLocation(this->programId, name.c_str());
        if(loc != -1) {
            glUniform2fv(loc, 1, &val.x);
        } else {
            cout << "there is no vec2 uniform var named: " << name << endl;
            cout << "exiting... " << endl;
            exit(-5);
        }

    } else {
        cout << "program object is not bound. " << endl;
        cout << "in setVec2() " << endl;
        cout << "trying to set: " << name << endl;
    }   
}

void ShaderProgram::setVec3(const string& name, const glm::vec3& val) {
    if(this->isBound()) {
        GLuint loc = glGetUniformLocation(this->programId, name.c_str());
        if(loc != -1) {
            glUniform3fv(loc, 1, &val.x);
        } else {
            cout << "there is no vec3 uniform var named: " << name << endl;
            cout << "exiting... " << endl;
            exit(-5);
        }

    } else {
        cout << "program object is not bound. " << endl;
        cout << "in setVec3() " << endl;
        cout << "trying to set: " << name << endl;
    }   
}

void ShaderProgram::setVec4(const string& name, const glm::vec4& val) {
    if(this->isBound()) {
        GLuint loc = glGetUniformLocation(this->programId, name.c_str());
        if(loc != -1) {
            glUniform4fv(loc, 1, &val.x);
        } else {
            cout << "there is no vec4 uniform var named: " << name << endl;
            cout << "exiting... " << endl;
            exit(-5);
        }

    } else {
        cout << "program object is not bound. " << endl;
        cout << "in setVec4() " << endl;
        cout << "trying to set: " << name << endl;
    }   
}


void ShaderProgram::useProgram() {
    glUseProgram(this->programId);
    this->inUse = true;
}

void ShaderProgram::linkProgram() {
    glLinkProgram(this->programId);
    int success{};
    char infolog[1024];
    glGetProgramiv(this->programId, GL_LINK_STATUS, &success);
    if(!success) {
        cout << "program object could not be linked. " << endl;
        glGetProgramInfoLog(this->programId, 1000, nullptr, infolog);
        cout << "infolog: " << infolog << endl;
        exit(-6);
    }
}

GLuint ShaderProgram::getAttribLocation(const string& name) {
    if(this->isBound()) {
        return glGetAttribLocation(this->programId, name.c_str());
    } else {
        cout << "program object is not in use. " << endl;
        cout << "can not retrieve attribute location named: " << name << endl;
        cout << "exiting... " << endl;
        exit(-3);
    }
}