#include "framebuffer.hpp"


Framebuffer::Framebuffer(int width, int height) {
    this->height = height;
    this->width = width;
    glGenFramebuffers(1, &this->framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, this->framebufferId);
}

// void Framebuffer::createTexture(int width, int height, const string& type, GLenum format) {
void Framebuffer::createTexture(const string& type, GLenum format) {
    Texture tex = Mesh::textureCreator(this->width, this->height, type, format);
    this->texture = Texture(tex);
}

void Framebuffer::createDepthBuffer() {
    glGenRenderbuffers(1, &this->depthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, this->depthBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->width, this->height);

}

void Framebuffer::configureFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->framebufferId);
    
    // bind depth buffer to framebuffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthBufferId);

    // bind color buffers:
    // for(int i=0; i<this->textures.size(); i++) {
    //     glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, this->textures[i].id, 0);
    //     // this->drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
    // }
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->texture.id, 0);
    

    glDrawBuffers(1, this->drawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        cout << "frame buffer is not complete. \nexiting... " << endl;
        this->isComplete = false;
        exit(-7);
        // return;
    }
    this->isComplete = true;

}

int Framebuffer::getWidth() {
    return this->width;
}

int Framebuffer::getHeight() {
    return this->height;
}

void Framebuffer::setAsRenderTarget() {

    if(!this->isComplete) {
        cout << "this framebuffer is not ready to be used." << endl;
        cout << "exiting. " << endl;
        exit(-7);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->framebufferId);
    glViewport(0, 0, this->width, this->height);
}

Texture& Framebuffer::getTexture() {
    return this->texture;
}