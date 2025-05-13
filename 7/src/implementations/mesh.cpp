#include "mesh.hpp"
#include "transformations.hpp"
// other includes above
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

int Mesh::active_texture_unit_count{0};

Mesh::Mesh(const string& objPath) {

    this->myObjLoader(objPath);
    this->createObjTangents();
    this->setupMesh();

    this->modelMatrix = glm::mat4(1);


}

void Mesh::centerTheMesh() {
    float midX, midY, midZ;
    midX = (this->maxX + this->minX)/2;
    midY = (this->maxY + this->minY)/2;
    midZ = (this->maxZ + this->minZ)/2;

    this->modelMatrix = glm::translate(glm::mat4(1), -glm::vec3(midX, midY, midZ));
}

glm::mat4 Mesh::getModelMatrix() {
    return this->modelMatrix;
}

Texture Mesh::textureCreator(int width, int height, const string& type, GLenum format, GLenum typeName) {
// Texture Mesh::textureCreator(int width, int height, const string& type, GLenum format) {
    Texture tex;
    tex.type = string(type);
    tex.textureUnit = Mesh::active_texture_unit_count;
    Mesh::active_texture_unit_count++;

    glActiveTexture(GL_TEXTURE0 + tex.textureUnit);
    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    // glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, typeName, nullptr);

    // glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return tex;
}

void Mesh::addTexture(Texture& tex) {
    this->textures.push_back(tex);
}

void Mesh::myTextureLoader(const string& path, const string& type) {
    Texture tex;
    tex.type = string(type);
    // tex.textureUnit = this->textures.size();
    tex.textureUnit = Mesh::active_texture_unit_count;
    Mesh::active_texture_unit_count++;
    glActiveTexture(GL_TEXTURE0 + tex.textureUnit);

    int width, height, numberOfChannels;
    stbi_set_flip_vertically_on_load(true);
    // unsigned char* data = stbi_load(path.c_str(), &width, &height, &numberOfChannels, STBI_rgb_alpha);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &numberOfChannels, 0);
    // cout << "NoC: " << numberOfChannels << endl;

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);

    if (data) {

        if(numberOfChannels == 3) {
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if(numberOfChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        } 
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    this->textures.push_back(tex);

    stbi_image_free(data);

}

void Mesh::textureCubeMapLoader(vector<string>& paths, const string& type) {

    Texture tex;
    tex.type = string(type);
    // tex.textureUnit = this->textures.size();
    tex.textureUnit = Mesh::active_texture_unit_count;
    Mesh::active_texture_unit_count++;
    glActiveTexture(GL_TEXTURE0 + tex.textureUnit);
    this->cubeMapTextureUnitId = tex.textureUnit;

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex.id);

    int width, height, numberOfChannels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data{}; // = stbi_load(path.c_str(), &width, &height, &numberOfChannels, 0);

    for(int i=0; i<paths.size(); i++) {
        data = stbi_load(paths[i].c_str(), &width, &height, &numberOfChannels, 0);
        if(data) {

            if(numberOfChannels == 3) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            } else if(numberOfChannels == 4) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }

            stbi_image_free(data);
        }       
    }

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    this->textures.push_back(tex);
    
}

