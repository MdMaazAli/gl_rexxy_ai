#include <iostream>
#include <bits/stdc++.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Socket.h"
#include "Shader.h"
#include "SimpleMesh.h"
#include "Camera.h"
using namespace std;
using namespace glm;

Camera camera(glm::vec3(0.0f,0.0f,8.0f));

float xPos = 0.0f;
float zPos = 0.0f;
glm::vec3 lightPos = glm::vec3(1.0f,1.0f,2.0f);
glm::vec3 lightColor = glm::vec3(0.8f,0.8f,0.2f);

bool mouseFirst = true;
float lastX = 800.0f/2.0f;
float lastY = 600.0f/2.0f;

void mouse_callback(GLFWwindow* window,double xPosIn,double yPosIn){
    float xPos = (float)xPosIn;
    float yPos = (float)yPosIn;

    if(mouseFirst){
        xPos = lastX;
        yPos = lastY;
        mouseFirst = false;
    }

    float xOffset = xPos-lastX;
    float yOffset = lastY-yPos;
    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset,yOffset);
}

void scroll_callback(GLFWwindow* window,double xOffset,double yOffset){
    camera.ProcessMouseScroll((float)yOffset);
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow* window){
    if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window,true);
    }
    if(glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS){
        camera.ProcessKeyboard(FORWARD,deltaTime);
    }
    if(glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS){
        camera.ProcessKeyboard(BACKWARD,deltaTime);
    }
    if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS){
        camera.ProcessKeyboard(LEFT,deltaTime);
    }
    if(glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS){
        camera.ProcessKeyboard(RIGHT,deltaTime);
    }
}

void framebuffer_size_callback(GLFWwindow* window,int width,int height){
    glViewport(0,0,width,height);
}


int main(){

    SOCKET sock = initSocket();

    if(sock == INVALID_SOCKET){
        std::cout<<"Socket failed\n";
    }
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800,600,"GL_REXXY",NULL,NULL);
    if(window == NULL){
        cout<<"failed to initialize window"<<endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cout<<"failed to initialize GLAD"<<endl;
        return -1;
    }

    Shader rexxyShader("src/rexxyVertShader.glsl","src/rexxyFragShader.glsl");
    SimpleMesh rexxyMesh("sphere");

    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window,scroll_callback);

    glEnable(GL_DEPTH_TEST);
    glViewport(0,0,800,600);

    while(!glfwWindowShouldClose(window)){
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;

        const char* msg = "REXXY runs fast and jumps high";
        send(sock,msg,strlen(msg),0);

        char buffer[512];
        int bytesReceived = recv(sock, buffer, sizeof(buffer)-1, 0);
        
        if(bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "AI says: " << buffer << std::endl;
        }

        processInput(window);

        glClearColor(0.1f,0.2f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(radians(camera.Zoom),800.0f/600.0f,0.1f,1000.0f);

        glm::mat4 view = glm::mat4(1.0f);
        view = camera.GetViewMatrix();

        glm::vec3 lightPosView = vec3(view*vec4(lightPos,1.0f));

        rexxyShader.use();
        glm::mat4 rexxy = glm::mat4(1.0f);
        rexxy = glm::scale(rexxy,glm::vec3(0.5f));
        rexxyShader.setMat4("model",rexxy);
        rexxyShader.setMat4("view",view);
        rexxyShader.setMat4("projection",projection);
        rexxyShader.setVec3("lightColor",lightColor);
        rexxyShader.setVec3("lightPosView",lightPosView);
        rexxyMesh.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    closeSocket(sock);
    return 0;
}