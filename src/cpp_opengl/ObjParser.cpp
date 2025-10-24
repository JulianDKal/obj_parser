#include "ObjParser.h"

void ObjParser::clearData()
{
    positions.clear();
    indices.clear();
    positionIndices.clear();
    normalIndices.clear();
    normals.clear();
    unIndexedNormals.clear();
    verticesMap.clear();
    vertices.clear();

    objName.clear();

    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
    if(uvVBO) glDeleteBuffers(1, &uvVBO);
    if (normalsVBO) glDeleteBuffers(1, &normalsVBO);

    vao = vbo = ebo = normalsVBO = 0;
}

void ObjParser::parseMtl(std::filesystem::path path)
{
    if(!std::filesystem::exists(path)) {
        std::cerr << "The path to the .mtl file does not exist: " << path << std::endl;
        return;
    }
    std::ifstream file;
    std::string currentLine;

    file.open(path);
    if(!file.is_open()) {
        std::cerr << "Error opening file for obj parsing!" << std::endl;
        return;
    }

    while (std::getline(file, currentLine))
    {
        std::stringstream lineStream(currentLine);
        std::string firstToken;

        lineStream >> firstToken;
        if(firstToken == "#" || firstToken == ""){
            continue;
        }
        //Ambient Color
        else if(firstToken == "Ka"){
            lineStream >> mat.colorAmbient.r >> mat.colorAmbient.g >> mat.colorAmbient.b;
        }
        //Diffuse color
        else if(firstToken == "Kd"){
            lineStream >> mat.colorDiffuse.r >> mat.colorDiffuse.g >> mat.colorDiffuse.b;
        }
        //Diffuse map
        else if(firstToken == "map_Kd") {
            lineStream >> std::ws;
            std::string textureName;
            std::getline(lineStream, textureName);


            //TODO: Maybe add support for texture locations that are in another directory 
            std::string textureLocation = path.remove_filename().append(textureName);
            std::cout << textureLocation << std::endl;

            mat.texture.initialize(textureLocation, 0);
            useTexture = true;
        }
        else if(firstToken == "Ks"){
            lineStream >> mat.colorSpecular.r >> mat.colorSpecular.g >> mat.colorSpecular.b;
        }
        else if(firstToken == "Ns") {
            lineStream >> mat.specExponent;
        }
        else if(firstToken == "Ni") continue; //The optical density of the surface, also known as index of refraction
        else if(firstToken == "d") continue; //Translucency of the object (1 means fully opaque)
        else if(firstToken == "Tr") continue; //Translucency of the object (1 means fully translucent, Tr = 1 - d)
    }
    
}

void ObjParser::Draw()
{
    getErrorCode();

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0 );

    getErrorCode();
}

Texture &ObjParser::getTexture()
{
    return mat.texture;
}

bool ObjParser::isUsingTexture()
{
    return useTexture;
}

const std::string &ObjParser::getObjName()
{
    return objName;
}

