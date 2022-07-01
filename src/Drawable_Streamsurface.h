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

struct Vertex {
    // position
    glm::vec4 Position;
    // 3D position
    glm::vec4 Position_3D;
    // color 
    glm::vec4 Color;
    //normal
    glm::vec3 Normal;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Drawable_Streamsurface {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;
    int surface_width;
    float surface_height = 1.0f;
    glm::vec4 c;
    Shader shader;
    Shader shader_normal = Shader("assets/surface.vert", "assets/normal.frag", "assets/normal.geom");

    Drawable_Streamsurface(Shader& shader_): 
        shader(shader_)
    {
    }

    void set_color(const Color& c) {
        this->c = glm::vec4(c.r() / 255.0f, c.g() / 255.0f, c.b() / 255.0f, c.a() / 255.0f);
    }

    void set_surface_height(float surface_height) {
        this->surface_height = surface_height;
    }

    // render the mesh
    void Draw(glm::mat4& mvp, glm::mat3& normalMatrix, glm::vec3& camera)
    {
        shader.use();
        shader.setMat4("mvp", mvp);
        shader.setMat3("normalMatrix", normalMatrix);

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
        /*
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
        glBeginTransformFeedback(GL_TRIANGLES);
        */
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        /*
        
        glEndTransformFeedback();
        glFlush();

        GLfloat feedback[12];
        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);

        std::cout << "Feedback: \n";
        for (auto p : feedback) {
            std::cout << p << ", ";
        }
        std::cout << "\n";
        */
        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    void Draw_Normals(glm::mat4& mvp, glm::mat3& normalMatrix, glm::mat4& projectionMatrix, glm::vec3& camera) {

        shader_normal.use();
        
        shader_normal.setMat4("mvp", mvp);
        shader_normal.setMat3("normalMatrix", normalMatrix);
        shader_normal.setMat4("projection", projectionMatrix);


        shader_normal.setFloat("translate.x", translate[0]);
        shader_normal.setFloat("translate.y", translate[1]);
        shader_normal.setFloat("translate.z", translate[2]);
        shader_normal.setFloat("translate.w", translate[3]);
        shader_normal.setFloat("translate.e", translate[4]);

        shader_normal.setFloat("scale.x", scale[0]);
        shader_normal.setFloat("scale.y", scale[1]);
        shader_normal.setFloat("scale.z", scale[2]);
        shader_normal.setFloat("scale.w", scale[3]);
        shader_normal.setFloat("scale.e", scale[4]);

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    //-------Mesh generation---------
    
    //Add one point strip to the mesh 
    void add_point_strip(std::vector<Scene_vertex_t>& points, float time) {

        //first vertex strip 
        if (vertices.empty()) {

            surface_width = points.size();

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

            for (int i = 0; i < points.size(); i++) {

                auto current = points.at(i);
                Vertex vertex;

                vertex.Position = glm::vec4(current(0), current(1), current(2), current(3));
                vertex.Color = c;

                vertices.push_back(vertex);

                int index = vertices.size() - 1;

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
                    indices.push_back(index - surface_width);
                    indices.push_back(index - 1);

                    indices.push_back(index);
                    indices.push_back(index - surface_width + 1);
                    indices.push_back(index - surface_width);
                }
            }
        }
    }

    //overwrites and projects the current positions of this surface to 3D
    void project_vertices_to3D_and_apply_transforms() {
        unsigned int index = 0;
        for (Vertex v : vertices) {

            Scene_vertex_t tmp_vert(5);
            tmp_vert[0] = v.Position.x;
            tmp_vert[1] = v.Position.y;
            tmp_vert[2] = v.Position.z;
            tmp_vert[3] = v.Position.w;
            tmp_vert[4] = 1.0f;

            tmp_vert += translate;

            for (int i = 0; i < tmp_vert.size(); i++)
            {
                tmp_vert[i] *= scale[i];
            }

            tmp_vert = prod(tmp_vert, rot_mat);
            tmp_vert = tmp_vert - camera_4D;
            tmp_vert = prod(tmp_vert, projection_4D);

            //TODO
            //assert(tmp_vert(3) > 0);
            if (tmp_vert(3) < 0)
                std::cerr << "tmp_vert(3) < 0 \n";

            tmp_vert(0) /= tmp_vert(4);
            tmp_vert(1) /= tmp_vert(4);
            tmp_vert(2) /= tmp_vert(4);

            vertices.at(index++).Position_3D = glm::vec4(tmp_vert(0), tmp_vert(1), tmp_vert(2), 1.0f);
        }
    }

    void buffer_vertex_data() {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    }

    //TODO Update 
    void calculate_normals() {
        /*
        std::vector<glm::vec3> projected_v;

        //project vertices to 3D
        for (Vertex v : vertices) {

            Scene_vertex_t tmp_vert(5);
            tmp_vert[0] = v.Position.x;
            tmp_vert[1] = v.Position.y;
            tmp_vert[2] = v.Position.z;
            tmp_vert[3] = v.Position.w;
            tmp_vert[4] = 1.0f;

            //TODO add rot_mat
            //Scene_vertex_t tmp_vert = prod(point, rot_mat);
            tmp_vert = tmp_vert - camera_4D;
            tmp_vert = prod(tmp_vert, projection_4D);

            //TODO
            //assert(tmp_vert(3) > 0);

            tmp_vert(0) /= tmp_vert(4);
            tmp_vert(1) /= tmp_vert(4);
            tmp_vert(2) /= tmp_vert(4);

            glm::vec3 vertex = glm::vec3(tmp_vert(0), tmp_vert(1), tmp_vert(2));

            projected_v.push_back(vertex);
        }
        */
        std::vector<std::vector<unsigned int>> adjacency(vertices.size());
        std::vector<glm::vec3> face_normals(indices.size() / 3);

        //calculate face normal and adjacency information
        for (int i = 0; i < indices.size(); i += 3) {
            glm::vec3 v1(vertices.at(indices.at(i)).Position_3D);
            glm::vec3 v2(vertices.at(indices.at(i + 1)).Position_3D);
            glm::vec3 v3(vertices.at(indices.at(i + 2)).Position_3D);

            //save adjacency info for vertex normals
            adjacency.at(indices.at(i)).push_back(i / 3);
            adjacency.at(indices.at(i + 1)).push_back(i / 3);
            adjacency.at(indices.at(i + 2)).push_back(i / 3);

            glm::vec3 e1 = v1 - v2;
            glm::vec3 e2 = v3 - v2;
            glm::vec3 normal = glm::normalize(glm::cross(e2, e1));
            face_normals.at(i / 3) = normal;
        }

        //calculate vertex normals
        int index = 0;
        for (std::vector<unsigned int> adjacent : adjacency) {
            glm::vec3 sum = glm::vec3(0.0, 0.0, 0.0);

            for (unsigned int index : adjacent) {
                sum += face_normals.at(index);
            }
            sum /= adjacent.size();

            vertices.at(index++).Normal = sum;
        }
    }

    // initializes all the buffer objects/arrays
    void Drawable_Streamsurface::setup_mesh()
    {

        project_vertices_to3D_and_apply_transforms();
        calculate_normals();

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
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position_3D));

        // vertex color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

        // vertex normal
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        // for Transfer Feedback Debugging
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * indices.size(), nullptr, GL_STATIC_READ);
    }
    
    void set_projection_camera(boost::numeric::ublas::matrix<float>& projection_4D, Scene_vertex_t& camera_4D) {
        this->projection_4D = projection_4D;
        this->camera_4D = camera_4D;
    }

    void set_rotation(boost::numeric::ublas::matrix<float>& rot_mat) {
        this->rot_mat = rot_mat;
    }

    void translate_vertices(Scene_vertex_t& translate) {
        this->translate = translate;
    }

    void scale_vertices(Scene_vertex_t& scale) {
        this->scale = scale;
    }

private:
    
    unsigned int VBO, EBO;
    GLuint tbo;

    boost::numeric::ublas::matrix<float> projection_4D; 
    Scene_vertex_t camera_4D; 
    boost::numeric::ublas::matrix<float> rot_mat;
    Scene_vertex_t translate;
    Scene_vertex_t scale;

};

#endif