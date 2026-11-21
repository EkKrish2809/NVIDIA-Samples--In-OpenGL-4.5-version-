#ifndef MODEL_PBR_H
#define MODEL_PBR_H

#include <memory>

#include "model.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"
#include "../../utils/OpenGL/Framebuffer.h"
#include "../../utils/Camera.h"
// #include "../shadow/Shadow.h"
// #ifndef STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
// #endif
#include "../../../includes/stb_image.h"

extern Camera camera;

class ModelPbr
{
public:
    ModelPbr(string const &path, unsigned int drawMode = GL_TRIANGLES)
    {
        m_drawMode = drawMode;
        // model_ = std::make_unique<Model>(path, drawMode);

        // instance vbo
       /* amount = 100;
        glm::mat4 *modelMatrices;
        modelMatrices = new glm::mat4[amount];
        srand(static_cast<unsigned int>(1)); // initialize random seed
        float radius = 3.0;
        float offset = 15.5f;
        for (unsigned int i = 0; i < amount; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            // 1. translation: displace along circle with 'radius' in range [-offset, offset]
            float angle = (float)i / (float)amount * 360.0f;
            // float angle = 0.0f;
            float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float x = sin(angle) * radius + displacement;
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float z = cos(angle) * radius + displacement;
            // model = glm::translate(model, glm::vec3(x, y, z));
            model = glm::translate(model, glm::vec3(x, 0.0f, z));

            // 2. scale: Scale between 0.05 and 0.25f
            float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
            model = glm::scale(model, glm::vec3(scale));

            // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
            float rotAngle = static_cast<float>((rand() % 360));
            // model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
            // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            // 4. now add to list of matrices
            modelMatrices[i] = model;
        }

        // configure instanced array
        // -------------------------
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
        // glBufferData(GL_ARRAY_BUFFER, ModelMatrix.size() * sizeof(glm::mat4), ModelMatrix.data(), GL_STATIC_DRAW);
        */
        // set transformation matrices as an instance vertex attribute (with divisor 1)
        // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
        // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
        // -----------------------------------------------------------------------------------------------------------------------------------
        // reloadVertexBuffers(model_);

        // initialize();
        initializeAdvancedPBR();
    }

    ~ModelPbr() {}

    int initialize();
    int initializeAdvancedPBR();

    void fboStuff();

    void render();
    void renderPBR();
    void resize(int width, int height);
    void uninitialize();

    void renderSphere();
    void renderCube();
    void renderQuad();

private:
    void reloadVertexBuffers(const std::unique_ptr<Model>& model)
    {
         for (unsigned int i = 0; i < model->meshes.size(); i++)
        {
            unsigned int VAO = model->meshes[i].VAO;
            glBindVertexArray(VAO);
            // set attribute pointers for matrix (4 times vec4)
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)0);
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(sizeof(glm::vec4)));
            glEnableVertexAttribArray(7);
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(2 * sizeof(glm::vec4)));
            glEnableVertexAttribArray(8);
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(3 * sizeof(glm::vec4)));

            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);
            glVertexAttribDivisor(7, 1);
            glVertexAttribDivisor(8, 1);

        }
    }

private:
    std::unique_ptr<Model> model_ = nullptr;
    unsigned int m_drawMode = 0;

    int _width ;
    int _height ;

    GLuint u_modelShader_pbr;
    GLuint equirectangularToCubemapShader;
    GLuint irradianceShader;
    GLuint prefilterShader;
    GLuint brdfShader;
    GLuint backgroundShader;

    // uniforms for pbr
    GLuint u_modelMatrixUniform_pbr;
    GLuint u_viewMatrixUniform_pbr;
    GLuint u_projectionMatrixUniform_pbr;
    GLuint u_normalMatrix_pbr;
    GLuint u_albedoMap;
    GLuint u_normalMap;
    GLuint u_metallicMap;
    GLuint u_roughnessMap;
    GLuint u_aoMap;
    GLuint u_lightPositions0;
    GLuint u_lightPositions1;
    GLuint u_lightPositions2;
    GLuint u_lightPositions3;
    GLuint u_lightColors0;
    GLuint u_lightColors1;
    GLuint u_lightColors2;
    GLuint u_lightColors3;
    GLuint u_camPos;
    GLuint u_exposure;
    GLuint u_isInstanced;

    // background shader uniforms
    GLuint u_viewUniformBGR;
    GLuint u_projectionUniformBGR;

    // equirectangular
    GLuint u_viewUniformEQU;
    GLuint u_projectionUniformEQU;

    // irradiance 
    GLuint u_viewUniformIRD;
    GLuint u_projectionUniformIRD;

    // prefilter
    GLuint u_viewUniformPRF;
    GLuint u_projectionUniformPRF;

    unsigned int amount;
    unsigned int captureFBO;
    unsigned int captureRBO;
    glm::mat4 captureProjection;
    glm::mat4 captureViews[6];
    unsigned int hdrTexture;
    unsigned int envCubemap;
    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUTTexture;

    unsigned int sphereVAO = 0;
    GLsizei indexCount;
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;
    unsigned int quadVAO = 0;
    unsigned int quadVBO;

    GLuint texture_albedo;
    GLuint texture_ao;
    GLuint texture_metallic;
    GLuint texture_normal;
    GLuint texture_roughness;
};


#endif