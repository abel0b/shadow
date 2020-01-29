#include "shadow/ResourcePack.hpp"
#include <iostream>
#include <sstream>
#include <fstream>


Mesh::Mesh() {

}


Mesh::Mesh(std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texcoords, std::vector<float>& colors) :
    vertices(vertices),
    normals(normals),
    texcoords(texcoords),
    colors(colors) {

}

ResourcePack::ResourcePack() : programs() {

}

GLuint ResourcePack::get_program(std::string& name) {
    if (programs.count(name)) {
        return programs[name];
    }
    else {
        std::cout << "shader not loaded " << name << std::endl;
        exit(1);
    }

}

Mesh * ResourcePack::get_mesh(std::string& name) {
    if (meshes.count(name)) {
        return &meshes[name];
    }
    else {
        std::cout << "mesh not loaded " << name << std::endl;
        exit(1);
    }
}

void ResourcePack::load_mesh(std::string& name) {
    std::ostringstream mesh_file_path_stream;
    mesh_file_path_stream << "../res/" << name << ".obj";
    std::string mesh_file_path = mesh_file_path_stream.str();
 
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string terr;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &terr, mesh_file_path.c_str());

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!terr.empty()) {
        std::cerr << terr << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    std::cout << attrib.vertices.size() << std::endl;
    Mesh mesh(attrib.vertices, attrib.normals, attrib.texcoords, attrib.colors);
    meshes[name] = mesh;

    std::cout << "mesh loaded" << std::endl;
}

void ResourcePack::load_shader(std::string& name) {
    std::ostringstream vertex_file_path_stream, fragment_file_path_stream;
    vertex_file_path_stream << "../res/" << name << ".vert";
    fragment_file_path_stream << "../res/" << name << ".frag";
   
    std::string vertex_file_path = vertex_file_path_stream.str();
    std::string fragment_file_path = fragment_file_path_stream.str();

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path.c_str(), std::ios::in);
    if(VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        printf("Could not open shader %s\n", vertex_file_path.c_str());
        getchar();
        exit(1);
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path.c_str(), std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path.c_str());
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path.c_str());
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    programs[name] = ProgramID;
}
