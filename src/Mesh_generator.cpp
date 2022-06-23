#include "Mesh_generator.h"
// Local
#include "Consts.h"
// glm
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <cassert>

#include "Streamsurface.h"

//******************************************************************************
// cylinder
//******************************************************************************

Mesh Mesh_generator::cylinder(
    unsigned int num_verts,
    float start_diameter,
    float end_diameter,
    const glm::vec3& start_point,
    const glm::vec3& end_point,
    const glm::vec4& color)
{
    Mesh cylinder_mesh;
    cylinder(
        num_verts,
        start_diameter,
        end_diameter,
        start_point,
        end_point,
        color,
        cylinder_mesh);
    return cylinder_mesh;
}

//******************************************************************************
// cylinder
//******************************************************************************

void Mesh_generator::cylinder(
    unsigned int num_verts,
    float start_diameter,
    float end_diameter,
    const glm::vec3& start_point,
    const glm::vec3& end_point,
    const glm::vec4& color,
    Mesh& mesh)
{
    assert(num_verts >= 3);

    mesh.colors.push_back(color);

    auto dir = end_point - start_point;
    float length = glm::length(dir);

    const glm::vec3 up_vec(0.f, 0.f, 1.f);
    auto rotation = glm::rotation(glm::normalize(up_vec), glm::normalize(dir));

    size_t first_vert = mesh.vertices.size();
    size_t first_norm = mesh.normals.size();

    // creating vertices and normals
    for(unsigned int i = 0; i < num_verts + 1; ++i)
    {
        float angle = static_cast<float>(2 * PI * i / num_verts);

        glm::vec3 lower_vert;
        lower_vert[0] = 0.5f * start_diameter * std::cos(angle);
        lower_vert[1] = 0.5f * start_diameter * std::sin(angle);
        lower_vert[2] = 0.0f;

        glm::vec3 upper_vert;
        upper_vert[0] = 0.5f * end_diameter * std::cos(angle);
        upper_vert[1] = 0.5f * end_diameter * std::sin(angle);
        upper_vert[2] = length;

        glm::vec3 normal;
        normal[0] = lower_vert[0];
        normal[1] = lower_vert[1];
        normal[2] = 0;
        normal = glm::normalize(normal);

        mesh.vertices.push_back(rotation * lower_vert + start_point);
        mesh.vertices.push_back(rotation * upper_vert + start_point);
        mesh.normals.push_back(rotation * normal);
        mesh.normals.push_back(rotation * normal);
    }

    Mesh::Object object;
    // creating faces
    for(unsigned int i = 0; i < num_verts; ++i)
    {
        auto vert_shift = first_vert + 2 * i;
        auto norm_shift = first_norm + 2 * i;

        Mesh::Object::FaceType f1;
        f1.emplace_back(Mesh::Vertex(vert_shift, norm_shift, 0));
        f1.emplace_back(Mesh::Vertex(vert_shift + 2, norm_shift + 2, 0));
        f1.emplace_back(Mesh::Vertex(vert_shift + 1, norm_shift + 1, 0));

        Mesh::Object::FaceType f2;
        f2.emplace_back(Mesh::Vertex(vert_shift + 1, norm_shift + 1, 0));
        f2.emplace_back(Mesh::Vertex(vert_shift + 2, norm_shift + 2, 0));
        f2.emplace_back(Mesh::Vertex(vert_shift + 3, norm_shift + 3, 0));

        object.faces.push_back(f1);
        object.faces.push_back(f2);
    }

    mesh.objects.push_back(object);
}

