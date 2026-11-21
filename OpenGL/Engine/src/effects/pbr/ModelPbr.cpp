#include "ModelPbr.h"



int ModelPbr::initialize()
{
    // intialize shaders
    GLuint vertex_shader_object = CreateAndCompileShaderObjects("./src/shaders/model/pbr_vs2.glsl", VERTEX);
    GLuint fragment_shader_object = CreateAndCompileShaderObjects("./src/shaders/model/pbr_fs2.glsl", FRAGMENT);

    // shader program object
    u_modelShader_pbr = glCreateProgram();

    glAttachShader(u_modelShader_pbr, vertex_shader_object);
    glAttachShader(u_modelShader_pbr, fragment_shader_object);

    // prelinked binding
    // glBindAttribLocation(m_shaderProgramObject_waterQuad, MATRIX_ATTRIBUTE_POSITION, "a_position");
    glBindAttribLocation(u_modelShader_pbr, 0, "aPos");
    glBindAttribLocation(u_modelShader_pbr, 1, "aNormal");
    glBindAttribLocation(u_modelShader_pbr, 2, "aTexCoords");
    glBindAttribLocation(u_modelShader_pbr, 3, "aTangent");
    glBindAttribLocation(u_modelShader_pbr, 4, "aBitangent");
    glBindAttribLocation(u_modelShader_pbr, 5, "aInstanceMatrix");
    // glBindAttribLocation(u_modelShader_pbr, 4, "aBitangent");

    // linking shaderProgramObject
    Bool shader_link_status = LinkShaderProgramObject(u_modelShader_pbr);
    if (shader_link_status == False)
    {
        std::cout << "shader linking failed" << std::endl;
        return -1;
    }

    u_modelMatrixUniform_pbr = glGetUniformLocation(u_modelShader_pbr, "model");
    u_viewMatrixUniform_pbr = glGetUniformLocation(u_modelShader_pbr, "view");
    u_projectionMatrixUniform_pbr = glGetUniformLocation(u_modelShader_pbr, "projection");
    u_normalMatrix_pbr = glGetUniformLocation(u_modelShader_pbr, "normalMatrix");

    u_albedoMap = glGetUniformLocation(u_modelShader_pbr, "texture_diffuse");
    u_normalMap = glGetUniformLocation(u_modelShader_pbr, "texture_normal");
    u_metallicMap = glGetUniformLocation(u_modelShader_pbr, "metallicMap");
    u_roughnessMap = glGetUniformLocation(u_modelShader_pbr, "roughnessMap");
    u_aoMap = glGetUniformLocation(u_modelShader_pbr, "aoMap");

    u_lightPositions0 = glGetUniformLocation(u_modelShader_pbr, "lightPositions[0]");
    u_lightPositions1 = glGetUniformLocation(u_modelShader_pbr, "lightPositions[1]");
    u_lightPositions2 = glGetUniformLocation(u_modelShader_pbr, "lightPositions[2]");
    u_lightPositions3 = glGetUniformLocation(u_modelShader_pbr, "lightPositions[3]");
    u_lightColors0 = glGetUniformLocation(u_modelShader_pbr, "lightColors[0]");
    u_lightColors1 = glGetUniformLocation(u_modelShader_pbr, "lightColors[1]");
    u_lightColors2 = glGetUniformLocation(u_modelShader_pbr, "lightColors[2]");
    u_lightColors3 = glGetUniformLocation(u_modelShader_pbr, "lightColors[3]");
    u_camPos = glGetUniformLocation(u_modelShader_pbr, "camPos");
    u_exposure = glGetUniformLocation(u_modelShader_pbr, "exposure");
    u_isInstanced = glGetUniformLocation(u_modelShader_pbr, "isInstanced");

    return 0;
}

