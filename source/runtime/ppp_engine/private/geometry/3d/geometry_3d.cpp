#include "geometry/3d/geometry_3d.h"
#include "geometry/geometry_helpers.h"

#include "constants.h"

#include <array>
#include <vector>

namespace ppp
{
    namespace geometry
    {
        namespace internal
        {
            struct point_3d_data
            {
                glm::vec3 vertex;
                render::index index = { 0 };
            } _point_data;

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
                std::vector<glm::vec2> texcoords;
                std::vector<glm::vec3> normals;
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

            std::vector<glm::vec3>& make_sphere_vertices(f32 radiusX, f32 radiusY, f32 radiusZ, s32 detailX, s32 detailY) {
                _sphere_data.vertices.clear();
                std::vector<glm::vec3>& vertices = _sphere_data.vertices;

                for (s32 i = 0; i <= detailY; i++) {
                    f32 v = static_cast<f32>(i) / detailY;
                    f32 phi = constants::pi() * v - constants::pi() / 2;
                    f32 cosPhi = std::cos(phi);
                    f32 sinPhi = std::sin(phi);

                    for (s32 j = 0; j <= detailX; j++) {
                        f32 u = static_cast<f32>(j) / detailX;
                        f32 theta = 2 * constants::pi() * u;
                        f32 cosTheta = std::cos(theta);
                        f32 sinTheta = std::sin(theta);

                        // Scale the sphere's vertex to form an ellipsoid
                        glm::vec3 p(radiusX * cosPhi * sinTheta, radiusY * sinPhi, radiusZ * cosPhi * cosTheta);
                        vertices.push_back(p);
                    }
                }
                return vertices;
            }

            std::vector<glm::vec2>& make_sphere_texcoords(s32 detailX, s32 detailY) {
                std::vector<glm::vec2>& uvs = _sphere_data.texcoords;

                for (s32 i = 0; i <= detailY; i++) {
                    f32 v = static_cast<f32>(i) / detailY;

                    for (s32 j = 0; j <= detailX; j++) {
                        f32 u = static_cast<f32>(j) / detailX;
                        uvs.emplace_back(u, v);
                    }
                }
                return uvs;
            }

            std::vector<glm::vec3>& make_sphere_normals(f32 radiusX, f32 radiusY, f32 radiusZ, s32 detailX, s32 detailY) {
                _sphere_data.normals.clear();
                std::vector<glm::vec3>& normals = _sphere_data.normals;

                for (s32 i = 0; i <= detailY; i++) {
                    f32 v = static_cast<f32>(i) / detailY;
                    f32 phi = constants::pi() * v - constants::pi() / 2;
                    f32 cosPhi = std::cos(phi);
                    f32 sinPhi = std::sin(phi);

                    for (s32 j = 0; j <= detailX; j++) {
                        f32 u = static_cast<f32>(j) / detailX;
                        f32 theta = 2 * constants::pi() * u;
                        f32 cosTheta = std::cos(theta);
                        f32 sinTheta = std::sin(theta);

                        // Calculate the normal by creating a unit vector
                        glm::vec3 normal(cosPhi * sinTheta, sinPhi, cosPhi * cosTheta);
                        normals.push_back(glm::normalize(normal));
                    }
                }
                // Normalize each accumulated vertex normal
                for (auto& normal : normals)
                {
                    if (glm::length(normal) > 0.0f)
                    {
                        normal = glm::normalize(normal);
                    }
                }
                return normals;
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

            std::vector<unsigned int>& make_sphere_indices(s32 detailX, s32 detailY) {
                _sphere_data.indices.clear();
                std::vector<unsigned int>& indices = _sphere_data.indices;

                for (s32 i = 0; i < detailY; ++i) {
                    for (s32 j = 0; j < detailX; ++j) {
                        // Calculate the four corners of each quad
                        unsigned int topLeft = i * (detailX + 1) + j;
                        unsigned int topRight = topLeft + 1;
                        unsigned int bottomLeft = (i + 1) * (detailX + 1) + j;
                        unsigned int bottomRight = bottomLeft + 1;

                        // First Triangle: (bottomLeft, topLeft, topRight)
                        indices.push_back(bottomLeft);
                        indices.push_back(topLeft);
                        indices.push_back(topRight);

                        // Second Triangle: (bottomRight, bottomLeft, topRight)
                        indices.push_back(bottomRight);
                        indices.push_back(bottomLeft);
                        indices.push_back(topRight);

                    }
                }

                return indices;
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

        render::render_item make_cylinder(f32 radius, f32 height, bool smooth_normals, s32 detail, bool bottom_cap, bool top_cap)
        {
            auto& vertices = internal::make_cylinder_vertices(radius, height, detail);
            auto& indices = internal::make_cylinder_indices(height, detail, bottom_cap, top_cap);
            auto& texcoords = internal::make_cylinder_texcoords(detail, bottom_cap, top_cap);
            auto normals = compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

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
            auto& vertices = internal::make_sphere_vertices(radius, radius, radius, detail, detail);
            auto& indices = internal::make_sphere_indices(detail, detail);
            auto& texcoords = internal::make_sphere_texcoords(detail, detail);
            auto& normals = internal::make_sphere_normals(radius, radius, radius, detail, detail);

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
            auto normals = compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

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
            auto normals = compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

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
            auto normals = compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

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
            auto normals = compute_normals(vertices.data(), vertices.size(), indices.data(), indices.size(), smooth_normals);

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