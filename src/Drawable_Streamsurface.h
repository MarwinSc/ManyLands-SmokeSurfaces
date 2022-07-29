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
    float hmg_coordinate = 1.0f;
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
    std::vector<glm::vec4>    positions;
    int surface_width;
    float surface_height = 1.0f;
    glm::vec4 c;
    Shader shader;
    Shader shader_normal = Shader("assets/surface.vert", "assets/normal.frag", "assets/normal.geom");
    ComputeShader shader_compute = ComputeShader("assets/compute_shader.comp");

    Drawable_Streamsurface(Shader& shader_): 
        shader(shader_)
    {
    }

    //TODO update color for all vertices on call
    void set_color(const Color& c) {
        auto temp_color = glm::vec4(c.r() / 255.0f, c.g() / 255.0f, c.b() / 255.0f, c.a() / 255.0f);
        if (!(this->c == temp_color)) {
            this->c = temp_color;
            flag_update = true;
        }
    }

    void set_surface_height(float surface_height) {
        this->surface_height = surface_height;
    }

    // render the mesh
    void Draw(glm::mat4& mvp, glm::mat3& normalMatrix, glm::vec3& camera)
    {
        enableBlendMode();

        shader.use();
        shader.setMat4("mvp", mvp);
        shader.setMat3("normalMatrix", normalMatrix);

        shader.setVec3("camera", camera);
        shader.setFloat("surface_height", surface_height);

        if(flag_use_computeshader){
            glBindVertexArray(vaos[1]); // bind VAO
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);


            glBindVertexArray(0);
            //glUseProgram(0);
        }
        else {

            //glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
            glActiveTexture(GL_TEXTURE0);
        }
        disableBlendMode();
    }

    void Draw_Normals(glm::mat4& mvp, glm::mat3& normalMatrix, glm::mat4& projectionMatrix, glm::vec3& camera) {
        
        shader_normal.use();

        shader_normal.setMat4("mvp", mvp);
        shader_normal.setMat3("normalMatrix", normalMatrix);
        shader_normal.setMat4("projection", projectionMatrix);

        if (flag_use_computeshader) {
            glBindVertexArray(vaos[1]);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        else {
            // draw mesh
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
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
                positions.push_back(vertex.Position);
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
                positions.push_back(vertex.Position);

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

    //-------Update---------

    void update(bool apply_transforms = true) {
        if(flag_update && !flag_use_computeshader){
            apply_transforms_and_project_vertices_to_3D(apply_transforms);
        }
    }

    void buffer_vertex_data() {
        if (flag_update && !flag_use_computeshader) {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            //glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
            flag_update = false;
        }
    }

    void update_and_buffer_vertex_data(bool apply_transforms = true) {
        if (flag_use_computeshader) {
            compute();
        }else if(flag_update){
            update(apply_transforms);
            buffer_vertex_data();
        }
    }

    //overwrites and projects the current positions of this surface to 3D
    void apply_transforms_and_project_vertices_to_3D(bool apply_transforms = true) {
        unsigned int index = 0;
        for (Vertex &v : vertices) {

            Scene_vertex_t tmp_vert(5);
            tmp_vert[0] = v.Position.x;
            tmp_vert[1] = v.Position.y;
            tmp_vert[2] = v.Position.z;
            tmp_vert[3] = v.Position.w;
            tmp_vert[4] = v.hmg_coordinate;

            //don't apply when unfolding
            if (apply_transforms) {
                tmp_vert += translate;

                for (int i = 0; i < tmp_vert.size(); i++)
                {
                    tmp_vert[i] *= scale[i];
                }
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
        calculate_normals();
    }

    void calculate_normals() {

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

    //--------------------Compute Shader----------------------------------------

    void compute() {
        if(flag_use_computeshader){
            //shader and uniforms 
            shader_compute.use();
            shader_compute.setFloat("field_of_view", 30.0f);
            shader_compute.setInt("nr_vertices", vertices.size());
        
            shader_compute.setFloat("translate.x", translate[0]);
            shader_compute.setFloat("translate.y", translate[1]);
            shader_compute.setFloat("translate.z", translate[2]);
            shader_compute.setFloat("translate.w", translate[3]);
            shader_compute.setFloat("translate.e", translate[4]);
        
            shader_compute.setFloat("scale.x", scale[0]);
            shader_compute.setFloat("scale.y", scale[1]);
            shader_compute.setFloat("scale.z", scale[2]);
            shader_compute.setFloat("scale.w", scale[3]);
            shader_compute.setFloat("scale.e", scale[4]);
        
            shader_compute.setFloat("camera_4D.x", camera_4D[0]);
            shader_compute.setFloat("camera_4D.y", camera_4D[1]);
            shader_compute.setFloat("camera_4D.z", camera_4D[2]);
            shader_compute.setFloat("camera_4D.w", camera_4D[3]);
            shader_compute.setFloat("camera_4D.e", camera_4D[4]);

            /*
            shader_compute.setFloat("projection_4D.x.x", projection_4D(0, 0));
            shader_compute.setFloat("projection_4D.x.y", projection_4D(0, 1));
            shader_compute.setFloat("projection_4D.x.z", projection_4D(0, 2));
            shader_compute.setFloat("projection_4D.x.w", projection_4D(0, 3));
            shader_compute.setFloat("projection_4D.x.e", projection_4D(0, 4));
                                        
            shader_compute.setFloat("projection_4D.y.x", projection_4D(1, 0));
            shader_compute.setFloat("projection_4D.y.y", projection_4D(1, 1));
            shader_compute.setFloat("projection_4D.y.z", projection_4D(1, 2));
            shader_compute.setFloat("projection_4D.y.w", projection_4D(1, 3));
            shader_compute.setFloat("projection_4D.y.e", projection_4D(1, 4));
                                        
            shader_compute.setFloat("projection_4D.z.x", projection_4D(2, 0));
            shader_compute.setFloat("projection_4D.z.y", projection_4D(2, 1));
            shader_compute.setFloat("projection_4D.z.z", projection_4D(2, 2));
            shader_compute.setFloat("projection_4D.z.w", projection_4D(2, 3));
            shader_compute.setFloat("projection_4D.z.e", projection_4D(2, 4));
                                        
            shader_compute.setFloat("projection_4D.w.x", projection_4D(3, 0));
            shader_compute.setFloat("projection_4D.w.y", projection_4D(3, 1));
            shader_compute.setFloat("projection_4D.w.z", projection_4D(3, 2));
            shader_compute.setFloat("projection_4D.w.w", projection_4D(3, 3));
            shader_compute.setFloat("projection_4D.w.e", projection_4D(3, 4));
                                        
            shader_compute.setFloat("projection_4D.e.x", projection_4D(4, 0));
            shader_compute.setFloat("projection_4D.e.y", projection_4D(4, 1));
            shader_compute.setFloat("projection_4D.e.z", projection_4D(4, 2));
            shader_compute.setFloat("projection_4D.e.w", projection_4D(4, 3));
            shader_compute.setFloat("projection_4D.e.e", projection_4D(4, 4));
            */

            shader_compute.setFloat("rot.x.x", rot_mat(0, 0));
            shader_compute.setFloat("rot.x.y", rot_mat(0, 1));
            shader_compute.setFloat("rot.x.z", rot_mat(0, 2));
            shader_compute.setFloat("rot.x.w", rot_mat(0, 3));
            shader_compute.setFloat("rot.x.e", rot_mat(0, 4));

            shader_compute.setFloat("rot.y.x", rot_mat(1, 0));
            shader_compute.setFloat("rot.y.y", rot_mat(1, 1));
            shader_compute.setFloat("rot.y.z", rot_mat(1, 2));
            shader_compute.setFloat("rot.y.w", rot_mat(1, 3));
            shader_compute.setFloat("rot.y.e", rot_mat(1, 4));

            shader_compute.setFloat("rot.z.x", rot_mat(2, 0));
            shader_compute.setFloat("rot.z.y", rot_mat(2, 1));
            shader_compute.setFloat("rot.z.z", rot_mat(2, 2));
            shader_compute.setFloat("rot.z.w", rot_mat(2, 3));
            shader_compute.setFloat("rot.z.e", rot_mat(2, 4));

            shader_compute.setFloat("rot.w.x", rot_mat(3, 0));
            shader_compute.setFloat("rot.w.y", rot_mat(3, 1));
            shader_compute.setFloat("rot.w.z", rot_mat(3, 2));
            shader_compute.setFloat("rot.w.w", rot_mat(3, 3));
            shader_compute.setFloat("rot.w.e", rot_mat(3, 4));

            shader_compute.setFloat("rot.e.x", rot_mat(4, 0));
            shader_compute.setFloat("rot.e.y", rot_mat(4, 1));
            shader_compute.setFloat("rot.e.z", rot_mat(4, 2));
            shader_compute.setFloat("rot.e.w", rot_mat(4, 3));
            shader_compute.setFloat("rot.e.e", rot_mat(4, 4));

            if (flag_tesseract_unfolding) {
                shader_compute.setBool("flag_tesseract_unfolding", true);

                shader_compute.setFloat("disp.x", disp[0]);
                shader_compute.setFloat("disp.y", disp[1]);
                shader_compute.setFloat("disp.z", disp[2]);
                shader_compute.setFloat("disp.w", disp[3]);
                shader_compute.setFloat("disp.e", disp[4]);

                if (flag_disp2) {
                    shader_compute.setBool("flag_disp2", true);

                    shader_compute.setFloat("disp2.x", disp2[0]);
                    shader_compute.setFloat("disp2.y", disp2[1]);
                    shader_compute.setFloat("disp2.z", disp2[2]);
                    shader_compute.setFloat("disp2.w", disp2[3]);
                    shader_compute.setFloat("disp2.e", disp2[4]);
                }

                shader_compute.setFloat("rot_unfolding.x.x", rot_mat_unfolding(0, 0));
                shader_compute.setFloat("rot_unfolding.x.y", rot_mat_unfolding(0, 1));
                shader_compute.setFloat("rot_unfolding.x.z", rot_mat_unfolding(0, 2));
                shader_compute.setFloat("rot_unfolding.x.w", rot_mat_unfolding(0, 3));
                shader_compute.setFloat("rot_unfolding.x.e", rot_mat_unfolding(0, 4));

                shader_compute.setFloat("rot_unfolding.y.x", rot_mat_unfolding(1, 0));
                shader_compute.setFloat("rot_unfolding.y.y", rot_mat_unfolding(1, 1));
                shader_compute.setFloat("rot_unfolding.y.z", rot_mat_unfolding(1, 2));
                shader_compute.setFloat("rot_unfolding.y.w", rot_mat_unfolding(1, 3));
                shader_compute.setFloat("rot_unfolding.y.e", rot_mat_unfolding(1, 4));

                shader_compute.setFloat("rot_unfolding.z.x", rot_mat_unfolding(2, 0));
                shader_compute.setFloat("rot_unfolding.z.y", rot_mat_unfolding(2, 1));
                shader_compute.setFloat("rot_unfolding.z.z", rot_mat_unfolding(2, 2));
                shader_compute.setFloat("rot_unfolding.z.w", rot_mat_unfolding(2, 3));
                shader_compute.setFloat("rot_unfolding.z.e", rot_mat_unfolding(2, 4));

                shader_compute.setFloat("rot_unfolding.w.x", rot_mat_unfolding(3, 0));
                shader_compute.setFloat("rot_unfolding.w.y", rot_mat_unfolding(3, 1));
                shader_compute.setFloat("rot_unfolding.w.z", rot_mat_unfolding(3, 2));
                shader_compute.setFloat("rot_unfolding.w.w", rot_mat_unfolding(3, 3));
                shader_compute.setFloat("rot_unfolding.w.e", rot_mat_unfolding(3, 4));

                shader_compute.setFloat("rot_unfolding.e.x", rot_mat_unfolding(4, 0));
                shader_compute.setFloat("rot_unfolding.e.y", rot_mat_unfolding(4, 1));
                shader_compute.setFloat("rot_unfolding.e.z", rot_mat_unfolding(4, 2));
                shader_compute.setFloat("rot_unfolding.e.w", rot_mat_unfolding(4, 3));
                shader_compute.setFloat("rot_unfolding.e.e", rot_mat_unfolding(4, 4));
            }
            if(flag_move_to_3D){
                shader_compute.setBool("flag_move_to_3D", true);
                shader_compute.setFloat("coeff", coeff);
                shader_compute.setFloat("tesseract_size",tesseract_size);
                shader_compute.setInt("move_index", move_index);
            }
            if (flag_plot_unfolding) {
                shader_compute.setMat4("rot_unfolding_3D", rot_mat_unfolding_3D);
                shader_compute.setBool("flag_plots_unfolding", true);

                shader_compute.setFloat("disp3.x", disp3[0]);
                shader_compute.setFloat("disp3.y", disp3[1]);
                shader_compute.setFloat("disp3.z", disp3[2]);
                shader_compute.setFloat("disp3.w", disp3[3]);
                shader_compute.setFloat("disp3.e", 0.0);

                if (flag_second_rotate_2D) {
                    shader_compute.setMat4("rot_unfolding_3D_2", rot_mat_unfolding_3D_2);
                    shader_compute.setBool("flag_second_rotate_2D", true);

                    shader_compute.setFloat("disp4.x", disp4[0]);
                    shader_compute.setFloat("disp4.y", disp4[1]);
                    shader_compute.setFloat("disp4.z", disp4[2]);
                    shader_compute.setFloat("disp4.w", disp4[3]);
                    shader_compute.setFloat("disp4.e", 0.0);
                }
            }
            if (flag_move_to_2D) {
                shader_compute.setBool("flag_move_to_2D", true);
                shader_compute.setFloat("coeff_2", coeff_2);
                shader_compute.setFloat("tesseract_size_2", tesseract_size_2);
                shader_compute.setInt("move_index_2", move_index_2);
            }
            //uniform vec5 camera_4D; TODO

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo[0]);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo[1]);
            glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, atomic_counter);

            //ssbo_idx = !ssbo_idx;

            // Execute compute shader with a 256 x 1 x 1 work group size
            GLuint groups = (vertices.size() / (256 * 1 * 1)) + 1; // +1 because of integer division
            glDispatchCompute(groups, 1, 1);

            // Read atomic counter
            glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
            glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_counter);

            GLuint* counterValue = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
            int vertex_amount = counterValue[0];
            //std::cout << "vertex_amount: " << vertex_amount << "\n";

            counterValue[0] = 0; // reset atomic counter
            glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER); // stop writing to buffer
            // memory barrier, to make sure everything from the compute shader is written
            glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

            flag_move_to_3D = false;
            flag_tesseract_unfolding = false;
            flag_disp2 = false;
            flag_plot_unfolding = false;
            flag_move_to_2D = false;
            flag_second_rotate_2D = false;
            shader_compute.setBool("flag_disp2", false);
            shader_compute.setBool("flag_tesseract_unfolding", false);
            shader_compute.setBool("flag_move_to_3D", false);
            shader_compute.setBool("flag_plots_unfolding", false);
            shader_compute.setBool("flag_move_to_2D", false);
            shader_compute.setBool("flag_second_rotate_2D", false);
        }
    }

    //--------------------Unfolding---------------------------------------------

    void tesseract_unfolding(boost::numeric::ublas::matrix<float>& rot, Scene_vertex_t& disp) {
        if (flag_use_computeshader) {
            this->rot_mat_unfolding = rot;
            this->disp = disp;
            this->flag_tesseract_unfolding = true;
        }
        else {
            //for helper if cpu is used
            this->rot_mat_unfolding = rot;

            int index = 0;
            for (Vertex &v : vertices) {
                Scene_vertex_t tmp_vert(5);
                tmp_vert[0] = v.Position.x;
                tmp_vert[1] = v.Position.y;
                tmp_vert[2] = v.Position.z;
                tmp_vert[3] = v.Position.w;
                tmp_vert[4] = v.hmg_coordinate;

                for (int i = 0; i < 5; ++i)
                    tmp_vert(i) += disp(i);

                tmp_vert = prod(tmp_vert, rot);

                for (int i = 0; i < 5; ++i)
                    tmp_vert(i) -= disp(i);

                vertices.at(index).Position = glm::vec4(tmp_vert(0), tmp_vert(1), tmp_vert(2), tmp_vert(3));
                vertices.at(index++).hmg_coordinate = tmp_vert(4);
            }
            flag_update = true;
        }
    }

    void tesseract_unfolding_helper(Scene_vertex_t &disp_2) {
        if (flag_use_computeshader) {
            this->disp2 = disp_2;
            flag_disp2 = true;
        }
        else {
            tesseract_unfolding(this->rot_mat_unfolding, disp_2);
        }
    }

    void move_to_3D_plots(float coeff, float tesseract_size, int index) {
        if (flag_use_computeshader) {
            this->coeff = coeff;
            this->tesseract_size = tesseract_size;
            this->move_index = index;
            this->flag_move_to_3D = true;

        }else{
            for (Vertex& v : vertices) {
                Scene_vertex_t tmp_vert(5);
                tmp_vert[0] = v.Position.x;
                tmp_vert[1] = v.Position.y;
                tmp_vert[2] = v.Position.z;
                tmp_vert[3] = v.Position.w;
                tmp_vert[4] = v.hmg_coordinate;

                //apply translation and scaling here and not in apply_transforms_and_project_vertices_to_3D()
                tmp_vert += translate;

                for (int i = 0; i < tmp_vert.size(); i++)
                {
                    tmp_vert[i] *= scale[i];
                }

                v.Position = glm::vec4(tmp_vert(0), tmp_vert(1), tmp_vert(2), tmp_vert(3));
                v.hmg_coordinate = tmp_vert(4);

                v.Position[index] = v.Position[index] + coeff * (tesseract_size / 2 - v.Position[index]);
            }
            flag_update = true;
        }
    }

    void plots_unfolding(boost::numeric::ublas::matrix<float>& rot, Scene_vertex_t disp) {
        if (flag_use_computeshader) {
            for (int i = 0; i < rot.size1(); ++i) {
                for (int ii = 0; ii < rot.size2(); ++ii) {
                    rot_mat_unfolding_3D[i][ii] = rot(i, ii);
                }
            }

            this->disp3 = disp;
            
            this->flag_plot_unfolding = true;
        }
        else {
            int index = 0;
            for (Vertex& v : vertices) {
                Scene_vertex_t tmp_vert(4);
                tmp_vert[0] = v.Position_3D.x;
                tmp_vert[1] = v.Position_3D.y;
                tmp_vert[2] = v.Position_3D.z;
                tmp_vert[3] = v.Position_3D.w;

                for (int i = 0; i < 4; ++i)
                    tmp_vert(i) += disp(i);

                tmp_vert = prod(tmp_vert, rot);

                for (int i = 0; i < 4; ++i)
                    tmp_vert(i) -= disp(i);

                vertices.at(index++).Position_3D = glm::vec4(tmp_vert(0), tmp_vert(1), tmp_vert(2), tmp_vert(3));
            }
            flag_update = true;
        }
    }

    void plots_unfolding_helper(boost::numeric::ublas::matrix<float>& rot, Scene_vertex_t& disp_2) {
        if (flag_use_computeshader) {

            this->disp4 = disp_2;

            for (int i = 0; i < rot.size1(); ++i) {
                for (int ii = 0; ii < rot.size2(); ++ii) {
                    rot_mat_unfolding_3D_2[i][ii] = rot(i, ii);
                }
            }
            flag_second_rotate_2D = true;
        }
        else {
            plots_unfolding(rot, disp_2);
        }
    }

    void move_to_2D_plot(float coeff, float tesseract_size, int index) {
        if (flag_use_computeshader) {

            this->coeff_2 = coeff;
            this->tesseract_size_2 = tesseract_size;
            this->move_index_2 = index;
            this->flag_move_to_2D = true;
        }
        else {
            for (Vertex& v : vertices) {
                v.Position[index] = v.Position[index] + coeff * (tesseract_size / 2 - v.Position[index]);
            }
            flag_update = true;
        }
    }

    //-----------Setter---------------

    void set_projection_camera(boost::numeric::ublas::matrix<float>& projection_4D, Scene_vertex_t& camera_4D) {
        if (!matrix_equal(this->projection_4D, projection_4D)) {
            this->projection_4D = projection_4D;
            flag_update = true;
        }
        this->camera_4D = camera_4D;
    }

    void set_rotation(boost::numeric::ublas::matrix<float>& rot_mat) {
        if (!matrix_equal(this->rot_mat, rot_mat)) {
            this->rot_mat = rot_mat;
            flag_update = true;
        }
    }

    void translate_vertices(Scene_vertex_t& translate) {
        this->translate = translate;
    }
    void translate_vertices3D(glm::vec3 translate) {
        glm::translate(this->transform3D, translate);
    }

    void scale_vertices(Scene_vertex_t& scale) {
        this->scale = scale;
    }

    void set_update_flag() {
        this->flag_update = true;
    }

    void set_compute_flag() {
        this->flag_use_computeshader = true;
    }

    void disable_compute_flag() {
        this->flag_use_computeshader = false;
    }
    //-------Initialization---------

    // initializes all the buffer objects/arrays
    void Drawable_Streamsurface::setup_mesh()
    {
        if (flag_use_computeshader){
            glGenVertexArrays(2, vaos);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glGenBuffers(1, &ssbo[0]);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[0]);
            //glBufferData(GL_SHADER_STORAGE_BUFFER, vertices.size() * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
            glBufferData(GL_SHADER_STORAGE_BUFFER, positions.size() * sizeof(glm::vec4), &positions[0], GL_STATIC_DRAW);


            glGenBuffers(1, &ssbo[1]);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[1]);
            glBufferData(GL_SHADER_STORAGE_BUFFER, positions.size() * sizeof(glm::vec4), &positions[0], GL_STATIC_DRAW);
            
            //create atomic counter
            glGenBuffers(1, &atomic_counter);
            glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_counter);
            glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
            GLuint value = 0;
            glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &value);
            glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

            //--------------------------------
            glBindVertexArray(vaos[0]);
            glBindBuffer(GL_ARRAY_BUFFER, ssbo[0]);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

            //--------------------------------
            glBindVertexArray(vaos[1]);
            glBindBuffer(GL_ARRAY_BUFFER, ssbo[1]);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);


            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

            // vertex color
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

            // vertex normal
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        }
        else {
            apply_transforms_and_project_vertices_to_3D();

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
    }



    void get_boundaries(Scene_vertex_t& origin, Scene_vertex_t& size)
    {
        // Finding minimum and maximum values of the curve
        glm::vec4 min;
        glm::vec4 max;

        if (vertices.size() > 0)
        {
            const auto& first = vertices.at(0).Position;
            min = first;
            max = first;
        }

        for (const auto& v : vertices)
        {
            const auto& position = v.Position;
            for (unsigned char i = 0; i < 4; ++i)
            {
                if (position[i] < min[i])
                    min[i] = position[i];
                if (position[i] > max[i])
                    max[i] = position[i];
            }
        }

        origin[0] = min.x;
        origin[1] = min.y;
        origin[2] = min.z;
        origin[3] = min.w;
        origin[4] = 1;

        auto diff = max - min;
        size[0] = diff.x;
        size[1] = diff.y;
        size[2] = diff.z;
        size[3] = diff.w;
        size[4] = 1;
    }

