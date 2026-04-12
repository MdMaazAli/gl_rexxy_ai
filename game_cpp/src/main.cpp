#include <iostream>
#include <vector>
#include <random>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Socket.h"
#include "Shader.h"
#include "SimpleMesh.h"
#include "Camera.h"
#include "Collision.h"
#include "TelemetryDashboard.h"
using namespace std;
using namespace glm;

Camera camera(glm::vec3(0.0f,0.0f,7.0f));

float xPos = 0.0f;
float zPos = 0.0f;
glm::vec3 lightPos = glm::vec3(1.0f,1.0f,2.0f);
glm::vec3 lightColor = glm::vec3(0.8f,0.8f,0.2f);

bool mouseFirst = true;
float lastX = 800.0f/2.0f;
float lastY = 600.0f/2.0f;

// mouse capture toggle
bool lstAltState = false;
bool captureMouse = false;

void mouse_callback(GLFWwindow* window,double xPosIn,double yPosIn){
    if(captureMouse){
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
}

void scroll_callback(GLFWwindow* window,double xOffset,double yOffset){
    if(!ImGui::GetIO().WantCaptureMouse && captureMouse){
        camera.ProcessMouseScroll((float)yOffset);
    }
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float speedY = 0.0f;
glm::vec3 playerPos = glm::vec3(0.0f);
bool isGrounded = true;

bool isShoot = false;
bool lstSpaceState = false;

int action = 0;
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

    // mouse capture toggle
    bool currAltState = (glfwGetKey(window,GLFW_KEY_LEFT_ALT) == GLFW_PRESS);
    if(currAltState && !lstAltState){
        captureMouse = !captureMouse;

        if(captureMouse){
            glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
            mouseFirst = true;
        }
        else{
            glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
        }
    }
    lstAltState = currAltState;

    // debug
    action = 0;
    // if(glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded){
    //     action = 1;
    // }
    bool currSpaceState = (glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS);
    if(currSpaceState && !lstSpaceState){
        isShoot = true;
    }
    lstSpaceState = currSpaceState;
    if(action == 1 && isGrounded){
        isGrounded = false;
        speedY = 5.0f;
    }
}

void framebuffer_size_callback(GLFWwindow* window,int width,int height){
    glViewport(0,0,width,height);
}

void initModel(Shader shader,glm::mat4 model,glm::mat4 view,glm::mat4 projection,glm::vec3 lightColor,glm::vec3 lightPosView){
    shader.setMat4("model",model);
    shader.setMat4("view",view);
    shader.setMat4("projection",projection);
    shader.setVec3("lightColor",lightColor);
    shader.setVec3("lightPosView",lightPosView);
}

struct Player{
    glm::vec3 Pos;
    glm::vec3 Size;
};

struct Obstacle{
    glm::vec3 Pos;
    glm::vec3 Size;
    float Speed;
};

struct Collider{
    glm::vec3 Pos;
    glm::vec3 Size;
    float speed;
};

struct Ammo{
    glm::vec3 Pos; 
    float Size; 
    glm::vec3 Speed; 
    glm::vec3 ShootCords;
    bool Fired; 
};

vector<float> getState(const vector<Obstacle>& obstacles,float reward,bool done){
    float distance = 20.0f;
    float speed = 0.0f;
    float height = 0.0f;
    float currReward = reward;
    bool isDone = done;
    
    for(auto& obs:obstacles){
        float dist = obs.Pos.x;
        if(dist > 0 && dist < distance){
            distance = dist;
            speed = obs.Speed;
            height = obs.Size.y;
        }
    }
    
    return {playerPos.y,speedY,isGrounded ? 1.0f:0.0f,distance,height,speed,currReward,isDone ? 1.0f:0.0f};
}

int main(){

    SOCKET sock = initSocket();
    
    if(sock == INVALID_SOCKET){
        std::cout<<"Socket failed\n";
    }
    
    // DWORD timeout = 1; // 1 ms
    // setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    
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
    
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
    glfwSetScrollCallback(window,scroll_callback);
    
    // initialising gui
    // dashboard
    TelemetryDashboard dashboard;
    dashboard.Init(window);

    // tracking the rexxy's performance
    float currentFrames = 0.0f,highScoreFrames = 0.0f;
    float currentSurvivalTime = 0.0f,maxSurvivalTime = 0.0f;

    // seeding random values
    std::random_device rd;  // Hardware entropy source (if available)
    std::mt19937 gen(rd()); // Mersenne Twister engine

    // Define our variance windows
    std::uniform_real_distribution<float> spawnDist(1.2f, 2.5f); // Spawn every 1.2s to 2.5s
    std::uniform_real_distribution<float> speedDist(4.0f, 7.5f); // Move at 4.0 to 7.5 units/sec
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cout<<"failed to initialize GLAD"<<endl;
        return -1;
    }

    Shader rexxyShader("src/rexxyVertShader.glsl","src/rexxyFragShader.glsl");
    SimpleMesh rexxyMesh("sphere");
    Player player;

    glEnable(GL_DEPTH_TEST);
    glViewport(0,0,800,600);
    
    vector<Obstacle> obstacles;
    SimpleMesh pyramid("pyramid");
    
    bool isCollision = false;
    float accTime = 0.0f,dt = 1/60.0f;

    // obs spawner
    float spawnTimer = 0.0f,spawnRate = 1.5f;

    Shader ammoShader("src/rexxyVertShader.glsl","src/rexxyFragShader.glsl");
    SimpleMesh bullet("sphere");
    vector<Ammo> ammoPool;
    // ammo tracking
    float shootTime = 0.0f;
    float bulletSpeed = 5.0f;
    int ammoLeft = 3;

    // ammo pool
    ammoPool.resize(3);
    for(int i=0; i<3; i++){
        ammoPool[i].Pos = glm::vec3(playerPos.x,0.0f,0.0f);
        ammoPool[i].Size = 0.2f;
        ammoPool[i].Speed = glm::vec3(bulletSpeed,0.0f,0.0f);
    }

    // rl learning timer
    // synced with the physics loop
    float rlTimer = 0.0f,rlStep = 1/60.0f; // can tweak the rlStep first slower then faster as the model's ability to think grows

    // reward variables
    float prevReward = 1.0f;
    bool prevDone = false;

    while(!glfwWindowShouldClose(window)){
        
        float currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        accTime += deltaTime;

        currentFrames++;
        currentSurvivalTime += deltaTime;
        
        processInput(window);
        
        glClearColor(0.1f,0.2f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(radians(camera.Zoom),800.0f/600.0f,0.1f,1000.0f);
        
        glm::mat4 view = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        
        glm::vec3 lightPosView = vec3(view*vec4(lightPos,1.0f));
        
        //  obstacles
        spawnTimer += deltaTime;
        if(spawnTimer >= spawnRate){
            Obstacle newObstacle;
            // Always spawn exactly at the edge of the world
            newObstacle.Pos = glm::vec3(20.0f, 0.0f, 0.0f); 
            newObstacle.Size = glm::vec3(0.4f);

            // Assign a highly randomized speed from our distribution
            newObstacle.Speed = speedDist(gen); 
            
            obstacles.push_back(newObstacle);
            
            // Reset the timer
            spawnTimer = 0.0f; 
            
            // Randomize the NEXT spawn time directly using the distribution
            spawnRate = spawnDist(gen);
        }
        
        // ammoPool
        if(glfwGetTime()-shootTime >= 5.0f && ammoLeft<=0){
            ammoLeft = 3;
            for(int i=0; i<3; i++){
                Ammo newAmmo;
                newAmmo.Pos = glm::vec3(playerPos.x,playerPos.y,0.0f);
                newAmmo.Size = 0.2f;
                newAmmo.Speed = glm::vec3(0.0f);
                ammoPool.push_back(newAmmo);
            }
        }
        
        float reward = 1.0f;
        bool done = false;
        rlTimer += deltaTime;
        if(rlTimer >= rlStep){
            vector<float> states = getState(obstacles,prevReward,prevDone);
            send(sock,(char*)states.data(),sizeof(float)*8,0);
            
            // ----- DEBUG ----- //
            // cout << "STATE: ";
            // for(float s : states) cout << s << " ";
            // cout << endl;

            // resetting reward variables
            prevReward = reward;
            prevDone = false;
            
            int bytes = recv(sock, (char*)&action, sizeof(int), 0);
            // if(bytes > 0 ){
            //     cout<<"REXXY : "<<action<<endl;
            // }
            
            rlTimer -= rlStep;
        }
        
        // physics loop
        while(accTime>=dt){
            player.Pos = playerPos;
            player.Size = glm::vec3(0.5f);
            if(action == 1 && isGrounded){
                isGrounded = false;
                speedY = 5.0f;
            }
            if(!isGrounded){
                float gravity = 9.8f;
                speedY -= gravity*dt;
                playerPos.y += speedY*dt -(0.5f)*gravity*dt*dt;
                if(playerPos.y <= 0.0f && !isGrounded){
                    isGrounded = true;
                    playerPos.y = 0.0f;
                    speedY = 0.0f;
                }
            }
            for (auto it = obstacles.begin(); it != obstacles.end(); ) {
                it->Pos.x -= it->Speed*dt;
                glm::vec3 trueAABBCenter = it->Pos + glm::vec3(0.0f, it->Size.y / 2.0f, 0.0f);
                isCollision = Collision::CheckCollision_Sphere(player.Pos,0.25f,trueAABBCenter,it->Size);
                if(isCollision){
                    // cout<<"< ------------------------ DEBUG::COLLISION ----------------------- >"<<endl;
                    done = true;
                }
                if(it->Pos.x < -10.0f){
                    it = obstacles.erase(it);
                }
                else{
                    ++it;
                }
            }

            // ammoPool
            for(int i=ammoPool.size()-1; i>=0; i--){
                if(isShoot && ammoPool[i].Fired==false){
                    cout<<"DEBUG::SHOOTING!!"<<endl;
                    if(ammoLeft == 1){
                        shootTime = glfwGetTime();
                    }
                    ammoPool[i].Fired = true;
                    ammoPool[i].Pos = glm::vec3(playerPos.x,playerPos.y,0.0f);
                    ammoPool[i].ShootCords = ammoPool[i].Pos;
                    ammoPool[i].Speed.x = bulletSpeed;
                    isShoot = false;
                    ammoLeft--;
                }
                else if(ammoPool[i].Fired == true){
                    float dist = glm::distance(ammoPool[i].Pos,ammoPool[i].ShootCords);
                    
                    if(dist >= 20.0f){
                        // ----- DEBUG:OUT OF RANGE ----- //
                        ammoPool[i].Fired = false;
                    }
                    for(auto it = obstacles.begin(); it != obstacles.end(); ){
                        glm::vec3 trueAABBCenter = it->Pos + glm::vec3(0.0f, it->Size.y / 2.0f, 0.0f);
                        bool isCollision = Collision::CheckCollision_Sphere(ammoPool[i].Pos,0.2f,trueAABBCenter,it->Size);
                        // collision check
                        if(isCollision){
                            cout<<"DEBUG::COLLISION"<<endl;
                            ammoPool[i].Fired = false;
                            it = obstacles.erase(it);
                            break;
                        }
                        else{
                            it++;
                        }
                    }
                    
                    if(ammoPool[i].Fired){
                        ammoPool[i].Pos.x += ammoPool[i].Speed.x*dt;
                    }
                }
            }
            isShoot = false;

            if(done){
                // ----- DEBUG ----- //
                if(currentFrames > highScoreFrames){
                    highScoreFrames = currentFrames;
                    maxSurvivalTime = currentSurvivalTime;
                }

                currentFrames = 0.0f;
                currentSurvivalTime = 0.0f;

                prevReward = -100.0f;
                playerPos = glm::vec3(0.0f);
                prevDone = done;
                speedY = 0.0f;
                isGrounded = true;
                obstacles.clear();

                done = false;
            }
            accTime -= dt;
        }
        // ----- DEBUG ----- //
        // cout<<endl;
        // cout<<"REWARD:"<<reward<<" "<<"RESET:"<<done<<endl;
        // cout<<endl;
        
        // rexxy
        rexxyShader.use();
        glm::mat4 rexxy = glm::mat4(1.0f);
        rexxy = glm::translate(rexxy,glm::vec3(0.0,playerPos.y,0.0f));
        rexxy = glm::scale(rexxy,glm::vec3(0.5f));
        initModel(rexxyShader,rexxy,view,projection,lightColor,lightPosView);
        rexxyMesh.draw();
        
        rexxyShader.use();
        for(auto it:obstacles){
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model,it.Pos);
            model = glm::scale(model,it.Size);
            model = glm::rotate(model,(float)(sin(glfwGetTime())),glm::vec3(0.0f,1.0f,0.0f));
            initModel(rexxyShader,model,view,projection,lightColor,lightPosView);
            pyramid.draw();
        }
        
        ammoShader.use();
        for(int i=ammoPool.size()-1; i>=0; i--){
            if(ammoPool[i].Fired == true){
                glm::mat4 ammoMesh = glm::mat4(1.0f);
                ammoMesh = glm::translate(ammoMesh,glm::vec3(ammoPool[i].Pos.x,0.0f,0.0f));
                ammoMesh = glm::scale(ammoMesh,glm::vec3(ammoPool[i].Size));
                initModel(ammoShader,ammoMesh,view,projection,lightColor,lightPosView);
                bullet.draw();
            }
        }

        // ----- GUI ----- //
        dashboard.Render(currentFrames, currentSurvivalTime, highScoreFrames, maxSurvivalTime, camera.Position, ammoLeft);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    dashboard.Shutdown();
    glfwTerminate();
    closeSocket(sock);
    return 0;
}