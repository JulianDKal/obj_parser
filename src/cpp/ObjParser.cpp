#include "ObjParser.h"

void ObjParser::clearData()
{
    positionIndices.clear();
    normalIndices.clear();   
    unIndexedNormals.clear();
    verticesMap.clear();
    vertices.clear();
    uvIndices.clear();
    unIndexedUVs.clear();

    obj.clearObjectData();
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
            lineStream >> obj.mat.colorAmbient.r >> obj.mat.colorAmbient.g >> obj.mat.colorAmbient.b;
        }
        //Diffuse color
        else if(firstToken == "Kd"){
            lineStream >> obj.mat.colorDiffuse.r >> obj.mat.colorDiffuse.g >> obj.mat.colorDiffuse.b;
        }
        //Diffuse map
        else if(firstToken == "map_Kd") {
            lineStream >> std::ws;
            std::string textureName;
            std::getline(lineStream, textureName);

            //TODO: Maybe add support for texture locations that are in another directory 
            std::string textureLocation = path.remove_filename().append(textureName);
            std::cout << textureLocation << std::endl;
            Texture newTexture;
            if(newTexture.initialize(textureLocation, 0)){
                obj.mat.texture = newTexture;
                obj.mat.useTexture = true;
            }

        }
        else if(firstToken == "Ks"){
            lineStream >> obj.mat.colorSpecular.r >> obj.mat.colorSpecular.g >> obj.mat.colorSpecular.b;
        }
        else if(firstToken == "Ns") {
            lineStream >> obj.mat.specExponent;
        }
        else if(firstToken == "Ni") continue; //The optical density of the surface, also known as index of refraction
        else if(firstToken == "d") continue; //Translucency of the object (1 means fully opaque)
        else if(firstToken == "Tr") continue; //Translucency of the object (1 means fully translucent, Tr = 1 - d)
    }
    
}

void ObjParser::Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
    obj.DrawObject(model, view, projection);
}

Texture &ObjParser::getTexture()
{
    return obj.mat.texture;
}


const std::string &ObjParser::getObjName()
{
    return obj.objName;
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

    if(obj.positions.size() != 0) clearData();

    float v1, v2, v3, vt1, vt2; //vertex 1 2 3 and uv 1 2 of each line
    unsigned int i1, i2, i3, i4, vn, vt;
    float n1, n2, n3;
    char sep, sep2; //used only for writing slashes into it with the stringstream
    std::string line;

    bool normalsIncluded = false;
    bool uvsIncluded = false;
    obj.mat.useTexture = false;

    int counter = 0;

    while (std::getline(file, line))
    {
        std::stringstream lineStream(line);

        //read vertex data
        if(lineStream.peek() == 'v') { 
            lineStream.get(); //consume the v
            //process obj.normals
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
                unIndexedUVs.push_back(glm::vec2(0.0f));
                unIndexedUVs.back().x = vt1;
                unIndexedUVs.back().y = vt2;
            }
            //process normal vertices
            else {
                lineStream >> std::ws >> v1 >> v2 >> v3;
                obj.positions.push_back(glm::vec3(0.0f));
                obj.positions.at(obj.positions.size() - 1).x = v1;
                obj.positions.at(obj.positions.size() - 1).y = v2;
                obj.positions.at(obj.positions.size() - 1).z = v3;
                
                obj.scale = std::max(obj.scale, v1);
                obj.scale = std::max(obj.scale, v2);
                obj.scale = std::max(obj.scale, v3);
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
                int index;
                while (lineStream >> index)
                {
                    obj.indices.push_back(index - 1);
                }
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
            obj.objName = name;
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

    //TODO: This is potentially dangerous if there are normals included but not UVS
    if(normalsIncluded) {
        //construct the arrays of indices and vertices
        //using a hashmap to reuse duplicate vertices
        for (int i = 0; i < positionIndices.size(); i++)
        {
            Vertex vertex;
            vertex.normal = unIndexedNormals[normalIndices[i]];
            vertex.position = obj.positions[positionIndices[i]];
            vertex.uv = unIndexedUVs[uvIndices[i]];

            auto it = verticesMap.find(vertex);
            if(it != verticesMap.end()) { //Vertex already exists in this exact constellation
                obj.indices.push_back(it->second);
            }
            else {
                unsigned int newIndex = vertices.size();
                vertices.push_back(vertex);
                verticesMap[vertex] = newIndex;
                obj.indices.push_back(newIndex);
            }
        }

        obj.positions.resize(vertices.size());
        obj.normals.resize(vertices.size());
        obj.uvs.resize(vertices.size());
        
        for(int i = 0; i < vertices.size(); i++){
            obj.positions[i] = vertices[i].position;
            obj.normals[i] = vertices[i].normal;
            obj.uvs[i] = vertices[i].uv;
        }
    }    

    else {
        std::cout << "Calculating normals based on vertex data" << std::endl;
        std::cout << "Vertices without normals - Positions size: " << obj.positions.size() << " Indices size: " << obj.indices.size() << std::endl;
        
        //calculate the obj.normals based on the vertex data
        obj.normals = std::vector<glm::vec3>(obj.positions.size(), glm::vec3(0.0f));
        std::cout << "normals size: " << obj.normals.size() << std::endl;
        for (size_t i = 0; i < obj.indices.size(); i += 3) {
        glm::vec3 v0 = obj.positions[obj.indices[i]];
        glm::vec3 v1 = obj.positions[obj.indices[i+1]];
        glm::vec3 v2 = obj.positions[obj.indices[i+2]];
    
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));
    
        obj.normals[obj.indices[i]] += faceNormal;
        obj.normals[obj.indices[i+1]] += faceNormal;    
        obj.normals[obj.indices[i+2]] += faceNormal;
        }
    }

    std::cout << "obj.normals: " << obj.normals.size() << " obj.positions: " << obj.positions.size() << " UVs: " << obj.uvs.size() << " obj.indices: " << obj.indices.size() << std::endl;   
    
    if(obj.objName == ""){
        obj.objName = path.filename();
    }
    std::cout << "Object name: " << obj.objName << std::endl; 

    //OpenGL Setup
    glGenVertexArrays(1, &obj.vao);
    glGenBuffers(1, &obj.vbo);
    glGenBuffers(1, &obj.ebo);
    glGenBuffers(1, &obj.normalsVBO);
    glGenBuffers(1, &obj.uvVBO);
    getErrorCode();

    glBindVertexArray(obj.vao);

    // === Vertex buffer (obj.positions) ===
    glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * obj.positions.size(), (float*)obj.positions.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    getErrorCode();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * obj.indices.size(), obj.indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, obj.normalsVBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * obj.normals.size(), (float*)obj.normals.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    getErrorCode();

    glBindBuffer(GL_ARRAY_BUFFER, obj.uvVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * obj.uvs.size() * 2, (float*)obj.uvs.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    getErrorCode();
    
    std::cout << "useTexture: " << obj.mat.useTexture << std::endl;
    return 0;

}

ObjParser::ObjParser()
{
}

ObjParser::~ObjParser()
{
}