void Mesh::myObjLoader(const string& path) {
    this->vertices.clear();

    // function scope buffers.
    vector<glm::vec3> vertex_p; 
    vector<glm::vec3> vertex_n;
    vector<glm::vec2> vertex_t;

    glm::vec3 tempP, tempN; // position normal and texturecoord variables.
    glm::vec2 tempT;

    // face indices.
    vector<int> pst_indices, nrm_indices, txt_indices;

    stringstream ss{};
    ifstream myFile(path);
    if(!myFile.is_open()) {
        cout << path << " named file could not be opened. " << endl;
        exit(-9);
    }

    string line{}, prefix{}; // for extracting lines from file.
    int quadNumber{};

    while(getline(myFile, line)) {
        ss.clear();
        ss.str(line);
        ss >> prefix;

        if(prefix == "v") { // extract vertex positions.

            ss >> tempP.x;
            ss >> tempP.y;
            ss >> tempP.z;
            vertex_p.push_back(tempP);

            // find bounding box.
            if(tempP.x > this->maxX) this->maxX = tempP.x;
            if(tempP.y > this->maxY) this->maxY = tempP.y;
            if(tempP.z > this->maxZ) this->maxZ = tempP.z;

            if(tempP.x < this->minX) this->minX = tempP.x;
            if(tempP.y < this->minY) this->minY = tempP.y;
            if(tempP.z < this->minZ) this->minZ = tempP.z;

        } else if(prefix == "vt") { // extract texture coords.

            ss >> tempT.x;
            ss >> tempT.y;

            vertex_t.push_back(tempT);

        } else if(prefix == "vn") { // extract vertex normals.

            ss >> tempN.x;
            ss >> tempN.y;
            ss >> tempN.z;

            vertex_n.push_back(tempN);

        } else if(prefix == "f") { // extract faces.

            GLint tempInt; // extracted value.
            int counter{}; // determines we are at position or texture coordinate or normal.
            int isPolygon{}; // counts number of edges. 

            while(ss >> tempInt) {

                if(counter == 0) {
                    pst_indices.push_back(tempInt-1);
                } else if(counter == 1) {
                    txt_indices.push_back(tempInt-1);
                } else if(counter == 2) {
                    nrm_indices.push_back(tempInt-1);
                }
                if(ss.peek() == '/') {
                    counter++;
                    ss.ignore(1, '/');

                    if(ss.peek() == '/') {
                        counter++;
                        ss.ignore(1, '/');
                        txt_indices.push_back(0);
                        cout << "here double //" << endl;
                        cout << "line: " << line << endl;
                        cin.get();

                    }
                } else if(ss.peek() == ' ') {
                    counter++;
                    isPolygon++;

                    if(isPolygon == 4) { // this means we are at a quad. this part triangulates the quad.

                        GLint tmp = pst_indices[pst_indices.size()-1];
                        pst_indices[pst_indices.size()-1] = pst_indices[pst_indices.size()-4];
                        pst_indices.push_back(pst_indices[pst_indices.size()-2]);
                        pst_indices.push_back(tmp);

                        tmp = txt_indices[txt_indices.size()-1];
                        txt_indices[txt_indices.size()-1] = txt_indices[txt_indices.size()-4];
                        txt_indices.push_back(txt_indices[txt_indices.size()-2]);
                        txt_indices.push_back(tmp);

                        tmp = nrm_indices[nrm_indices.size()-1];
                        nrm_indices[nrm_indices.size()-1] = nrm_indices[nrm_indices.size()-4];
                        nrm_indices.push_back(nrm_indices[nrm_indices.size()-2]);
                        nrm_indices.push_back(tmp);

                        isPolygon = 0; // 
                    }

                    ss.ignore(1, ' ');
                }
                counter %= 3;
            }

        }
    }

    this->unique_vertex_positions = vertex_p;
    this->unique_vertex_normals = vertex_n;
    this->unique_vertex_texCoords = vertex_t;

    for(int i=0; i<pst_indices.size(); i++) {
        Vertex v;
        v.position = vertex_p[pst_indices[i]];
        v.normal = vertex_n[nrm_indices[i]];
        v.textureCoords = vertex_t[txt_indices[i]];
        this->vertices.push_back(v);
    }

    myFile.close();

    return;
}

void Mesh::createObjTangents() {

    glm::vec3 e1, e2;
    glm::vec2 tex1, tex2;
    glm::vec3 T, B;

    for(int i=0; i<this->vertices.size(); i+=3) {

        e1 = this->vertices[i+1].position - this->vertices[i].position;
        e2 = this->vertices[i+2].position - this->vertices[i].position;

        tex1 = this->vertices[i+1].textureCoords - this->vertices[i].textureCoords;
        tex2 = this->vertices[i+2].textureCoords - this->vertices[i].textureCoords;

        float a = tex1.x;
        float b = tex2.x;
        float c = tex1.y;
        float d = tex2.y;

        float k = a * d - b * c;
        k = 1/k;

        T = d * e1 - c * e2;
        T *= k;
        B = -b * e1 + a * e2;
        B *= k;

        this->vertices[i].tangent = T;
        this->vertices[i+1].tangent = T;
        this->vertices[i+2].tangent = T;

        this->vertices[i].bitangent = B;
        this->vertices[i+1].bitangent = B;
        this->vertices[i+2].bitangent = B;
    }

    return;
}



