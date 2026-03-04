#include <iostream>
#include <bits/stdc++.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
using namespace glm;

class SimpleMesh{
    private:
        string type;

        // Helper to generate sphere data dynamically
        std::vector<float> generateSphereData(float radius, int sectors, int stacks) {
        std::vector<float> data;
        const float PI = 3.14159265359f;
        float sectorStep = 2 * PI / sectors;
        float stackStep = PI / stacks;

        for(int i = 0; i < stacks; ++i) {
            float stackAngle1 = PI / 2 - i * stackStep;
            float stackAngle2 = PI / 2 - (i + 1) * stackStep;

            for(int j = 0; j <= sectors; ++j) {
                float sectorAngle = j * sectorStep;
                float nextSectorAngle = (j+1) * sectorStep;

                auto pushVertex = [&](float stackAngle, float secAngle) {
                    float x = radius * cosf(stackAngle) * cosf(secAngle);
                    float y = radius * sinf(stackAngle);
                    float z = radius * cosf(stackAngle) * sinf(secAngle);

                    data.push_back(x); data.push_back(y); data.push_back(z); // Pos
                    data.push_back(1.0f); data.push_back(1.0f); data.push_back(1.0f); // Color
                    data.push_back((float)j / sectors); data.push_back((float)i / stacks); // Tex
                    data.push_back(x); data.push_back(y); data.push_back(z); // Norm (not normalized)
                };

                pushVertex(stackAngle1, sectorAngle);
                pushVertex(stackAngle2, sectorAngle);
                pushVertex(stackAngle1, nextSectorAngle);

                pushVertex(stackAngle2, sectorAngle);
                pushVertex(stackAngle2, nextSectorAngle);
                pushVertex(stackAngle1, nextSectorAngle);
            }
        }
        return data;
    }
    
    public:
        unsigned int VAO;
        int vertexCount;
        SimpleMesh(string type){
            if(type=="cube"){
                float vertices[] = {
                    // vertices     // color        // texCoord         // normals
                    
                    // BACK FACE (Normal: 0, 0, -1)
                    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
                    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
                    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
                    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
                    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
                    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
        
                    // FRONT FACE (Normal: 0, 0, 1)
                    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
                    0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
                    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
                    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
                    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
                    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
        
                    // LEFT FACE (Normal: -1, 0, 0)
                    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
                    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
                    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
                    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
                    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
                    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        
                    // RIGHT FACE (Normal: 1, 0, 0)
                    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
                    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
                    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
                    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
                    0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
                    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
        
                    // BOTTOM FACE (Normal: 0, -1, 0)
                    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
                    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
                    0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
                    0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
                    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
                    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
        
                    // TOP FACE (Normal: 0, 1, 0)
                    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
                    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
                    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
                    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
                    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
                    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
                };
                vertexCount=36;
                setupBuffers(vertices,sizeof(vertices));
            }
            else if(type == "pyramid"){
                float vertices[] = {
                    // positions       // color         // texCoord  // normals
                
                    // Front Face
                    -0.5f,-0.5f,0.5f,  1.0f,1.0f,1.0f,  0.0f,0.0f,  0.0f,0.45f,0.89f,
                    0.5f,-0.5f,0.5f,   1.0f,1.0f,1.0f,  1.0f,0.0f,  0.0f,0.45f,0.89f,
                    0.0f,0.5f,0.0f,    1.0f,1.0f,1.0f,  0.5f,1.0f,  0.0f,0.45f,0.89f,

                    // Right Face
                    0.5f,-0.5f,0.5f,   1.0f,1.0f,1.0f,  0.0f,0.0f,  0.89f,0.45f,0.0f,
                    0.5f,-0.5f,-0.5f,  1.0f,1.0f,1.0f,  1.0f,0.0f,  0.89f,0.45f,0.0f,
                    0.0f,0.5f,0.0f,    1.0f,1.0f,1.0f,  0.5f,1.0f,  0.89f,0.45f,0.0f,

                    // Back Face
                    0.5f,-0.5f,-0.5f,  1.0f,1.0f,1.0f,  0.0f,0.0f,  0.0f,0.45f,-0.89f,
                    -0.5f,-0.5f,-0.5f, 1.0f,1.0f,1.0f,  1.0f,0.0f,  0.0f,0.45f,-0.89f,
                    0.0f,0.5f,0.0f,    1.0f,1.0f,1.0f,  0.5f,1.0f,  0.0f,0.45f,-0.89f,

                    // Left Face
                    -0.5f,-0.5f,-0.5f, 1.0f,1.0f,1.0f,  0.0f,0.0f,  -0.89f,0.45f,0.0f,
                    -0.5f,-0.5f,0.5f,  1.0f,1.0f,1.0f,  1.0f,0.0f,  -0.89f,0.45f,0.0f,
                    0.0f,0.5f,0.0f,    1.0f,1.0f,1.0f,  0.5f,1.0f,  -0.89f,0.45f,0.0f,

                    // Bottom Face
                    -0.5f,-0.5f,-0.5f, 1.0f,1.0f,1.0f,  0.0f,1.0f,  0.0f,-1.0f,0.0f,
                    0.5f,-0.5f,-0.5f,  1.0f,1.0f,1.0f,  1.0f,1.0f,  0.0f,-1.0f,0.0f,
                    0.5f,-0.5f,0.5f,   1.0f,1.0f,1.0f,  1.0f,0.0f,  0.0f,-1.0f,0.0f,
                    0.5f,-0.5f,0.5f,   1.0f,1.0f,1.0f,  1.0f,0.0f,  0.0f,-1.0f,0.0f,
                    -0.5f,-0.5f,0.5f,  1.0f,1.0f,1.0f,  0.0f,0.0f,  0.0f,-1.0f,0.0f,
                    -0.5f,-0.5f,-0.5f, 1.0f,1.0f,1.0f,  0.0f,1.0f,  0.0f,-1.0f,0.0f
                    };
                setupBuffers(vertices,sizeof(vertices));
                vertexCount = 18;
            }
            else if(type == "sphere"){
                vector<float> sphereData = generateSphereData(0.5f, 36, 18);
                setupBuffers(sphereData.data(), sphereData.size() * sizeof(float));
                vertexCount = sphereData.size() / 11;
            }
        }

        void setupBuffers(float* vertices,size_t size){
            glGenVertexArrays(1,&VAO);
            glBindVertexArray(VAO);

            unsigned int VBO;
            glGenBuffers(1,&VBO);
            glBindBuffer(GL_ARRAY_BUFFER,VBO);
            glBufferData(GL_ARRAY_BUFFER,size,vertices,GL_STATIC_DRAW);

            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(0));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(3*sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(5*sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,11*sizeof(float),(void*)(8*sizeof(float)));
            glEnableVertexAttribArray(3);
        }

        void draw(){
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES,0,vertexCount);
        }
};