void Mesh_generator::cylinder_v2(
    unsigned int num_verts,
    float start_diameter,
    float end_diameter,
    const glm::vec3& start_point,
    const glm::vec3& end_point,
    const glm::vec3& start_dir,
    const glm::vec3& end_dir,
    const glm::vec4& color,
    Mesh& mesh)
{
    assert(num_verts >= 3);
    auto dir = end_point - start_point;
    const auto norm_dir = glm::normalize(dir);
    // avoid very sharp angles
    if(glm::dot(start_dir, norm_dir) < 0.1f ||
       glm::dot(end_dir, norm_dir) < 0.1f)
    {
        return cylinder(
            num_verts,
            start_diameter,
            end_diameter,
            start_point,
            end_point,
            color,
            mesh);
    }

    mesh.colors.push_back(color);

    float length = glm::length(dir);

    const glm::vec3 up_vec(0.f, 0.f, 1.f);
    auto rotation = glm::rotation(glm::normalize(up_vec), norm_dir);

    const size_t first_vert = mesh.vertices.size();
    const size_t first_norm = mesh.normals.size();

    const float s_d = -glm::dot(start_dir, start_point);
    const float e_d = -glm::dot(end_dir, end_point);
    // creating vertices and normals
    for(unsigned int i = 0; i < num_verts + 1; ++i)
    {
        float angle = static_cast<float>(2 * PI * i / num_verts);

        glm::vec3 lower_vert;
        lower_vert[0] = 0.5f * start_diameter * std::cos(angle);
        lower_vert[1] = 0.5f * start_diameter * std::sin(angle);
        lower_vert[2] = 0.0f;

        glm::vec3 upper_vert;
        upper_vert[0] = 0.5f * end_diameter * std::cos(angle);
        upper_vert[1] = 0.5f * end_diameter * std::sin(angle);
        upper_vert[2] = length;

        glm::vec3 normal;
        normal[0] = lower_vert[0];
        normal[1] = lower_vert[1];
        normal[2] = 0;
        normal = glm::normalize(normal);

        // intersect ray with planes bisecting joints
        // then adjust points to the intersection points
        const glm::vec3 s = rotation * lower_vert + start_point;
        const glm::vec3 e = rotation * upper_vert + start_point;
        const glm::vec3 dir = e - s;
        const float t_s =
            -(glm::dot(start_dir, s) + s_d) / glm::dot(start_dir, dir);
        const float t_e =
            -(glm::dot(end_dir, s) + e_d) / glm::dot(end_dir, dir);
        lower_vert = s + dir * t_s;
        upper_vert = s + dir * t_e;

        mesh.vertices.push_back(lower_vert);
        mesh.vertices.push_back(upper_vert);
        mesh.normals.push_back(rotation * normal);
        mesh.normals.push_back(rotation * normal);
    }

    Mesh::Object object;
    // creating faces
    for(unsigned int i = 0; i < num_verts; ++i)
    {
        auto vert_shift = first_vert + 2 * i;
        auto norm_shift = first_norm + 2 * i;

        Mesh::Object::FaceType f1;
        f1.emplace_back(Mesh::Vertex(vert_shift, norm_shift, 0));
        f1.emplace_back(Mesh::Vertex(vert_shift + 2, norm_shift + 2, 0));
        f1.emplace_back(Mesh::Vertex(vert_shift + 1, norm_shift + 1, 0));

        Mesh::Object::FaceType f2;
        f2.emplace_back(Mesh::Vertex(vert_shift + 1, norm_shift + 1, 0));
        f2.emplace_back(Mesh::Vertex(vert_shift + 2, norm_shift + 2, 0));
        f2.emplace_back(Mesh::Vertex(vert_shift + 3, norm_shift + 3, 0));

        object.faces.push_back(f1);
        object.faces.push_back(f2);
    }

    mesh.objects.push_back(object);
}

Mesh Mesh_generator::cylinder_v2(
    unsigned int num_verts,
    float start_diameter,
    float end_diameter,
    const glm::vec3& start_point,
    const glm::vec3& end_point,
    const glm::vec3& start_dir,
    const glm::vec3& end_dir,
    const glm::vec4& color)
{
    Mesh cylinder_mesh;
    cylinder_v2(
        num_verts,
        start_diameter,
        end_diameter,
        start_point,
        end_point,
        start_dir,
        end_dir,
        color,
        cylinder_mesh);
    return cylinder_mesh;
}

//******************************************************************************
// sphere
//******************************************************************************

