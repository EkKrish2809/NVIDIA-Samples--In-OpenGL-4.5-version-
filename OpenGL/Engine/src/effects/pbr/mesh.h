#ifndef MESH_H
#define MESH_H

// #include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// #include "shader.h"
#include "../../utils/common.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};

struct PBRMaterial
    {
        glm::vec3 albedo;
        glm::float32 metallic;
        glm::float32 roughness;
        glm::float32 emissive;
        glm::float32 alpha;
    };
struct Texture {
	unsigned int id;
	string type;
	string path;
	// for PBR
	PBRMaterial pbrMaterial;
};

class Mesh {
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;
	bool isEmissiceNeeded = false;

	/*  Functions  */
	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, bool isEmissive = false)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		isEmissiceNeeded = isEmissive;

		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}

	// render the mesh
	// void Draw(Shader shader, unsigned int drawMode = GL_TRIANGLES)
	void Draw(GLuint shader, unsigned int drawMode = GL_TRIANGLES)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		unsigned int metalNr = 1;
		unsigned int roughNr = 1;
		unsigned int aoNr = 1;
		unsigned int depthNr = 1;
		unsigned int emissiveNr = 1;
		// printf("texture vector size : %ld\n", textures.size());
		// for (unsigned int i = 3; i < textures.size(); i++)
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i + 3); // active proper texture unit before binding
											  // retrieve texture number (the N in diffuse_textureN)
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream
			else if (name == "texture_metallicMap")
				number = std::to_string(metalNr++); // transfer unsigned int to stream
			else if (name == "texture_roughnessMap")
				number = std::to_string(roughNr++); // transfer unsigned int to stream
			else if (name == "texture_aoMap")
				number = std::to_string(aoNr++); // transfer unsigned int to stream
			else if (name == "depth_texture")
				number = std::to_string(depthNr++); // transfer unsigned int to stream
			else if (name == "texture_emissiveMap")
				number = std::to_string(emissiveNr++); // transfer unsigned int to stream

													 // now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader, (name + number).c_str()), i+3 );
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
			glUniform1f(glGetUniformLocation(shader, "mr_roughness"), textures[i].pbrMaterial.roughness);
			glUniform1f(glGetUniformLocation(shader, "mr_metallic"), textures[i].pbrMaterial.metallic);
			glUniform1f(glGetUniformLocation(shader, "mr_emissive"), textures[i].pbrMaterial.emissive);

			glUniform1i(glGetUniformLocation(shader, "is_emissive_needed"), isEmissiceNeeded);

            // printf("texture name : %s\n", (name + number ).c_str());
            // printf("texture id : %d\n", textures[i].id);
		}
        
		// draw mesh
		glBindVertexArray(VAO);
		// shader.use();
		// printf("Num Elements : %d\n", indices.size());
		glDrawElements(drawMode, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE3);
	}
	// void DrawIstances(Shader shader, unsigned int drawMode = GL_TRIANGLES, unsigned int nIstances = 1)
	void DrawIstances(GLuint shader, unsigned int drawMode = GL_TRIANGLES, unsigned int nIstances = 1)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		unsigned int metalNr = 1;
		unsigned int roughNr = 1;
		unsigned int aoNr = 1;
		printf("Instanced1\n");
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
											  // retrieve texture number (the N in diffuse_textureN)
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream
			else if (name == "texture_metallicMap")
				number = std::to_string(metalNr++); // transfer unsigned int to stream
			else if (name == "texture_roughnessMap")
				number = std::to_string(roughNr++); // transfer unsigned int to stream
			else if (name == "texture_aoMap")
				number = std::to_string(aoNr++); // transfer unsigned int to stream

													 // now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader, (name + number).c_str()), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		printf("Instanced2\n");
		// draw mesh
		glBindVertexArray(VAO);
		//shader.use();
		// printf("Num Elements : %d\n", indices.size());
		glDrawElementsInstanced(drawMode, indices.size(), GL_UNSIGNED_INT, 0, nIstances);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

private:
	/*  Render data  */
	unsigned int VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
	}
};
#endif