void Mesh::setupMesh() {

    // GLint val;
    // glGetIntegerv(GL_CURRENT_PROGRAM, &val);
    // cout << "mesh icinde bagli program id'si: " << val << endl;




    glGenVertexArrays(1, &this->vaoId);
    glBindVertexArray(this->vaoId);

    glCreateBuffers(1, &this->vboId);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboId);
    glBufferData(GL_ARRAY_BUFFER, 
                this->vertices.size() * sizeof(Vertex), 
                &this->vertices[0].position.x, 
                GL_STATIC_DRAW);

    // in fact this could be like this:  
    // give shader program object as parameter.
    // then use a getter method of shader program object for attribute locations map of shader program. 
    this->meshAttribLocMap.emplace("a_pos", 0);
    this->meshAttribLocMap.emplace("a_nrm", 1);
    this->meshAttribLocMap.emplace("a_texCoord", 2);
    this->meshAttribLocMap.emplace("a_tangent", 3);
    this->meshAttribLocMap.emplace("a_bitangent", 4);

    GLuint a_loc;
    a_loc = this->meshAttribLocMap["a_pos"];
    if(a_loc != -1) {
        glEnableVertexAttribArray(a_loc);
        glVertexAttribPointer(a_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    }
    a_loc = this->meshAttribLocMap["a_nrm"];
    if(a_loc != -1) {
        glEnableVertexAttribArray(a_loc);
        glVertexAttribPointer(a_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    }
    a_loc = this->meshAttribLocMap["a_texCoord"];
    if(a_loc != -1) {
        glEnableVertexAttribArray(a_loc);
        glVertexAttribPointer(a_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));
    }
    a_loc = this->meshAttribLocMap["a_tangent"];
    if(a_loc != -1) {
        glEnableVertexAttribArray(a_loc);
        glVertexAttribPointer(a_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    }
    a_loc = this->meshAttribLocMap["a_bitangent"];
    if(a_loc != -1) {
        glEnableVertexAttribArray(a_loc);
        glVertexAttribPointer(a_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
        // cout << " calisiyor la. ' " << endl;
    }

    // glBindVertexArray(0); // unbind vao object. cuz not in use. 
}

void Mesh::draw(ShaderProgram& program) {
    program.useProgram();
    // glEnableVertexAttribArray(0);

    for(int i=0; i<this->textures.size(); i++) {

        glActiveTexture(GL_TEXTURE0 + this->textures[i].textureUnit);
        // set uniform sampler.
        glUniform1i(glGetUniformLocation(program.programId, this->textures[i].type.c_str()), this->textures[i].textureUnit);
    }

    glBindVertexArray(this->vaoId);
    glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
    // glDrawArrays(GL_POINTS, 0, this->vertices.size());
    glBindVertexArray(0);

}

void Mesh::setPatchSize(int n) {
    glBindVertexArray(0);
    GLuint a_loc;
    glGenVertexArrays(1, &this->patch_vaoId);
    glBindVertexArray(this->patch_vaoId);
    // cout << "patch vao id: " << this->patch_vaoId << endl;

    glCreateBuffers(1, &this->patch_pboId);
    glBindBuffer(GL_ARRAY_BUFFER, this->patch_pboId);
    glBufferData(GL_ARRAY_BUFFER, this->unique_vertex_positions.size() * sizeof(glm::vec3), 
                            &this->unique_vertex_positions[0].x, GL_STATIC_DRAW);
    a_loc = this->meshAttribLocMap["a_pos"]; // i am getting attribute locations from map. 
                                             // this map is attribute of this class. 
                                             // this map could come from a parameter which is ShaderProgram type.
                                             // of course as a convention i assume a program is bound to opengl context.
    glEnableVertexAttribArray(a_loc);
    glVertexAttribPointer(a_loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


    glCreateBuffers(1, &this->patch_nboId);
    glBindBuffer(GL_ARRAY_BUFFER, this->patch_nboId);
    glBufferData(GL_ARRAY_BUFFER, this->unique_vertex_normals.size() * sizeof(glm::vec3), 
                            &this->unique_vertex_normals[0].x, GL_STATIC_DRAW);
    a_loc = this->meshAttribLocMap["a_nrm"];
    glEnableVertexAttribArray(a_loc);
    glVertexAttribPointer(a_loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


    glCreateBuffers(1, &this->patch_texboId);
    glBindBuffer(GL_ARRAY_BUFFER, this->patch_texboId);
    glBufferData(GL_ARRAY_BUFFER, this->unique_vertex_texCoords.size() * sizeof(glm::vec2), 
                            &this->unique_vertex_texCoords[0].x, GL_STATIC_DRAW);
    a_loc = this->meshAttribLocMap["a_texCoord"];
    glEnableVertexAttribArray(a_loc);
    glVertexAttribPointer(a_loc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


    glPatchParameteri(GL_PATCH_VERTICES, n);

}

void Mesh::drawPatches(ShaderProgram& program) {
    program.useProgram();

    for(int i=0; i<this->textures.size(); i++) {

        glActiveTexture(GL_TEXTURE0 + this->textures[i].textureUnit);
        // set uniform sampler.
        glUniform1i(glGetUniformLocation(program.programId, this->textures[i].type.c_str()), this->textures[i].textureUnit);
    }

    // glBindVertexArray(this->vaoId);
    glBindVertexArray(this->patch_vaoId);
    glDrawArrays(GL_PATCHES, 0, this->unique_vertex_positions.size());
    glBindVertexArray(0);
}

void Mesh::transformMesh(const glm::mat4& mat) {
    this->modelMatrix = mat * this->modelMatrix;
    return;
}

vector<glm::vec3> Mesh::getUniqueVertexPositions() {
    return this->unique_vertex_positions;
}

vector<glm::vec3> Mesh::getUniqueVertexNormals() {
    return this->unique_vertex_normals;
}

vector<glm::vec2> Mesh::getUniqueVertexTexCoords() {
    return this->unique_vertex_texCoords;
}