Mesh Mesh_generator::sphere(
    unsigned int segments,
    unsigned int rings,
    float diameter,
    const glm::vec3& position,
    const glm::vec4& color)
{
    Mesh sphere_mesh;
    sphere(segments, rings, diameter, position, color, sphere_mesh);
    return sphere_mesh;
}

//******************************************************************************
// sphere
//******************************************************************************

void Mesh_generator::sphere(
    unsigned int segments,
    unsigned int rings,
    float diameter,
    const glm::vec3& position,
    const glm::vec4& color,
    Mesh& mesh)
{
    mesh.colors.push_back(color);

    size_t first_vert = mesh.vertices.size();
    size_t first_norm = mesh.normals.size();

    for(unsigned int i = 0; i < segments + 1; ++i)
    {
        for(unsigned int j = 0; j < rings + 1; ++j)
        {
            float alpha = static_cast<float>(PI * i / segments);
            float betta = static_cast<float>(2 * PI * j / rings);

            glm::vec3 vert;
            vert[0] = diameter * std::sin(alpha) * cos(betta);
            vert[1] = diameter * std::sin(alpha) * sin(betta);
            vert[2] = diameter * std::cos(alpha);

            mesh.vertices.push_back(vert + position);
            mesh.normals.push_back(glm::normalize(vert));
        }
    }

    auto vert_index = [&](unsigned int i, unsigned int j) {
        return (first_vert + i * (rings + 1) + j);
    };
    auto norm_index = [&](unsigned int i, unsigned int j) {
        return (first_norm + i * (rings + 1) + j);
    };

    Mesh::Object object;
    // Optimization: resize the face array to its final size
    object.faces.resize(2 * segments * rings);

    for(unsigned int i = 0; i < segments; ++i)
    {
        for (unsigned int j = 0; j < rings; ++j)
        {
            Mesh::Object::FaceType& f1 = object.faces[2 * (i * rings + j)];
            f1.emplace_back(Mesh::Vertex(vert_index(i, j), norm_index(i, j), 0));
            f1.emplace_back(
                Mesh::Vertex(vert_index(i, j + 1), norm_index(i, j + 1), 0));
            f1.emplace_back(
                Mesh::Vertex(vert_index(i + 1, j), norm_index(i + 1, j), 0));

            Mesh::Object::FaceType& f2 = object.faces[2 * (i * rings + j) + 1];
            f2.emplace_back(Mesh::Vertex(
                vert_index(i + 1, j + 1), norm_index(i + 1, j + 1), 0));
            f2.emplace_back(
                Mesh::Vertex(vert_index(i + 1, j), norm_index(i + 1, j), 0));
            f2.emplace_back(
                Mesh::Vertex(vert_index(i, j + 1), norm_index(i, j + 1), 0));
        }
    }

    mesh.objects.push_back(object);
}

//******************************************************************************
// surface
//******************************************************************************

float calculate_triangle_area(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    auto c1 = (v2 - v1);
    auto c2 = (v3 - v1);
    auto r = glm::cross(c1, c2);
    return sqrt(r.x * r.x + r.y * r.y + r.z * r.z) / 2.0f;
}

float angle_between_vectors(glm::vec3 v1, glm::vec3 v2) {
    v1 = glm::normalize(v1);
    v2 = glm::normalize(v2);
    auto d = glm::dot(v1, v2);
    return acos(d) * (180.0 / 3.141592653589793238463);
}

float calculate_opacity(glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3, glm::vec3 camera_pos, glm::vec3 normal, float surface_height, float distance) {
    //alpha_density
    float area = calculate_triangle_area(v1,v2,v3);
    float angle = angle_between_vectors(camera_pos, normal);
    float density = std::clamp(surface_height / (area * angle), 0.0f, 1.0f);
    //alpha_shape
    float d0 = glm::length(v3 - v2);
    float d1 = glm::length(v1 - v3);
    float d2 = glm::length(v2 - v1);
    std::vector<float> distances{ d0, d1, d2 };
    float max_distance = *std::max_element(distances.begin(), distances.end());
    //own to make large triangles less opaque
    float exponent = std::clamp(max_distance / distance, 0.1f,1.0f);
    float shape = pow((4.0f * area) / (sqrt(3) * max_distance), exponent);

    return std::clamp(density * shape, 0.1f, 1.0f);
}

