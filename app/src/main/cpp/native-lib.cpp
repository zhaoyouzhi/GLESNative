#include <jni.h>
#include <string>
#include <vector>

#include "util/EGLUtil.h"
//#include "util/OBJ_Loader.h"
#include "glm/mat4x4.hpp"
#include "glm/ext.hpp"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc

#include "util/tiny_obj_loader.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb/stb_image.h"


typedef struct {
    // Handle to a program object
    GLuint programObject;

    // Uniform locations
    GLint mvpLoc;
    GLint samplerLoc;

    // Vertex daata
    GLfloat *vertices;
    GLfloat *textures;
    int numIndices;

    // Rotation angle
    GLfloat angle;

    GLuint textureId;

    // VertexArrayObject Id
    GLuint vaoId;
// VertexBufferObject Ids
    GLuint vboIds[1];
//    std::vector<std::vector<float>> vertexArray;
//    std::vector<int> numArray;
    // MVP matrix
    glm::mat4 mvpMatrix;

} UserData;

std::vector<std::vector<float>> vertexArray;
std::vector<int> numArray;

std::vector<float> vertexArrays;
std::vector<float> textureArrays;
int num;

GLuint createTexture(const char *filename) {
    int w, h;
    GLint iChannel = 0;
    FILE *pFile = fopen(filename, "rb");
    unsigned char *pImageData = stbi_load_from_file(pFile, &w, &h, &iChannel, 0);


    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (glIsTexture(texture) == GL_FALSE) {
        ALOGE("Is not currently the name of texture.Or some error occurs.Please check.");
        return 0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImageData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    free(pImageData);

    ALOGE("enter into CreateTextureFromPng5");
    return texture;
}

/**
 * 初始化
 * @param esContext
 * @return
 */
int init(ESContext *esContext) {
//    EGLInit(esContext);
    UserData *userData = (UserData *) esContext->userData;

    char *vertexStr = readAssetFile("vertex_dz.glsl",
                                    esContext->activity->assetManager);
    char *fragmentStr = readAssetFile("fragment_dz.glsl",
                                      esContext->activity->assetManager);

    // Load the shaders and get a linked program object
    userData->programObject = loadProgram(vertexStr, fragmentStr);
    if (userData->programObject == 0) {
        return GL_FALSE;
    }

    // Exit the program
    std::string inputfile = "/sdcard/opengles/lbt_obj.obj";

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());
    if (!err.empty()) { // `err` may contain warning message.
        return 0;
    }

    if (!ret) {
        return 0;
    }
    float scaleFactor = 1.0f;
    float largest = tinyobj::dimensions_t.getLargest();
    if (largest != 0.0f)
        scaleFactor = (1.0f / largest);
    // get the model's center point
    tinyobj::Vector3 center = tinyobj::dimensions_t.getCenter();
// Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        std::vector<float> ver;
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
//                tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
//                tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
//                tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
//                // Optional: vertex colors
//                tinyobj::real_t red = attrib.colors[3 * idx.vertex_index + 0];
//                tinyobj::real_t green = attrib.colors[3 * idx.vertex_index + 1];
//                tinyobj::real_t blue = attrib.colors[3 * idx.vertex_index + 2];

//                ver.push_back((vx - center.X) * scaleFactor);
//                ver.push_back((vy - center.Y) * scaleFactor);
//                ver.push_back((vz - center.Z) * scaleFactor);
                vertexArrays.push_back((vx - center.X) * scaleFactor);
                vertexArrays.push_back((vy - center.Y) * scaleFactor);
                vertexArrays.push_back((vz - center.Z) * scaleFactor);

                textureArrays.push_back(tx);
                textureArrays.push_back(ty);
//                vertices[3 * i + 0] = (vx- center.X) * scaleFactor;
//                vertices[3 * i + 1] = (vy - center.Y) * scaleFactor;
//                vertices[3 * i + 2] = (vz - center.Z) * scaleFactor;
            }
            index_offset += fv;
            num += fv;
            // per-face material
            shapes[s].mesh.material_ids[f];
        }
        vertexArray.push_back(ver);
        numArray.push_back(index_offset);
    }

    num = vertexArrays.size() / 3;

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(1, userData->vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, vertexArrays.size() * sizeof(float), &vertexArrays[0],
                 GL_STATIC_DRAW);

    vector<float>().swap(vertexArrays);

    // Bind the VAO and then setup the vertex
    // Generate VAO Id
    glGenVertexArrays(1, &userData->vaoId);
    // Bind the VAO and then setup the vertex
    // attributes
    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3,
                          GL_FLOAT, GL_FALSE, 0, (const void *) 0);
    // Reset to the default VAO
    glBindVertexArray(0);

    // Get the uniform locations
    userData->mvpLoc = glGetUniformLocation(userData->programObject, "u_mvpMatrix");
    userData->samplerLoc = glGetUniformLocation(userData->programObject, "s_Texture");

    // Load the texture
    userData->textureId = createTexture("/sdcard/opengles/LBT_LP_UV_A_01.jpg");

    // Starting rotation angle for the cube
    userData->angle = 45.0f;


    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    return 1;
}

