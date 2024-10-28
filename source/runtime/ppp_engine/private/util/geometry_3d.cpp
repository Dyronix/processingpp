#include "util/geometry_3d.h"
#include "constants.h"

#include <array>
#include <vector>

namespace ppp
{
    namespace geometry
    {
        namespace internal
        {
            std::vector<glm::vec3> compute_smooth_normals(const glm::vec3* vertices, size_t vertex_count, const u32* indices, size_t index_count)
            {
                std::vector<glm::vec3> vertex_normals(vertex_count, glm::vec3(0.0f));

                for (u64 i = 0; i < index_count; i += 3)
                {
                    u32 idx0 = indices[i];
                    u32 idx1 = indices[i + 1];
                    u32 idx2 = indices[i + 2];

                    // Get the vertices for the current triangle
                    const glm::vec3& v0 = vertices[idx0];
                    const glm::vec3& v1 = vertices[idx1];
                    const glm::vec3& v2 = vertices[idx2];

                    // Calculate the face normal
                    glm::vec3 face_normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

                    // Accumulate the face normal into each vertex normal
                    vertex_normals[idx0] += face_normal;
                    vertex_normals[idx1] += face_normal;
                    vertex_normals[idx2] += face_normal;
                }

                // Normalize each accumulated vertex normal
                for (auto& normal : vertex_normals)
                {
                    if (glm::length(normal) > 0.0f)
                    {
                        normal = glm::normalize(normal);
                    }
                }

                return vertex_normals;
            }

            std::vector<glm::vec3> compute_flat_normals(const glm::vec3* vertices, size_t vertex_count, const u32* indices, size_t index_count)
            {
                std::vector<glm::vec3> flat_normals;
                flat_normals.reserve(index_count); // 1 unique normal per index

                for (u64 i = 0; i < index_count; i += 3)
                {
                    u32 idx0 = indices[i];
                    u32 idx1 = indices[i + 1];
                    u32 idx2 = indices[i + 2];

                    // Get the vertices for the current triangle
                    const glm::vec3& v0 = vertices[idx0];
                    const glm::vec3& v1 = vertices[idx1];
                    const glm::vec3& v2 = vertices[idx2];

                    // Calculate the face normal
                    glm::vec3 face_normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

                    // Assign this face normal to each vertex of the triangle
                    flat_normals.push_back(face_normal);
                    flat_normals.push_back(face_normal);
                    flat_normals.push_back(face_normal);
                }

                return flat_normals;
            }

            std::vector<glm::vec3> compute_normals(const glm::vec3* vertices, size_t vertex_count, const u32* indices, size_t index_count, bool smooth_normals)
            {
                if (smooth_normals)
                {
                    return compute_smooth_normals(vertices, vertex_count, indices, index_count);
                }

                return compute_flat_normals(vertices, vertex_count, indices, index_count);
            }

            struct point_3d_data
            {
                glm::vec3 vertex;
                render::index index = { 0 };
            } _point_data;

            struct box_data
            {
                std::array<glm::vec3, 24> vertices;
                std::array<glm::vec2, 24> texcoords = 
                {
                    // Front face (matches vertices 0, 1, 2, 3)
                    glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
                    // Back face (matches vertices 4, 5, 6, 7)
                    glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
                    // Left face (matches vertices 8, 9, 10, 11)
                    glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
                    // Right face (matches vertices 12, 13, 14, 15)
                    glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
                    // Bottom face (matches vertices 16, 17, 18, 19)
                    glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
                    // Top face (matches vertices 20, 21, 22, 23)
                    glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f}
                };
                std::array<render::index, 36> indices = 
                {
                    // Front face
                    0, 1, 2, 0, 2, 3,
                    // Back face
                    4, 6, 5, 4, 7, 6,
                    // Left face
                    8, 9, 10, 8, 10, 11,
                    // Right face
                    12, 14, 13, 12, 15, 14,
                    // Bottom face
                    16, 18, 17, 16, 19, 18,
                    // Top face
                    20, 21, 22, 20, 22, 23
                };
                std::vector<glm::vec3> normals;

            } _box_data;

            struct cylinder_data
            {
                std::vector<glm::vec3> vertices;
                std::vector<render::index> indices;
            } _cylinder_data;

