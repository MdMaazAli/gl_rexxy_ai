#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <bits/stdc++.h>
using namespace std;
using namespace glm;

class Shader{
    public:
        unsigned int ID;
        Shader(const char* vertexShaderPath,const char* fragShaderPath){
            ifstream vertexShaderFile;
            ifstream fragShaderFile;
            string vertexCode;
            string fragCode;

            vertexShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
            fragShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
            try{
                stringstream vertexShaderStream;
                stringstream fragShaderStream;

                vertexShaderFile.open(vertexShaderPath);
                fragShaderFile.open(fragShaderPath);
                vertexShaderStream<<vertexShaderFile.rdbuf();
                fragShaderStream<<fragShaderFile.rdbuf();
                vertexShaderFile.close();
                fragShaderFile.close();

                vertexCode = vertexShaderStream.str();
                fragCode = fragShaderStream.str();
            }
            catch(ifstream::failure& e){
                cout<<"ERROR::SHADER::FILE_NOT_SUCCESSFULLY_OPENED"<<e.what()<<endl;
            }

            const char* vertexShaderSource = vertexCode.c_str();
            const char* fragShaderSource = fragCode.c_str();

            unsigned int vertexShader;
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader,1,&vertexShaderSource,0);
            glCompileShader(vertexShader);
            checkCompileError(vertexShader,"VERTEX");
            
            unsigned int fragShader;
            fragShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragShader,1,&fragShaderSource,0);
            glCompileShader(fragShader);
            checkCompileError(fragShader,"FRAGMENT");
            
            ID = glCreateProgram();
            glAttachShader(ID,vertexShader);
            glAttachShader(ID,fragShader);
            glLinkProgram(ID);
            checkCompileError(ID,"PROGRAM");
        }

        void use(){
            glUseProgram(ID);
        }

        void setBool(const string &name,bool value){
            glUniform1i(glGetUniformLocation(ID,name.c_str()),(int)value);
        }

        void setInt(const string &name,int value){
            glUniform1i(glGetUniformLocation(ID,name.c_str()),value);
        }

        void setFloat(const string &name,float value){
            glUniform1f(glGetUniformLocation(ID,name.c_str()),value);
        }

        void setVec2(const std::string &name, const glm::vec2 &value) const
        { 
            glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
        }
        
        void setVec2(const std::string &name, float x, float y) const
        { 
            glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
        }

        void setVec3(const std::string &name, const glm::vec3 &value) const
        { 
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
        }
        void setVec3(const std::string &name, float x, float y, float z) const
        { 
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
        }

        void setVec4(const std::string &name, const glm::vec4 &value) const
        { 
            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
        }
        void setVec4(const std::string &name, float x, float y, float z, float w) const
        { 
            glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
        }

        void setMat2(const std::string &name, const glm::mat2 &mat) const
        {
            glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        void setMat3(const std::string &name, const glm::mat3 &mat) const
        {
            glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        void setMat4(const std::string &name, const glm::mat4 &mat) const
        {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

    private:
        void checkCompileError(unsigned int shader,string type){
            int success;
            char infoLog[1024];
            if (type != "PROGRAM")
            {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
                }
            }
            else
            {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success)
                {
                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
                }
            }
        }
};