private:

    unsigned int VAO, VBO, EBO;
    GLuint tbo;
    boost::numeric::ublas::matrix<float> projection_4D;
    boost::numeric::ublas::matrix<float> rot_mat;
    boost::numeric::ublas::matrix<float> rot_mat_unfolding;
    glm::mat4 rot_mat_unfolding_3D = glm::mat4(1.0);
    glm::mat4 rot_mat_unfolding_3D_2 = glm::mat4(1.0);
    Scene_vertex_t camera_4D;
    Scene_vertex_t disp;
    Scene_vertex_t disp2;
    Scene_vertex_t disp3;
    Scene_vertex_t disp4;
    Scene_vertex_t translate;
    Scene_vertex_t scale;
    glm::mat4 transform3D = glm::mat4(1.0f);
    bool flag_update = false;
    bool flag_tesseract_unfolding = false;
    bool flag_move_to_3D = false;
    bool flag_disp2 = false;
    bool flag_plot_unfolding = false;
    bool flag_move_to_2D = false;
    bool flag_second_rotate_2D = false;

    float coeff = 0.0f;
    float coeff_2 = 0.0f;
    float tesseract_size = 200.0f;
    float tesseract_size_2 = 200.0f;
    GLuint move_index = 0;
    GLuint move_index_2 = 0;

    //compute shader 
    unsigned int ssbo [2];
    unsigned int ssbo_idx = 0;
    GLuint vaos[2]; // one VAO for each SSBO
    bool flag_use_computeshader = true;
    unsigned int atomic_counter;


    bool matrix_equal(const boost::numeric::ublas::matrix<float>& m, const boost::numeric::ublas::matrix<float>& n)
    {
        bool returnValue =
            (m.size1() == n.size1()) &&
            (m.size2() == n.size2());

        if (returnValue)
        {
            for (unsigned int i = 0; returnValue && i < m.size1(); ++i)
            {
                for (unsigned int j = 0; returnValue && j < m.size2(); ++j)
                {
                    returnValue &= m(i, j) == n(i, j);
                }
            }
        }
        return returnValue;
    }


    void enableBlendMode() {
        glEnable(GL_BLEND); // activate blending
        glDepthMask(GL_TRUE); // disable writing to depth buffer
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
    }
    void disableBlendMode() {
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }


};

#endif