#ifndef MESH_H
#define MESH_H

//#include <glad/glad.h> // holds all OpenGL type declarations

#if defined(USE_GL_ES3)
#include <GLES3/gl3.h>  // Use GL ES 3
#else
#include <GL/gl3w.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Scene_state.h"
#include "Scene_vertex_t.h"

#include <string>
#include <vector>
using namespace std;

struct Vertex {
    // position
    glm::vec4 Position;
    // color 
    glm::vec4 Color;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Drawable_Streamsurface {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    unsigned int VAO;
    int surface_width;
    float surface_height = 0.1f;
    float distance_threshold;
    glm::vec4 c;
    Shader shader;
    Scene_vertex_t translate;
    Scene_vertex_t scale;

    Drawable_Streamsurface(Shader& shader) {
        this->shader = shader;
    }

    void set_color(const Color& c) {
        this->c = glm::vec4(c.r() / 255.0f, c.g() / 255.0f, c.b() / 255.0f, c.a() / 255.0f);
    }

    void set_surface_height(float surface_height) {
        this->surface_height = surface_height;
    }

    // render the mesh
    void Draw(glm::mat4& mvp, glm::vec3& camera)
    {
        shader.use();
        shader.setMat4("mvp", mvp);

        shader.setFloat("translate.x", translate[0]);
        shader.setFloat("translate.y", translate[1]);
        shader.setFloat("translate.z", translate[2]);
        shader.setFloat("translate.w", translate[3]);
        shader.setFloat("translate.e", translate[4]);

        shader.setFloat("scale.x", scale[0]);
        shader.setFloat("scale.y", scale[1]);
        shader.setFloat("scale.z", scale[2]);
        shader.setFloat("scale.w", scale[3]);
        shader.setFloat("scale.e", scale[4]);


        shader.setVec3("camera", camera);
        shader.setFloat("surface_height", surface_height);

        //glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
        //glBeginTransformFeedback(GL_TRIANGLES);

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        /*
        glEndTransformFeedback();
        glFlush();

        GLfloat feedback[3];
        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);

        for (auto p : feedback) {
            std::cout << p << ", ";
        }
        std::cout << "\n";
        */

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    //-------Mesh generation---------

    //Add one point strip to the mesh 
    //TODO hardcoded color
    void add_point_strip(std::vector<Scene_vertex_t>& points, float time) {

        //first vertex strip 
        if (vertices.empty()) {

            surface_width = points.size();

            auto v1 = points.at(0);
            auto v2 = points.at(points.size()-1);
            distance_threshold = glm::distance(glm::vec3(v1(0), v1(1), v1(2)), glm::vec3(v2(0), v2(1), v2(2)));

            for (int i = 0; i < points.size(); i++) {
                auto current = points.at(i);
                Vertex vertex;

                vertex.Position = glm::vec4(current(0), current(1), current(2), current(3));
                vertex.Color = c;

                vertices.push_back(vertex);
            }
        }
        //later vertex strips
        else {

            int index = vertices.size() - 1;

            for (int i = 0; i < points.size(); i++) {

                auto current = points.at(i);
                Vertex vertex;

                vertex.Position = glm::vec4(current(0), current(1), current(2), current(3));
                vertex.Color = c;

                vertices.push_back(vertex);

                //faces
                //first vertex of the strip
                if (i == 0) {
                    indices.push_back(index);
                    indices.push_back(index - surface_width + 1);
                    indices.push_back(index - surface_width);
                }
                //last vertex
                else if (i == points.size() - 1) {
                    indices.push_back(index);
                    indices.push_back(index - surface_width);
                    indices.push_back(index - 1);
                }
                //middle vertex
                else {
                    indices.push_back(index);
                    indices.push_back(index - surface_width + 1);
                    indices.push_back(index - surface_width);

                    indices.push_back(index);
                    indices.push_back(index - surface_width);
                    indices.push_back(index - 1);
                }
            }
        }
    }

    void translate_vertices(Scene_vertex_t& translate) {
        this->translate = translate;
    }

    void scale_vertices(Scene_vertex_t& scale) {
        this->scale = scale;
    }

    // initializes all the buffer objects/arrays
    void Drawable_Streamsurface::setup_mesh()
    {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // vertex color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

        
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * indices.size(), nullptr, GL_STATIC_READ);
    }

private:
    
    unsigned int VBO, EBO;
    GLuint tbo;
    glm::mat4 model_matrix = glm::mat4(1.f);

};
#endif