            struct plane_data
            {
                std::array<glm::vec3, 4> vertices;
                std::array<glm::vec2, 4> texcoords = 
                {
                    glm::vec2(0.0f, 0.0f), // Bottom Left
                    glm::vec2(1.0f, 0.0f), // Bottom Right
                    glm::vec2(1.0f, 1.0f), // Top Right
                    glm::vec2(0.0f, 1.0f)  // Top Left
                };
                std::array<render::index, 6> indices = { 0, 1 ,2, 0, 2, 3 };
                std::array<glm::vec3, 4> normals = 
                {
                    glm::vec3(0, 0, 1),
                    glm::vec3(0, 0, 1),
                    glm::vec3(0, 0, 1),
                    glm::vec3(0, 0, 1)
                };
            } _plane_data;

            struct sphere_data
            {
                std::vector<glm::vec3> vertices;
                std::vector<render::index> indices;
            } _sphere_data;

            struct torus_data
            {
                std::vector<glm::vec3> vertices;
                std::vector<render::index> indices;
            } _torus_data;

            struct cone_data
            {
                std::vector<glm::vec3> vertices;
                std::vector<render::index> indices;
            } _cone_data;

            struct tetrahedron_data
            {
                std::array<glm::vec3, 4> vertices;
                std::array<glm::vec2, 4> texcoords =
                {
                    glm::vec2{ 0.5f, 0.0f }, 
                    glm::vec2{ 1.0f, 1.0f }, 
                    glm::vec2{ 0.0f, 1.0f }, 
                    glm::vec2{ 0.5f, 0.5f }, 
                };
                std::array<render::index, 12> indices = { 0, 1, 2,
                    0, 2, 3,
                    0, 3, 1,
                    1, 3, 2 };
            } _tetrahedron_data;

            struct octahedron_data
            {
                std::array<glm::vec3, 6> vertices;
                std::array<glm::vec2, 6> texcoords = 
                {
                    glm::vec2 { 0.5f, 0.0f },
                    glm::vec2 { 1.0f, 1.0f },
                    glm::vec2 { 0.5f, 1.0f },
                    glm::vec2 { 0.0f, 1.0f },
                    glm::vec2 { 0.5f, 1.0f },
                    glm::vec2 { 0.5f, 1.0f } 
                };
                std::array<render::index, 24> indices = 
                {
                    0, 2, 1,
                    0, 3, 2,
                    0, 4, 3,
                    0, 1, 4,
                    5, 1, 2,
                    5, 2, 3,
                    5, 3, 4,
                    5, 4, 1
                };
            } _octahedron_data;

