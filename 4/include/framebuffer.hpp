#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh.hpp"

class Framebuffer {
    private:
        int width, height;
        GLuint framebufferId, depthBufferId;
        Texture texture;
        GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        bool isComplete{false};


    public:
        Framebuffer(int width, int height);
        // void createTexture(int width, int height, const string& type, GLenum format);
        void createTexture(const string& type, GLenum format);
        void createDepthBuffer();
        void configureFramebuffer();
        int getWidth();
        int getHeight();
        void setAsRenderTarget();
        Texture& getTexture();


};


#endif