int ModelPbr::initializeAdvancedPBR()
{
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // intialize shaders
    GLuint vertex_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/pbr_vs.glsl", VERTEX);
    GLuint fragment_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/pbr_fs.glsl", FRAGMENT);

    // shader program object
    u_modelShader_pbr = glCreateProgram();

    glAttachShader(u_modelShader_pbr, vertex_shader_object);
    glAttachShader(u_modelShader_pbr, fragment_shader_object);

    // prelinked binding
    // glBindAttribLocation(m_shaderProgramObject_waterQuad, MATRIX_ATTRIBUTE_POSITION, "a_position");
    glBindAttribLocation(u_modelShader_pbr, 0, "aPos");
    glBindAttribLocation(u_modelShader_pbr, 1, "aNormal");
    glBindAttribLocation(u_modelShader_pbr, 2, "aTexCoords");
    glBindAttribLocation(u_modelShader_pbr, 3, "aTangent");
    glBindAttribLocation(u_modelShader_pbr, 4, "aBitangent");
    // glBindAttribLocation(u_modelShader_pbr, 5, "aInstanceMatrix");
    // glBindAttribLocation(u_modelShader_pbr, 4, "aBitangent");

    // linking shaderProgramObject
    Bool shader_link_status = LinkShaderProgramObject(u_modelShader_pbr);
    if (shader_link_status == False)
    {
        std::cout << "shader linking failed u_modelShader_pbr" << std::endl;
        return -1;
    }

    u_modelMatrixUniform_pbr = glGetUniformLocation(u_modelShader_pbr, "model");
    u_viewMatrixUniform_pbr = glGetUniformLocation(u_modelShader_pbr, "view");
    u_projectionMatrixUniform_pbr = glGetUniformLocation(u_modelShader_pbr, "projection");
    u_normalMatrix_pbr = glGetUniformLocation(u_modelShader_pbr, "normalMatrix");

    // u_albedoMap = glGetUniformLocation(u_modelShader_pbr, "texture_diffuse");
    // u_normalMap = glGetUniformLocation(u_modelShader_pbr, "texture_normal");
    // u_metallicMap = glGetUniformLocation(u_modelShader_pbr, "metallicMap");
    // u_roughnessMap = glGetUniformLocation(u_modelShader_pbr, "roughnessMap");
    // u_aoMap = glGetUniformLocation(u_modelShader_pbr, "aoMap");

    u_lightPositions0 = glGetUniformLocation(u_modelShader_pbr, "lightPositions[0]");
    u_lightPositions1 = glGetUniformLocation(u_modelShader_pbr, "lightPositions[1]");
    u_lightPositions2 = glGetUniformLocation(u_modelShader_pbr, "lightPositions[2]");
    u_lightPositions3 = glGetUniformLocation(u_modelShader_pbr, "lightPositions[3]");
    u_lightColors0 = glGetUniformLocation(u_modelShader_pbr, "lightColors[0]");
    u_lightColors1 = glGetUniformLocation(u_modelShader_pbr, "lightColors[1]");
    u_lightColors2 = glGetUniformLocation(u_modelShader_pbr, "lightColors[2]");
    u_lightColors3 = glGetUniformLocation(u_modelShader_pbr, "lightColors[3]");
    u_camPos = glGetUniformLocation(u_modelShader_pbr, "camPos");
    // u_exposure = glGetUniformLocation(u_modelShader_pbr, "exposure");
    // u_isInstanced = glGetUniformLocation(u_modelShader_pbr, "isInstanced");

    glUseProgram(u_modelShader_pbr);
    glUniform1i(glGetUniformLocation(u_modelShader_pbr, "irradianceMap"), 0);
    glUniform1i(glGetUniformLocation(u_modelShader_pbr, "prefilterMap"), 1);
    glUniform1i(glGetUniformLocation(u_modelShader_pbr, "brdfLUT"), 2);
    glUniform1i(glGetUniformLocation(u_modelShader_pbr, "texture_diffuse1"), 3);
    glUniform1i(glGetUniformLocation(u_modelShader_pbr, "texture_normal1"), 4);
    glUniform1i(glGetUniformLocation(u_modelShader_pbr, "texture_metallicMap1"), 5);
    glUniform1i(glGetUniformLocation(u_modelShader_pbr, "texture_roughnessMap1"), 6);
    glUniform1i(glGetUniformLocation(u_modelShader_pbr, "texture_aoMap1"), 7);
    glUseProgram(0);


    // ****************** background shader *********************

    vertex_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/background_vs.glsl", VERTEX);
    fragment_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/background_fs.glsl", FRAGMENT);
    // shader program object
    backgroundShader = glCreateProgram();

    glAttachShader(backgroundShader, vertex_shader_object);
    glAttachShader(backgroundShader, fragment_shader_object);

    glBindAttribLocation(backgroundShader, 0, "aPos");

    shader_link_status = LinkShaderProgramObject(backgroundShader);
    if (shader_link_status == False)
    {
        std::cout << "shader linking failed backgroundShader" << std::endl;
        return -1;
    }

    u_viewUniformBGR = glGetUniformLocation(backgroundShader, "view");
    u_projectionUniformBGR = glGetUniformLocation(backgroundShader, "projection");

    glUseProgram(backgroundShader);
    glUniform1i(glGetUniformLocation(backgroundShader, "environmentMap"), 0);
    glUseProgram(0);

    // ****************** FBO Stuff *********************
    fboStuff();

    // ****************** equirectangularToCubemapShader *********************
    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------

    vertex_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/cubemap_vs.glsl", VERTEX);
    fragment_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/equirectangular_to_cubemap_fs.glsl", FRAGMENT);
    // shader program object
    equirectangularToCubemapShader = glCreateProgram();

    glAttachShader(equirectangularToCubemapShader, vertex_shader_object);
    glAttachShader(equirectangularToCubemapShader, fragment_shader_object);

    glBindAttribLocation(equirectangularToCubemapShader, 0, "aPos");

    shader_link_status = LinkShaderProgramObject(equirectangularToCubemapShader);
    if (shader_link_status == False)
    {
        std::cout << "shader linking failed equirectangularToCubemapShader" << std::endl;
        return -1;
    }

    u_viewUniformEQU = glGetUniformLocation(equirectangularToCubemapShader, "view");
    u_projectionUniformEQU = glGetUniformLocation(equirectangularToCubemapShader, "projection");

    glUseProgram(equirectangularToCubemapShader);
    glUniform1i(glGetUniformLocation(equirectangularToCubemapShader, "equirectangularMap"), 0);
    glUniformMatrix4fv(u_projectionUniformEQU, 1, GL_FALSE, glm::value_ptr(captureProjection));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // equirectangularToCubemapShader.setMat4("view", captureViews[i]);
        glUniformMatrix4fv(u_viewUniformEQU, 1, GL_FALSE, glm::value_ptr(captureViews[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // ************** Irradiance Map ****************
    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
   
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    // *************** Irradiance Shader creation ********************
    vertex_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/cubemap_vs.glsl", VERTEX);
    fragment_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/irradiance_convolution_fs.glsl", FRAGMENT);
    // shader program object
    irradianceShader = glCreateProgram();

    glAttachShader(irradianceShader, vertex_shader_object);
    glAttachShader(irradianceShader, fragment_shader_object);

    glBindAttribLocation(irradianceShader, 0, "aPos");

    shader_link_status = LinkShaderProgramObject(irradianceShader);
    if (shader_link_status == False)
    {
        std::cout << "shader linking failed irradianceShader" << std::endl;
        return -1;
    }

    u_viewUniformIRD = glGetUniformLocation(irradianceShader, "view");
    u_projectionUniformIRD = glGetUniformLocation(irradianceShader, "projection");

    glUseProgram(irradianceShader);
    glUniform1i(glGetUniformLocation(irradianceShader, "environmentMap"), 0);
    glUniformMatrix4fv(u_projectionUniformIRD, 1, GL_FALSE, glm::value_ptr(captureProjection));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // irradianceShader.setMat4("view", captureViews[i]);
        glUniformMatrix4fv(u_viewUniformIRD, 1, GL_FALSE, glm::value_ptr(captureViews[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);


    // ************ Prefilter map *************
    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // ************************* Create Prefilter Shader *****************

    vertex_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/cubemap_vs.glsl", VERTEX);
    fragment_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/prefilter_fs.glsl", FRAGMENT);
    // shader program object
    prefilterShader = glCreateProgram();
    glAttachShader(prefilterShader, vertex_shader_object);
    glAttachShader(prefilterShader, fragment_shader_object);

    glBindAttribLocation(prefilterShader, 0, "aPos");

    shader_link_status = LinkShaderProgramObject(prefilterShader);
    if (shader_link_status == False)
    {
        std::cout << "shader linking failed prefilterShader" << std::endl;
        return -1;
    }

    u_viewUniformPRF = glGetUniformLocation(prefilterShader, "view");
    u_projectionUniformPRF = glGetUniformLocation(prefilterShader, "projection");

    glUseProgram(prefilterShader);
    glUniformMatrix4fv(u_projectionUniformPRF, 1, GL_FALSE, glm::value_ptr(captureProjection));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glUniform1i(glGetUniformLocation(prefilterShader, "environmentMap"), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        glUniform1f(glGetUniformLocation(prefilterShader, "roughness"), roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            // prefilterShader.setMat4("view", captureViews[i]);
            glUniformMatrix4fv(u_viewUniformPRF, 1, GL_FALSE, glm::value_ptr(captureViews[i]));
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);

    // *************** Generate BRDF LUT **************************
    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    // ****************** BRDF Shader creation ********************
    vertex_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/brdf_vs.glsl", VERTEX);
    fragment_shader_object = CreateAndCompileShaderObjects("./src/shaders/pbr/brdf_fs.glsl", FRAGMENT);
    // shader program object
    brdfShader = glCreateProgram();
    glAttachShader(brdfShader, vertex_shader_object);
    glAttachShader(brdfShader, fragment_shader_object);

    glBindAttribLocation(brdfShader, 0, "aPos");
    glBindAttribLocation(brdfShader, 1, "aTexCoords");
    shader_link_status = LinkShaderProgramObject(brdfShader);
    if (shader_link_status == False)
    {
        std::cout << "shader linking failed brdfShader" << std::endl;
        return -1;
    }
    glViewport(0, 0, 512, 512);
    glUseProgram(brdfShader);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)1200 / (float)720, 0.1f, 100.0f);
    glUseProgram(u_modelShader_pbr);
    glUniformMatrix4fv(glGetUniformLocation(u_modelShader_pbr, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUseProgram(0);

    glUseProgram(backgroundShader);
    glUniformMatrix4fv(glGetUniformLocation(backgroundShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUseProgram(0);


    // LoadGlTexture(&texture_albedo, "./assets/textures/pbr/gold/albedo.png");
    // LoadGlTexture(&texture_ao, "./assets/textures/pbr/gold/ao.png");
    // LoadGlTexture(&texture_metallic, "./assets/textures/pbr/gold/metallic.png");
    // LoadGlTexture(&texture_normal, "./assets/textures/pbr/gold/normal.png");
    // LoadGlTexture(&texture_roughness, "./assets/textures/pbr/gold/roughness.png");

    LoadGlTexture(&texture_albedo, "./assets/textures/pbr/albedo.png");
    LoadGlTexture(&texture_ao, "./assets/textures/pbr/ao.png");
    LoadGlTexture(&texture_metallic, "./assets/textures/pbr/metallic.png");
    LoadGlTexture(&texture_normal, "./assets/textures/pbr/normal.png");
    LoadGlTexture(&texture_roughness, "./assets/textures/pbr/roughness.png");
    
    // LoadGlTexture(&texture_albedo, "./assets/textures/pbr/plastic/albedo.png");
    // LoadGlTexture(&texture_ao, "./assets/textures/pbr/plastic/ao.png");
    // LoadGlTexture(&texture_metallic, "./assets/textures/pbr/plastic/metallic.png");
    // LoadGlTexture(&texture_normal, "./assets/textures/pbr/plastic/normal.png");
    // LoadGlTexture(&texture_roughness, "./assets/textures/pbr/plastic/roughness.png");

    // LoadGlTexture(&texture_albedo, "./assets/textures/pbr/wall/albedo.png");
    // LoadGlTexture(&texture_ao, "./assets/textures/pbr/wall/ao.png");
    // LoadGlTexture(&texture_metallic, "./assets/textures/pbr/wall/metallic.png");
    // LoadGlTexture(&texture_normal, "./assets/textures/pbr/wall/normal.png");
    // LoadGlTexture(&texture_roughness, "./assets/textures/pbr/wall/roughness.png");

    // LoadGlTexture(&texture_albedo, "./assets/textures/pbr/grass/albedo.png");
    // LoadGlTexture(&texture_ao, "./assets/textures/pbr/grass/ao.png");
    // LoadGlTexture(&texture_metallic, "./assets/textures/pbr/grass/metallic.png");
    // LoadGlTexture(&texture_normal, "./assets/textures/pbr/grass/normal.png");
    // LoadGlTexture(&texture_roughness, "./assets/textures/pbr/grass/roughness.png");

    // for helmet
    // LoadGlTexture(&texture_albedo, "./assets/models/helmet/Default_albedo.jpg");
    // LoadGlTexture(&texture_ao, "./assets/models/helmet/Default_AO.jpg");
    // LoadGlTexture(&texture_metallic, "./assets/models/helmet/Default_emissive.jpg");
    // LoadGlTexture(&texture_normal, "./assets/models/helmet/Default_normal.jpg");
    // LoadGlTexture(&texture_roughness, "./assets/models/helmet/Default_metalRoughness.jpg");

    return 0;
}

void ModelPbr::fboStuff()
{
    // pbr: setup framebuffer
    // ----------------------
    
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    // ---------------------------------
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf("./assets/textures/pbr/barcelona_rooftop.hdr", &width, &height, &nrComponents, 0);
    
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
   
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f));
    captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f));
    captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    
}

void ModelPbr::render()
{
    glm::vec3 lightPositions[] = {
        glm::vec3(-15.0f, 15.0f, 10.0f),
        glm::vec3(15.0f, 15.0f, 10.0f),
        glm::vec3(-15.0f, -15.0f, 10.0f),
        glm::vec3(15.0f, -15.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(600.0f, 600.0f, 600.0f),
        glm::vec3(600.0f, 600.0f, 600.0f),
        glm::vec3(600.0f, 600.0f, 600.0f),
        glm::vec3(600.0f, 600.0f, 600.0f)};

    glUseProgram(u_modelShader_pbr);
    glm::mat4 model_matrix = glm::mat4(1.0f);
    glm::mat4 view_matrix = glm::mat4(1.0f);
    glm::mat4 projection_matrix = glm::mat4(1.0f);

    // model_matrix = glm::rotate(model_matrix, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(0.5f, 0.5f, 0.5f));// * glm::rotate(model_matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    view_matrix = camera.getViewMatrix();
    projection_matrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(u_modelMatrixUniform_pbr, 1, GL_FALSE, glm::value_ptr(model_matrix));
    glUniformMatrix4fv(u_viewMatrixUniform_pbr, 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniformMatrix4fv(u_projectionMatrixUniform_pbr, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix3fv(u_normalMatrix_pbr, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(model_matrix)))));

    glUniform3fv(u_camPos, 1, glm::value_ptr(camera.getEye()));

    glUniform3fv(u_lightPositions0, 1, glm::value_ptr(lightPositions[0]));
    glUniform3fv(u_lightPositions1, 1, glm::value_ptr(lightPositions[1]));
    glUniform3fv(u_lightPositions2, 1, glm::value_ptr(lightPositions[2]));
    glUniform3fv(u_lightPositions3, 1, glm::value_ptr(lightPositions[3]));
    glUniform3fv(u_lightColors0, 1, glm::value_ptr(lightColors[0]));
    glUniform3fv(u_lightColors1, 1, glm::value_ptr(lightColors[1]));
    glUniform3fv(u_lightColors2, 1, glm::value_ptr(lightColors[2]));
    glUniform3fv(u_lightColors3, 1, glm::value_ptr(lightColors[3]));

    glUniform1f(u_exposure, 3.0f);
    glUniform1i(u_isInstanced, false);
    // model_->Draw(u_modelShader_pbr, amount);
    model_->Draw(u_modelShader_pbr);

    glUseProgram(0);
}

void ModelPbr::renderPBR()
{
    static bool firstTime = true;
    if (firstTime)
    {
        model_ = std::make_unique<Model>("./assets/models/WaterBottle.glb", m_drawMode);
        reloadVertexBuffers(model_);
        firstTime = false;
    }

    glm::vec3 lightPositions[] = {
        glm::vec3(-15.0f, 5.0f, 10.0f),
        glm::vec3(15.0f, 5.0f, 10.0f),
        glm::vec3(-15.0f, -5.0f, 10.0f),
        glm::vec3(15.0f, -5.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)};

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();

    // model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

    glUseProgram(u_modelShader_pbr);

    glUniformMatrix4fv(u_modelMatrixUniform_pbr, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(u_viewMatrixUniform_pbr, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(u_projectionMatrixUniform_pbr, 1, GL_FALSE, glm::value_ptr(perspectiveProjectionMatrix));
    glUniformMatrix3fv(u_normalMatrix_pbr, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(model)))));

    glUniform3fv(u_camPos, 1, glm::value_ptr(camera.getEye()));
    // bind pre-computed IBL data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glUniform1i(glGetUniformLocation(u_modelShader_pbr, "irradianceMap"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glUniform1i(glGetUniformLocation(u_modelShader_pbr, "prefilterMap"), 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glUniform1i(glGetUniformLocation(u_modelShader_pbr, "brdfLUT"), 2);

    // glUniform1i(glGetUniformLocation(u_modelShader_pbr, "texture_diffuse1"), 3);
    // glUniform1i(glGetUniformLocation(u_modelShader_pbr, "texture_normal1"), 4);
    // glUniform1i(glGetUniformLocation(u_modelShader_pbr, "texture_metallicMap1"), 5);
    // glUniform1i(glGetUniformLocation(u_modelShader_pbr, "texture_roughnessMap1"), 6);
    // glUniform1i(glGetUniformLocation(u_modelShader_pbr, "texture_aoMap1"), 7);
    // // rusted iron
    //     glActiveTexture(GL_TEXTURE3);
    //     glBindTexture(GL_TEXTURE_2D, texture_albedo);
    //     glActiveTexture(GL_TEXTURE4);
    //     glBindTexture(GL_TEXTURE_2D, texture_normal);
    //     glActiveTexture(GL_TEXTURE5);
    //     glBindTexture(GL_TEXTURE_2D, texture_metallic);
    //     glActiveTexture(GL_TEXTURE6);
    //     glBindTexture(GL_TEXTURE_2D, texture_roughness);
    //     glActiveTexture(GL_TEXTURE7);
    //     glBindTexture(GL_TEXTURE_2D, texture_ao);


    glUniform3fv(u_lightPositions0, 1, glm::value_ptr(lightPositions[0]));
    glUniform3fv(u_lightPositions1, 1, glm::value_ptr(lightPositions[1]));
    glUniform3fv(u_lightPositions2, 1, glm::value_ptr(lightPositions[2]));
    glUniform3fv(u_lightPositions3, 1, glm::value_ptr(lightPositions[3]));
    glUniform3fv(u_lightColors0, 1, glm::value_ptr(lightColors[0]));
    glUniform3fv(u_lightColors1, 1, glm::value_ptr(lightColors[1]));
    glUniform3fv(u_lightColors2, 1, glm::value_ptr(lightColors[2]));
    glUniform3fv(u_lightColors3, 1, glm::value_ptr(lightColors[3]));

    model_->Draw(u_modelShader_pbr);
    // renderSphere();

    glUseProgram(0);

    // background shader
    glUseProgram(backgroundShader);
    glUniformMatrix4fv(u_viewUniformBGR, 1, GL_FALSE, glm::value_ptr(view));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    renderCube();

    glUseProgram(0);
}

void ModelPbr::resize(int width, int height)
{
    _width = width;
    _height = height;
}

void ModelPbr::uninitialize()
{

}


/// 
// renders (and builds at first invocation) a sphere
// -------------------------------------------------


void ModelPbr::renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<GLsizei>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------


void ModelPbr::renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------


void ModelPbr::renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