int ObjParser::Parse(std::filesystem::path path)
{
    std::ifstream file;

    if(!std::filesystem::exists(path)) return ERR_PATH_NOT_VALID;
    if(std::filesystem::path(path).extension() != ".obj") return ERR_NOT_AN_OBJ_FILE;

    file.open(path);
    if(!file.is_open()) {
        std::cerr << "Error opening file for obj parsing!" << std::endl;
        return ERR_PARSING_ERROR;
    }

    if(positions.size() != 0) clearData();
    float v1, v2, v3, vt1, vt2; //vertex 1 2 3 and uv 1 2 of each line
    unsigned int i1, i2, i3, i4, vn, vt;
    float n1, n2, n3;
    char sep, sep2; //used only for writing slashes into it with the stringstream
    std::string line;

    bool normalsIncluded = false;
    bool uvsIncluded = false;
    useTexture = false;

    int counter = 0;

    while (std::getline(file, line))
    {
        std::stringstream lineStream(line);

        //read vertex data
        if(lineStream.peek() == 'v') { 
            lineStream.get(); //consume the v
            //process normals
            if(lineStream.peek() == 'n') {
                lineStream.get();
                normalsIncluded = true;
                lineStream >> std::ws >> n1 >> n2 >> n3;
                unIndexedNormals.push_back(glm::vec3(0.0f));
                unIndexedNormals.at(unIndexedNormals.size() - 1).x = n1;
                unIndexedNormals.at(unIndexedNormals.size() - 1).y = n2;
                unIndexedNormals.at(unIndexedNormals.size() - 1).z = n3;
            }
            else if(lineStream.peek() == 't') {
                lineStream.get();
                uvsIncluded = true;
                lineStream >> std::ws >> vt1 >> vt2;
                unIndexedUVs.push_back(glm::vec3(0.0f));
                unIndexedUVs.back().x = vt1;
                unIndexedUVs.back().y = vt2;
            }
            //process normal vertices
            else {
                lineStream >> std::ws >> v1 >> v2 >> v3;
                positions.push_back(glm::vec3(0.0f));
                positions.at(positions.size() - 1).x = v1;
                positions.at(positions.size() - 1).y = v2;
                positions.at(positions.size() - 1).z = v3;
                
                scale = std::max(scale, v1);
                scale = std::max(scale, v2);
                scale = std::max(scale, v3);
            }
        }
        //read index data
        else if(lineStream.peek() == 'f') {
            lineStream.get();  //get f
            lineStream >> std::ws;
            if(normalsIncluded || uvsIncluded) {
                //need to process differently, because format is now f 1/2/3 4/5/6 etc.
                    std::string restOfLine;
                    std::vector<unsigned int> currentFacePositions;
                    std::vector<unsigned int> currentFaceNormals;
                    std::vector<unsigned int> currentFaceUVs;

                    while (std::getline(lineStream, restOfLine, ' ')) {
                        std::istringstream tokenizer(restOfLine);
                        std::string vPart, vtPart, vnPart;
                        
                        std::getline(tokenizer, vPart, '/');
                        std::getline(tokenizer, vtPart, '/');
                        std::getline(tokenizer, vnPart, '/');
 
                        int v = std::stoi(vPart) - 1;
                        int vt = vtPart.empty() ? -1 : std::stoi(vtPart) - 1;
                        int vn = vnPart.empty() ? -1 : std::stoi(vnPart) - 1;
                        
                        currentFacePositions.push_back(v);
                        currentFaceNormals.push_back(vn);
                        currentFaceUVs.push_back(vt);
                    }

                    //triangulate the face if it has more than three vertices
                    if(currentFacePositions.size() > 3){
                        for (size_t i = 1; i < currentFacePositions.size() - 1; ++i) {
                            positionIndices.push_back(currentFacePositions[0]);
                            positionIndices.push_back(currentFacePositions[i]);
                            positionIndices.push_back(currentFacePositions[i + 1]);

                            normalIndices.push_back(currentFaceNormals[0]);
                            normalIndices.push_back(currentFaceNormals[i]);
                            normalIndices.push_back(currentFaceNormals[i + 1]);

                            uvIndices.push_back(currentFaceUVs[0]);
                            uvIndices.push_back(currentFaceUVs[i]);
                            uvIndices.push_back(currentFaceUVs[i + 1]);
                        }
                    }
                    else {
                        for(size_t i = 0; i < currentFacePositions.size(); i++) {
                            positionIndices.push_back(currentFacePositions[i]);
                            normalIndices.push_back(currentFaceNormals[i]);
                            uvIndices.push_back(currentFaceUVs[i]);
                        }
                    }
            }
            else {
                lineStream >> std::ws >> i1 >> i2 >> i3 >> i4;

                indices.push_back(i1 - 1); //decrement by 1 because openGL indexing starts at 0, but obj indexing at 1
                indices.push_back(i2 - 1);
                indices.push_back(i3 - 1);
                if(i4) indices.push_back(i4 - 1);
            }
        }
        else if(lineStream.peek() == '#') {
            continue;
        }

        else if(lineStream.peek() == 'm') {
            std::string s, mtlLocation;
            lineStream >> s;
            if(s == "mtllib") {
                mtlLocation = path.remove_filename(); //.mtl file is assumed to be in the same directory as the .obj file
                lineStream >> std::ws;
                std::getline(lineStream, s);
                mtlLocation.append(s);
                std::cout << "mtl path: " << mtlLocation << std::endl;

                parseMtl(mtlLocation);
            }
        }

        //TODO: add support for multiple objects inside one file
        //Get the object name
        else if(lineStream.peek() == 'o') {
            lineStream.get();
            std::string name;
            lineStream >> std::ws >> name;
            objName = name;
        }

        //TODO: Add support for smooth shading flag (s = 1 or 0 )
    }

    std::cout << "positionIndices size: " << positionIndices.size() << std::endl;
    std::cout << "normalIndices size: " << normalIndices.size() << std::endl;
    std::cout << "uvIndices size: " << uvIndices.size() << std::endl;

    // verify that position, normal and uv index arrays have the same size
    // if (positionIndices.size() != normalIndices.size() || positionIndices.size() != uvIndices.size()) {
    //     return ERR_PARSING_ERROR;
    // }

    if(normalsIncluded) {
        //construct the arrays of indices and vertices
        //using a hashmap to reuse duplicate vertices
        for (int i = 0; i < positionIndices.size(); i++)
        {
            Vertex vertex;
            vertex.normal = unIndexedNormals[normalIndices[i]];
            vertex.position = positions[positionIndices[i]];
            vertex.uv = unIndexedUVs[uvIndices[i]];

            auto it = verticesMap.find(vertex);
            if(it != verticesMap.end()) { //Vertex already exists in this exact constellation
                indices.push_back(it->second);
            }
            else {
                unsigned int newIndex = vertices.size();
                vertices.push_back(vertex);
                verticesMap[vertex] = newIndex;
                indices.push_back(newIndex);
            }
        }

        positions.resize(vertices.size());
        normals.resize(vertices.size());
        uvs.resize(vertices.size());
        
        for(int i = 0; i < vertices.size(); i++){
            positions[i] = vertices[i].position;
            normals[i] = vertices[i].normal;
            uvs[i] = vertices[i].uv;
        }
    }    

    else {
        //calculate the normals based on the vertex data
        normals = std::vector<glm::vec3>(positions.size(), glm::vec3(0.0f));
        for (size_t i = 0; i < indices.size(); i += 3) {
        glm::vec3 v0 = positions[indices[i]];
        glm::vec3 v1 = positions[indices[i+1]];
        glm::vec3 v2 = positions[indices[i+2]];
    
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));
    
        normals[indices[i]] += faceNormal;
        normals[indices[i+1]] += faceNormal;    
        normals[indices[i+2]] += faceNormal;
        }
    }

    std::cout << "normals: " << normals.size() << " positions: " << positions.size() << " UVs: " << uvs.size() << " indices: " << indices.size() << std::endl;   
    
    if(objName == ""){
        objName = path.filename();
    }
    std::cout << "Object name: " << objName << std::endl; 

    //OpenGL Setup
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &normalsVBO);
    glGenBuffers(1, &uvVBO);
    getErrorCode();

    glBindVertexArray(vao);

    // === Vertex buffer (positions) ===
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * positions.size(), (float*)positions.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    getErrorCode();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * normals.size(), (float*)normals.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    getErrorCode();

    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size() * 2, (float*)uvs.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    getErrorCode();
    return 0;

}

ObjParser::ObjParser()
{
}

ObjParser::~ObjParser()
{
}