///
// Update MVP matrix based on time
//
void Update(ESContext *esContext, float deltaTime) {
    UserData *userData = (UserData *) esContext->userData;
    float aspect;

    // Compute a rotation angle based on time to rotate the cube
    userData->angle += (deltaTime * 40.0f);

    if (userData->angle >= 360.0f) {
        userData->angle -= 360.0f;
    }

    // Compute the window aspect ratio
    aspect = (GLfloat) esContext->width / (GLfloat) esContext->height;

    // Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    // Camera matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 6), // Camera is at (4,3,3), in World Space
            glm::vec3(0, 0, -1), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);

    Model = glm::scale(Model, glm::vec3(2.0f, 2.0f, 2.0f));
    Model = glm::rotate(Model, glm::radians(userData->angle), glm::vec3(0, 1, 0));
    // Our ModelViewProjection : multiplication of our 3 matrices
    userData->mvpMatrix =
            Projection * View * Model; // Remember, matrix multiplication is the other way around

}

void onDraw(ESContext *esContext) {
    UserData *userData = (UserData *) esContext->userData;
    // Set the viewport
    glViewport(0, 0, esContext->width, esContext->height);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object
    glUseProgram(userData->programObject);

//    for (int i = 0; i < vertexArray.size(); ++i) {
    // Load the vertex position
//        glVertexAttribPointer(0, 3, GL_FLOAT,
//                              GL_FALSE, 0, &vertexArrays[0]);
//
//        // Load the texture coordinate

//
//        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->textureId);
    // Set the sampler texture unit to 0
    glUniform1i(userData->samplerLoc, 0);

    glVertexAttribPointer(1, 2, GL_FLOAT,
                          GL_FALSE, 0, &textureArrays[0]);

    // Load the MVP matrix
    glUniformMatrix4fv(userData->mvpLoc, 1, GL_FALSE, (GLfloat *) &userData->mvpMatrix[0][0]);

    glBindVertexArray(userData->vaoId);
    // Bind the VAO
    glDrawArrays(GL_TRIANGLES, 0, num / 4);

    // Return to the default VAO
    glBindVertexArray(0);
//    }

}


void shutDown(ESContext *esContext) {
    UserData *userData = (UserData *) esContext->userData;
    if (userData->vertices != NULL) {
        free(userData->vertices);
    }

//    if (userData->textures != NULL) {
//        free(userData->textures);
//    }

    // Delete program object
    glDeleteProgram(userData->programObject);
}

//int32_t OnTouch(ESContext *esContext, AInputEvent *event) {
//    ALOGE("OnTouch");
//    int32_t lEventType = AInputEvent_getType(event);
//
//    //知道来源与那里的时候，在判断具体是什么消息
//    int32_t id = AMotionEvent_getAction(event);
//    switch(id)
//    {
////触摸移动消息
//        case AMOTION_EVENT_ACTION_MOVE:
//
//            break;
////触摸按下消息
//        case AMOTION_EVENT_ACTION_DOWN:
//            break;
//           // 触摸弹起消息
//        case AMOTION_EVENT_ACTION_UP:
//
//            break;
//    }
//
//    return 1;
//}

int onCreate(ESContext *esContext) {
    esContext->userData = malloc(sizeof(UserData));
    createWindow(esContext, "Demo", 320, 240, ES_WINDOW_RGB);
    if (!init(esContext)) {
        ALOGE("Cannot init!");
        return GL_FALSE;
    }
//    registerShutdownFunc(esContext, shutDown);
    registerDrawFunc(esContext, onDraw);
    registerUpdateFunc(esContext, Update);
//    registerOnTouchFunc(esContext, OnTouch);
    return GL_TRUE;
}