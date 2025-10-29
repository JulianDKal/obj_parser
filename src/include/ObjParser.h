#pragma once
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include "GL/glew.h"
#include "Shader.h"
#include "Helpers.h"
#include <unordered_map>

enum errors {
    ERR_PATH_NOT_VALID = 1,
    ERR_NOT_AN_OBJ_FILE = 2,
    ERR_PARSING_ERROR = 3
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    Vertex() : position(0.0f), normal(0.0f), uv(0.0f) {}

    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};

struct Material {
    glm::vec3 colorAmbient;
    glm::vec3 colorDiffuse;
    glm::vec3 colorSpecular;
    float specExponent;
    bool useTexture = false;

    Shader shader;
    Texture texture;

    Material() : colorAmbient(0.0f), colorDiffuse(0.0f), colorSpecular(0.0f), specExponent(1.0f) {
        shader.createProgram("./../src/shaders/teapot_vertex.glsl", "./../src/shaders/teapot_fragment.glsl");
    }
};

struct Object {

    //Vertex Data
    std::vector<glm::vec3> positions;
    std::vector<unsigned int> indices; //Final indices that the Index Buffer Object uses
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;

    float scale = 1;
    std::string objName;
    Material mat;

    //OpenGL Handlers
    unsigned int vbo = 0, vao = 0, ebo = 0, normalsVBO = 0, uvVBO = 0;

    void DrawObject(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
        getErrorCode();
        glUseProgram(mat.shader.ID);
        mat.shader.setFloat("scale", 1.0f);
        mat.shader.setBool("useTexture", mat.useTexture);
        mat.shader.setMatrix4("model", model);
        mat.shader.setMatrix4("view", view);
        mat.shader.setMatrix4("projection", projection);
        mat.shader.setVec3f("lightColor", glm::vec3(1.0f));
        mat.shader.setVec3f("lightPos", glm::vec3(5.0f));
        mat.shader.useTexture(mat.texture, "tex");
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0 );

        getErrorCode();
    }

    void clearObjectData() {
        positions.clear();
        indices.clear();
        normals.clear();
        objName.clear();

        if (vao) glDeleteVertexArrays(1, &vao);
        if (vbo) glDeleteBuffers(1, &vbo);
        if (ebo) glDeleteBuffers(1, &ebo);
        if(uvVBO) glDeleteBuffers(1, &uvVBO);
        if (normalsVBO) glDeleteBuffers(1, &normalsVBO);

        vao = vbo = ebo = normalsVBO = 0;
    }

};

struct VertexHash {
    std::size_t operator()(const Vertex& v) const {
        std::size_t h1 = std::hash<float>{}(v.position.x);
        std::size_t h2 = std::hash<float>{}(v.position.y);
        std::size_t h3 = std::hash<float>{}(v.position.z);
        std::size_t h4 = std::hash<float>{}(v.normal.x);
        std::size_t h5 = std::hash<float>{}(v.normal.y);
        std::size_t h6 = std::hash<float>{}(v.normal.z);
        
        // Combine hashes (this is a simple method)
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ 
               (h5 << 4) ^ (h6 << 5);
    }
};

class ObjParser
{
private:

    //Indices used for processing and constructing the final VBOs and EBO
    std::vector<unsigned int> normalIndices;
    std::vector<unsigned int> positionIndices;
    std::vector<unsigned int> uvIndices;
    std::vector<glm::vec3> unIndexedNormals; //Stream of normals as read from the file
    std::vector<glm::vec2> unIndexedUVs;

    std::unordered_map<Vertex, unsigned int, VertexHash> verticesMap;
    std::vector<Vertex> vertices;

    //TODO: Make this a vector of objects that can be drawn
    Object obj;

    void clearData();
    void parseMtl(std::filesystem::path path);
public:
    void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
    Texture& getTexture();
    const std::string& getObjName();
    int Parse(std::filesystem::path path);
    ObjParser();
    ~ObjParser();
};