            std::array<glm::vec3, 24>& make_box_vertices(f32 width, f32 height, f32 depth)
            {    
                u64 index = 0;

                // Front face
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, depth / 2) }; // Bottom Left
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, depth / 2) };  // Bottom Right
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, depth / 2) };   // Top Right
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, depth / 2) };  // Top Left

                // Back face
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, -depth / 2) }; // Bottom Left
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, -depth / 2) };  // Bottom Right
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, -depth / 2) };   // Top Right
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, -depth / 2) };  // Top Left

                // Left face
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, -depth / 2) }; // Bottom Back
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, depth / 2) };  // Bottom Front
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, depth / 2) };   // Top Front
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, -depth / 2) };  // Top Back

                // Right face
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, -depth / 2) };  // Bottom Back
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, depth / 2) };   // Bottom Front
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, depth / 2) };    // Top Front
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, -depth / 2) };   // Top Back

                // Top face
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, -depth / 2) };  // Back Left
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, -depth / 2) };   // Back Right
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, depth / 2) };    // Front Right
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, depth / 2) };   // Front Left

                // Bottom face
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, -depth / 2) }; // Back Left
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, -depth / 2) };  // Back Right
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, depth / 2) };   // Front Right
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, depth / 2) };  // Front Left

                return _box_data.vertices;
            }

            std::array<glm::vec2, 24>& make_box_texture_coordinates()
            {
                return _box_data.texcoords;
            }

            std::vector<glm::vec3>& make_cylinder_vertices(f32 r, f32 h, s32 detail)
            {
                s32 total_nr_vertices = (detail + 1) * 2;

                _cylinder_data.vertices.clear();
                _cylinder_data.vertices.resize(total_nr_vertices);

                _cylinder_data.vertices[0] = glm::vec3{ 0.0f, (+h) / 2, 0.0f};
                _cylinder_data.vertices[total_nr_vertices / 2] = glm::vec3{ 0.0f, (-h) / 2, 0.0f};

                for (s32 t = 1; t < total_nr_vertices / 2; ++t)
                {
                    f32 angle = (t / static_cast<f32>(detail)) * constants::two_pi();

                    f32 v_x = cos(angle) * r;
                    f32 v_z = sin(angle) * r;

                    s32 top_index = t;
                    s32 bottom_index = (total_nr_vertices / 2) + t; 

                    _cylinder_data.vertices[top_index] = glm::vec3(v_x, (+h) / 2, v_z);
                    _cylinder_data.vertices[bottom_index] = glm::vec3(v_x, (-h) / 2, v_z);
                }

                return _cylinder_data.vertices;
            }

            std::vector<glm::vec2> make_cylinder_texcoords(s32 detail, bool bottom_cap, bool top_cap) 
            {
                std::vector<glm::vec2> texcoords;

                if (top_cap) 
                {
                    texcoords.emplace_back(0.5f, 0.5f);
                }
                if (bottom_cap) 
                {
                    texcoords.emplace_back(0.5f, 0.5f);
                }

                const f32 angle_step = 1.0f / detail;
                for (s32 i = 0; i <= detail; ++i) 
                {
                    f32 u = static_cast<f32>(i) * angle_step;
                    texcoords.emplace_back(u, 0.0f);
                    texcoords.emplace_back(u, 1.0f);
                }

                return texcoords;
            }

            std::array<glm::vec3, 4>& make_plane_vertices(f32 width, f32 height)
            {
                _plane_data.vertices[0] = { glm::vec3(-width / 2, -height / 2, 0.0f) }; // Bottom Left
                _plane_data.vertices[1] = { glm::vec3(width / 2, -height / 2, 0.0f) };  // Bottom Right
                _plane_data.vertices[2] = { glm::vec3(width / 2, height / 2, 0.0f) };   // Top Right
                _plane_data.vertices[3] = { glm::vec3(-width / 2, height / 2, 0.0f) };  // Top Left

                return _plane_data.vertices;
            }

            std::array<glm::vec2, 4>& make_plane_texcoords()
            {
                return _plane_data.texcoords;
            }

            std::array<glm::vec3, 4>& make_plane_normals()
            {
                return _plane_data.normals;
            }

            //std::vector<glm::vec3>& make_sphere_vertices(f32 r, s32 detail)
            //{
            //    //_sphere_data.vertices.clear();

            //    //// Iterate through the latitudes and longitudes to create the sphere vertices
            //    //for (s32 lat = 0; lat <= detail; ++lat)
            //    //{
            //    //    f32 theta = lat * constants::pi() / detail; // Latitude angle
            //    //    for (s32 lon = 0; lon <= detail; ++lon)
            //    //    {
            //    //        f32 phi = lon * constants::two_pi() / detail; // Longitude angle

            //    //        // Calculate the x, y, z coordinates
            //    //        f32 x = r * sin(theta) * cos(phi);
            //    //        f32 y = r * cos(theta);
            //    //        f32 z = r * sin(theta) * sin(phi);

            //    //        // Store the vertex
            //    //        _sphere_data.vertices.push_back({ glm::vec3(x, y, z) });
            //    //    }
            //    //}

            //    //return _sphere_data.vertices;

            //    _sphere_data.vertices.clear();

            //    // North pole
            //    _sphere_data.vertices.push_back(glm::vec3(0, r, 0));

            //    for (s32 lat = 1; lat < detail; ++lat) // Start from 1 to avoid the north pole
            //    {
            //        f32 theta = lat * constants::pi() / detail;

            //        for (s32 lon = 0; lon <= detail; ++lon)
            //        {
            //            f32 phi = lon * constants::two_pi() / detail;

            //            f32 x = r * sin(theta) * cos(phi);
            //            f32 y = r * cos(theta);
            //            f32 z = r * sin(theta) * sin(phi);

            //            _sphere_data.vertices.push_back(glm::vec3(x, y, z));
            //        }
            //    }

            //    // South pole
            //    _sphere_data.vertices.push_back(glm::vec3(0, -r, 0));

            //    return _sphere_data.vertices;
            //}

            std::vector<glm::vec3>& make_sphere_vertices(float r, int detail) {
                _sphere_data.vertices.clear();

                // North pole
                _sphere_data.vertices.push_back(glm::vec3(0, r, 0));

                // Create the vertices for the sphere
                for (int lat = 0; lat < detail; ++lat) {
                    float theta = lat * constants::pi() / detail; // Latitude angle

                    for (int lon = 0; lon <= detail; ++lon) {
                        float phi = lon * constants::two_pi() / detail; // Longitude angle

                        // Calculate the x, y, z coordinates
                        float x = r * sin(theta) * cos(phi);
                        float y = r * cos(theta);
                        float z = r * sin(theta) * sin(phi);

                        // Store the vertex
                        _sphere_data.vertices.push_back(glm::vec3(x, y, z));
                    }
                }

                // South pole
                _sphere_data.vertices.push_back(glm::vec3(0, -r, 0));

                return _sphere_data.vertices;
            }

            std::vector<glm::vec2> make_sphere_texcoords(s32 detail)
            {
                std::vector<glm::vec2> texcoords;

                s32 rings = detail;
                s32 sectors = detail * 2;

                for (s32 r = 0; r < rings; ++r)
                {
                    for (s32 s = 0; s < sectors; ++s)
                    {
                        f32 u = (f32)s / (f32)(sectors - 1);
                        f32 v = (f32)r / (f32)(rings - 1);
                        texcoords.emplace_back(u, v);
                    }
                }

                return texcoords;
            }

            std::vector<glm::vec3>& make_torus_vertices(f32 r, f32 tr, s32 detailx, s32 detaily)
            {
                _torus_data.vertices.clear();

                for (s32 i = 0; i < detailx; ++i)
                {
                    f32 theta = i * constants::two_pi() / detailx;

                    for (s32 j = 0; j < detaily; ++j)
                    {
                        f32 phi = j * constants::two_pi() / detaily;
                        f32 x = (r + tr * cos(phi)) * cos(theta);
                        f32 y = (r + tr * cos(phi)) * sin(theta);
                        f32 z = tr * sin(phi);

                        _torus_data.vertices.push_back({ glm::vec3(x, y, z) });
                    }
                }

                return _torus_data.vertices;
            }

            std::vector<glm::vec2> make_torus_texcoords(s32 radial_detail, s32 tubular_detail)
            {
                std::vector<glm::vec2> texcoords;

                for (s32 i = 0; i < radial_detail; ++i)
                {
                    for (s32 j = 0; j < tubular_detail; ++j)
                    {
                        f32 u = (f32)i / (f32)(radial_detail - 1); // U coordinate
                        f32 v = (f32)j / (f32)(tubular_detail - 1); // V coordinate
                        texcoords.emplace_back(u, v);
                    }
                }

                return texcoords;
            }

            std::vector<glm::vec3>& make_cone_vertices(f32 r, f32 h, s32 detail, bool cap)
            {
                // Total vertices: apex (1) + base vertices (detail) + center vertex (1 if cap)
                s32 total_nr_vertices = detail + 1 + (cap ? 1 : 0);

                _cone_data.vertices.clear();
                _cone_data.vertices.resize(total_nr_vertices);

                // Apex vertex (top of the cone)
                _cone_data.vertices[0] = glm::vec3{ 0.0f, h/2, 0.0f }; // Apex

                // Base vertices
                for (s32 t = 1; t < total_nr_vertices - (cap ? 1 : 0); ++t)
                {
                    f32 angle = (t / static_cast<f32>(total_nr_vertices - (cap ? 1 : 0) - 1)) * constants::two_pi();

                    f32 v_x = cos(angle) * r;
                    f32 v_z = sin(angle) * r;

                    _cone_data.vertices[t] = glm::vec3(v_x, -h/2, v_z); // Base vertices
                }

                // If cap is true, add the center vertex for the base
                if (cap)
                {
                    _cone_data.vertices[total_nr_vertices - 1] = glm::vec3{ 0.0f, -h/2, 0.0f }; // Center of the base
                }

                return _cone_data.vertices;
            }

            std::vector<glm::vec2> make_cone_texcoords(s32 detail)
            {
                std::vector<glm::vec2> texcoords;

                for (s32 i = 0; i < detail; ++i)
                {
                    f32 u = (f32)i / (f32)(detail - 1); // U coordinate
                    f32 v = 0.0f; // Bottom vertex will have a constant V coordinate

                    texcoords.emplace_back(u, v);
                }

                // Top vertex (apex) of the cone
                texcoords.emplace_back(0.5f, 1.0f); // Middle of the texture at the top

                return texcoords;
            }

            std::array<glm::vec3, 4>& make_tetrahedron_vertices(f32 w, f32 h)
            {
                _tetrahedron_data.vertices[0] = { glm::vec3(0, h, 0) };
                _tetrahedron_data.vertices[1] = { glm::vec3(-w, -h, w) };
                _tetrahedron_data.vertices[2] = { glm::vec3(w, -h, w) };
                _tetrahedron_data.vertices[3] = { glm::vec3(0, -h, -w) };

                return _tetrahedron_data.vertices;
            }

            std::array<glm::vec2, 4>& make_tetrahedron_texcoords()
            {
                return _tetrahedron_data.texcoords;
            }

            std::array<glm::vec3, 6>& make_octa_hedron_vertices(f32 w, f32 h)
            {
                _octahedron_data.vertices[0] = { glm::vec3(0, h, 0) };
                _octahedron_data.vertices[1] = { glm::vec3(w, 0, 0) };
                _octahedron_data.vertices[2] = { glm::vec3(0, 0, w) };
                _octahedron_data.vertices[3] = { glm::vec3(-w, 0, 0) };
                _octahedron_data.vertices[4] = { glm::vec3(0, 0, -w) };
                _octahedron_data.vertices[5] = { glm::vec3(0, -h, 0) };

                return _octahedron_data.vertices;
            }

            std::array<glm::vec2, 6> make_octa_hedron_texcoords()
            {
                return _octahedron_data.texcoords;
            }

            std::array<render::index, 36>& make_box_indices()
            {
                return _box_data.indices;
            }

            std::vector<render::index>& make_cylinder_indices(f32 h, s32 detail, bool bottom_cap, bool top_cap)
            {
                s32 total_nr_indices = detail * 3;
                s32 total_indices = (top_cap ? total_nr_indices : 0) + (bottom_cap ? total_nr_indices : 0) + detail * 6;

                _cylinder_data.indices.clear();
                _cylinder_data.indices.resize(total_indices);

                s32 i = 0;
                s32 index_offset = 0;

                if (top_cap)
                {
                    for (s32 t = 0; t < detail; ++t)
                    {
                        // Center vertex of the top cap
                        _cylinder_data.indices[index_offset + 0] = 0;
                        // Next perimeter vertex, wrap around to 1 if it's the last vertex
                        _cylinder_data.indices[index_offset + 1] = (t + 2 <= detail) ? (t + 2) : 1;
                        // First perimeter vertex for this triangle (CCW order)
                        _cylinder_data.indices[index_offset + 2] = t + 1;

                        index_offset += 3;
                    }
                }

                s32 bottom_cap_start = _cylinder_data.vertices.size() / 2;
                i = bottom_cap_start;

                if (bottom_cap)
                {
                    for (s32 t = 0; t < detail; ++t)
                    {
                        // Center vertex of the bottom cap
                        _cylinder_data.indices[index_offset + 0] = bottom_cap_start;
                        // Next perimeter vertex, wrap around to 1 if it's the last vertex
                        _cylinder_data.indices[index_offset + 1] = (t + 1) % detail + bottom_cap_start + 1;
                        // First perimeter vertex for this triangle (CCW order)
                        _cylinder_data.indices[index_offset + 2] = (t + 2) % detail + bottom_cap_start + 1;

                        index_offset += 3;
                    }
                }

                // Generate side indices
                s32 side_start = index_offset;
                for (s32 j = 0; j < detail - 1; ++j)
                {
                    // First triangle of quad (Top -> Bottom -> Next Top)
                    _cylinder_data.indices[side_start + 0] = j + 1;                         // Top current vertex
                    _cylinder_data.indices[side_start + 1] = j + 2;                         // Bottom current vertex
                    _cylinder_data.indices[side_start + 2] = bottom_cap_start + j + 1;      // Top next vertex

                    // Second triangle of quad (Bottom -> Next Bottom -> Next Top)
                    _cylinder_data.indices[side_start + 3] = bottom_cap_start + j + 1;      // Bottom current vertex
                    _cylinder_data.indices[side_start + 4] = j + 2;                         // Bottom next vertex
                    _cylinder_data.indices[side_start + 5] = bottom_cap_start + j + 2;      // Top next vertex

                    side_start += 6;
                }

                _cylinder_data.indices[side_start + 0] = detail;                            // Last top perimeter vertex
                _cylinder_data.indices[side_start + 1] = 1;                                 // Last bottom perimeter vertex
                _cylinder_data.indices[side_start + 2] = bottom_cap_start + detail;         // First top perimeter vertex (to close the loop)

                _cylinder_data.indices[side_start + 3] = bottom_cap_start + detail;         // Last bottom perimeter vertex
                _cylinder_data.indices[side_start + 4] = 1;                                 // First bottom perimeter vertex (to close the loop)
                _cylinder_data.indices[side_start + 5] = bottom_cap_start + 1;              // First top perimeter vertex (to close the loop)

                return _cylinder_data.indices;
            }

            std::array<render::index, 6>& make_plane_indices()
            {
                return _plane_data.indices;
            }

            std::vector<render::index>& make_sphere_indices(int detail) {
                _sphere_data.indices.clear();

                // The number of vertices generated (1 for north pole + detail * (detail + 1) + 1 for south pole)
                int vertexCount = (detail + 1) * detail + 2; // 2 poles + detail rows of (detail + 1) vertices

                // Create the indices for the sphere in CCW order
                for (int lat = 0; lat < detail; ++lat) {
                    for (int lon = 0; lon < detail; ++lon) {
                        unsigned int first = lat * (detail + 1) + lon;          // Current vertex
                        unsigned int second = first + detail + 1;               // Vertex below (next latitude)
                        unsigned int third = first + 1;                         // Next vertex on the same latitude
                        unsigned int fourth = second + 1;                       // Next vertex on the next latitude

                        // Ensure the indices do not exceed bounds
                        if (lat < detail - 1) {
                            // Create two triangles for each quad in CCW order
                            _sphere_data.indices.push_back(first);  // First triangle
                            _sphere_data.indices.push_back(third);  // Next vertex on the same latitude
                            _sphere_data.indices.push_back(second);  // Vertex below

                            _sphere_data.indices.push_back(second);  // Second triangle
                            _sphere_data.indices.push_back(third);   // Next vertex on the same latitude
                            _sphere_data.indices.push_back(fourth);  // Next vertex on the next latitude
                        }
                    }
                }

                return _sphere_data.indices;
            }

            std::vector<render::index>& make_torus_indices(s32 detailx, s32 detaily)
            {
                _torus_data.indices.clear();

                for (s32 i = 0; i < detailx; ++i)
                {
                    for (s32 j = 0; j < detaily; ++j)
                    {
                        s32 next_i = (i + 1) % detailx;
                        s32 next_j = (j + 1) % detaily;

                        _torus_data.indices.push_back(i * detaily + j);
                        _torus_data.indices.push_back(next_i * detaily + j);
                        _torus_data.indices.push_back(i * detaily + next_j);

                        _torus_data.indices.push_back(next_i * detaily + j);
                        _torus_data.indices.push_back(next_i * detaily + next_j);
                        _torus_data.indices.push_back(i * detaily + next_j);
                    }
                }

                return _torus_data.indices;
            }

            std::vector<render::index>& make_cone_indices(s32 detail, bool cap)
            {
                // Calculate the total number of indices for the sides of the cone
                s32 total_nr_indices = detail * 3;

                // If cap is true, we will add additional indices for the base
                if (cap)
                {
                    total_nr_indices += detail * 3; // Additional indices for base triangles
                }

                _cone_data.indices.clear();
                _cone_data.indices.resize(total_nr_indices);

                s32 i = 0;

                // Create indices for the sides of the cone
                for (s32 t = 0; t < detail * 3; t += 3)
                {
                    _cone_data.indices[t + 0] = 0;
                    _cone_data.indices[t + 1] = i + 2;
                    _cone_data.indices[t + 2] = i + 1;

                    ++i;
                }

                // Loop back to the first triangle
                _cone_data.indices[(detail * 3) - 2] = _cone_data.indices[(detail * 3) - 2] % detail;

                // If cap is true, create indices for the base
                if (cap)
                {
                    i = 0;

                    for (s32 t = total_nr_indices / 2; t < total_nr_indices; t += 3)
                    {
                        _cone_data.indices[t + 0] = _cone_data.vertices.size() - 1;
                        _cone_data.indices[t + 1] = i + 1;
                        _cone_data.indices[t + 2] = i + 2;

                        ++i;
                    }

                    // Loop back to the first triangle
                    _cone_data.indices[total_nr_indices - 1] = 1;
                }

                return _cone_data.indices;
            }

            std::array<render::index, 12>& make_tetrahedron_indices()
            {
                return _tetrahedron_data.indices;
            }

            std::array<render::index, 24>& make_octa_hedron_indices()
            {
                return _octahedron_data.indices;
            }
        }

        render::render_item make_point(f32 x, f32 y, f32 z)
        {
            internal::_point_data.vertex.x = x;
            internal::_point_data.vertex.y = y;
            internal::_point_data.vertex.z = z;

            render::render_item item;

            auto vert_comp = render::make_vertex_component(1);
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, &internal::_point_data.vertex);

            auto idx_comp = render::make_index_component(&internal::_point_data.index, 1);

            item.add_component(std::move(vert_comp));
            item.add_component(std::move(idx_comp));

            return item;
        }

        render::render_item make_box(f32 width, f32 height, f32 depth, bool smooth_normals)
        {
            auto& indices = internal::make_box_indices();
            auto& vertices = internal::make_box_vertices(width, height, depth);
            auto& texcoords = internal::make_box_texture_coordinates();
            auto normals = internal::compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

            render::render_item item;

            auto vert_comp = render::make_vertex_component(vertices.size());
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, vertices.data());
            vert_comp->add_attribute(render::vertex_attribute_type::TEXCOORD, texcoords.data());
            vert_comp->add_attribute(render::vertex_attribute_type::NORMAL, normals.data());

            auto idx_comp = render::make_index_component(indices.data(), indices.size());

            item.add_component(std::move(vert_comp));
            item.add_component(std::move(idx_comp));

            return item;
        }

        render::render_item make_cylinder(f32 radius, f32 height, bool smooth_normals, s32 detail, bool bottom_cap, bool top_cap)
        {
            auto& vertices = internal::make_cylinder_vertices(radius, height, detail);
            auto& indices = internal::make_cylinder_indices(height, detail, bottom_cap, top_cap);
            auto& texcoords = internal::make_cylinder_texcoords(detail, bottom_cap, top_cap);
            auto normals = internal::compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

            render::render_item item;

            auto vert_comp = render::make_vertex_component(vertices.size());
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, vertices.data());
            vert_comp->add_attribute(render::vertex_attribute_type::TEXCOORD, texcoords.data());
            vert_comp->add_attribute(render::vertex_attribute_type::NORMAL, normals.data());
            
            auto idx_comp = render::make_index_component(indices.data(), indices.size());

            item.add_component(std::move(vert_comp));
            item.add_component(std::move(idx_comp));

            return item;
        }

        render::render_item make_plane(f32 width, f32 height)
        {
            auto& vertices = internal::make_plane_vertices(width, height);
            auto& indices = internal::make_plane_indices();
            auto& texcoords = internal::make_plane_texcoords();
            auto& normals = internal::make_plane_normals();

            render::render_item item;

            auto vert_comp = render::make_vertex_component(vertices.size());
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, vertices.data());
            vert_comp->add_attribute(render::vertex_attribute_type::TEXCOORD, texcoords.data());
            vert_comp->add_attribute(render::vertex_attribute_type::NORMAL, normals.data());
            
            auto idx_comp = render::make_index_component(indices.data(), indices.size());

            item.add_component(std::move(vert_comp));
            item.add_component(std::move(idx_comp));

            return item;
        }

        render::render_item make_sphere(f32 radius, bool smooth_normals, s32 detail)
        {
            auto& vertices = internal::make_sphere_vertices(radius, detail);
            auto& indices = internal::make_sphere_indices(detail);
            auto& texcoords = internal::make_sphere_texcoords(detail);
            auto normals = internal::compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

            render::render_item item;

            auto vert_comp = render::make_vertex_component(vertices.size());
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, vertices.data());
            vert_comp->add_attribute(render::vertex_attribute_type::TEXCOORD, texcoords.data());
            vert_comp->add_attribute(render::vertex_attribute_type::NORMAL, normals.data());

            auto idx_comp = render::make_index_component(indices.data(), indices.size());

            item.add_component(std::move(vert_comp));
            item.add_component(std::move(idx_comp));

            return item;
        }

        render::render_item make_torus(f32 radius, f32 tube_radius, bool smooth_normals, s32 detailx, s32 detaily)
        {
            auto& vertices = internal::make_torus_vertices(radius, tube_radius, detailx, detaily);
            auto& indices = internal::make_torus_indices(detailx, detaily);
            auto& texcoords = internal::make_torus_texcoords(detailx, detaily);
            auto normals = internal::compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

            render::render_item item;

            auto vert_comp = render::make_vertex_component(vertices.size());
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, vertices.data());
            vert_comp->add_attribute(render::vertex_attribute_type::TEXCOORD, texcoords.data());
            vert_comp->add_attribute(render::vertex_attribute_type::NORMAL, normals.data());
            
            auto idx_comp = render::make_index_component(indices.data(), indices.size());

            item.add_component(std::move(vert_comp));
            item.add_component(std::move(idx_comp));

            return item;
        }

        render::render_item make_cone(f32 radius, f32 height, bool smooth_normals, s32 detail, bool cap)
        {
            auto& vertices = internal::make_cone_vertices(radius, height, detail, cap);
            auto& indices = internal::make_cone_indices(detail, cap);
            auto& texcoords = internal::make_cone_texcoords(detail);
            auto normals = internal::compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

            render::render_item item;

            auto vert_comp = render::make_vertex_component(vertices.size());
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, vertices.data());
            vert_comp->add_attribute(render::vertex_attribute_type::TEXCOORD, texcoords.data());
            vert_comp->add_attribute(render::vertex_attribute_type::NORMAL, normals.data());
            
            auto idx_comp = render::make_index_component(indices.data(), indices.size());

            item.add_component(std::move(vert_comp));
            item.add_component(std::move(idx_comp));

            return item;
        }

        render::render_item make_tetrahedron(f32 width, f32 height, bool smooth_normals)
        {
            auto& vertices = internal::make_tetrahedron_vertices(width, height);
            auto& indices = internal::make_tetrahedron_indices();
            auto& texcoords = internal::make_tetrahedron_texcoords();
            auto normals = internal::compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

            render::render_item item;

            auto vert_comp = render::make_vertex_component(vertices.size());
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, vertices.data());
            vert_comp->add_attribute(render::vertex_attribute_type::TEXCOORD, texcoords.data());
            vert_comp->add_attribute(render::vertex_attribute_type::NORMAL, normals.data());
            
            auto idx_comp = render::make_index_component(indices.data(), indices.size());

            item.add_component(std::move(vert_comp));
            item.add_component(std::move(idx_comp));

            return item;
        }

        render::render_item make_octahedron(f32 width, f32 height, bool smooth_normals)
        {
            auto& vertices = internal::make_octa_hedron_vertices(width, height);
            auto& indices = internal::make_octa_hedron_indices();
            auto& texcoords = internal::make_octa_hedron_texcoords();
            auto normals = internal::compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

            render::render_item item;

            auto vert_comp = render::make_vertex_component(vertices.size());
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, vertices.data());
            vert_comp->add_attribute(render::vertex_attribute_type::TEXCOORD, texcoords.data());
            vert_comp->add_attribute(render::vertex_attribute_type::NORMAL, normals.data());
            
            auto idx_comp = render::make_index_component(indices.data(), indices.size());

            item.add_component(std::move(vert_comp));
            item.add_component(std::move(idx_comp));

            return item;
        }
    }
}