void Mesh_generator::surface(Streamsurface &s, Mesh &surface_mesh,const Color &c, glm::vec3 &camera_pos, bool use_distance_treshold, float surface_height) {

    Mesh::Object surface_mesh_object;
    //surface_mesh.colors.push_back(glm::vec4(c.r() / 255.0f, c.g() / 255.0f, c.b() / 255.0f, 1.0f));

    //set distance threshold for edges
    auto v1 = s.get_vertices().at(0);
    auto v2 = s.get_vertices().at(4);
    float distance_threshold = glm::distance(glm::vec3(v1(0), v1(1), v1(2)), glm::vec3(v2(0), v2(1), v2(2)));
    int color_index = 0;
    int normal_index = 0;
    
    for (int i = 0; i < s.get_vertices().size(); i++) {
        auto current = s.get_vertices().at(i);
        surface_mesh.vertices.push_back(glm::vec3(current(0), current(1), current(2)));

        //After first vertex strip 
        if(i >= s.length){
            // not the last vertex of the strip
            if (i % s.length != s.length-1) {

                // check length between the vertices in the previous strip
                auto v1 = surface_mesh.vertices.at(i - s.length + 1);
                auto v2 = surface_mesh.vertices.at(i - s.length);
                float distance = glm::distance(v1, v2);

                if (!use_distance_treshold || distance < distance_threshold) {
                    Mesh::Object::FaceType f1;
                    f1.emplace_back(Mesh::Vertex(i - s.length + 1, normal_index, color_index));
                    f1.emplace_back(Mesh::Vertex(i - s.length, normal_index, color_index));
                    f1.emplace_back(Mesh::Vertex(i, normal_index, color_index));
                    surface_mesh_object.faces.push_back(f1);
                }
                glm::vec3 normal = glm::cross(
                    surface_mesh.vertices.at(i),
                    surface_mesh.vertices.at(i - s.length + 1));
                normal = glm::normalize(normal);
                surface_mesh.normals.push_back(normal);
                normal_index++;

                float opacity = calculate_opacity(v1, v2, surface_mesh.vertices.at(i), camera_pos, normal, surface_height, distance_threshold);
                surface_mesh.colors.push_back(glm::vec4(c.r() / 255.0f, c.g() / 255.0f, c.b() / 255.0f, opacity));
                color_index++;
            }

            //after first point in vertex strip
            if (i % s.length >= 1) {

                // check length to previous vertex in strip
                auto v1 = surface_mesh.vertices.at(i);
                auto v2 = surface_mesh.vertices.at(i - 1);
                float distance = glm::distance(v1, v2);

                if (!use_distance_treshold || distance<distance_threshold){
                    Mesh::Object::FaceType f2;
                    f2.emplace_back(Mesh::Vertex(i, normal_index, color_index));
                    f2.emplace_back(Mesh::Vertex(i - 1, normal_index, color_index));
                    f2.emplace_back(Mesh::Vertex(i - s.length, normal_index, color_index));
                    surface_mesh_object.faces.push_back(f2);
                }
                
                glm::vec3 normal = glm::cross(
                    surface_mesh.vertices.at(i),
                    surface_mesh.vertices.at(i - s.length));
                normal = glm::normalize(normal);
                surface_mesh.normals.push_back(normal);
                normal_index++;
            
                float opacity = calculate_opacity(v1, v2, surface_mesh.vertices.at(i - s.length), camera_pos, normal, surface_height, distance_threshold);
                surface_mesh.colors.push_back(glm::vec4(c.r() / 255.0f, c.g() / 255.0f, c.b() / 255.0f, opacity));
                color_index++;
            }
        }
    }
    surface_mesh.objects.push_back(surface_mesh_object);
}

