#include "model/parse_obj.h"

#include "render/render.h"

#include "image.h"

#include "geometry/geometry.h"

#include "util/string_ops.h"

#include <unordered_map>
#include <fstream>
#include <sstream>

namespace ppp
{
    namespace model
    {
        std::unordered_map<std::string, material> parse_mtl(const std::string& mtl_path) 
        {
            std::unordered_map<std::string, material> materials;

            std::ifstream file(mtl_path);
            if (!file.is_open()) 
            {
                throw std::runtime_error("Failed to open MTL file: " + mtl_path);
            }

            std::string line;
            std::string current_material;

            while (std::getline(file, line)) 
            {
                std::istringstream iss(line);
                std::string token;

                iss >> token;

                if (token == "newmtl") 
                {
                    iss >> current_material;
                    materials[current_material] = material();
                }
                else if (token == "Kd") 
                {
                    f32 r, g, b;
                    iss >> r >> g >> b;
                    materials[current_material].diffuse_color = glm::vec4(r, g, b, 1.0f);
                }
                else if (token == "Ka") 
                {
                    f32 r, g, b;
                    iss >> r >> g >> b;
                    materials[current_material].ambient_color = glm::vec4(r, g, b, 1.0f);
                }
                else if (token == "Ks") 
                {
                    f32 r, g, b;
                    iss >> r >> g >> b;
                    materials[current_material].specular_color = glm::vec4(r, g, b, 1.0f);
                }
                else if (token == "map_Kd") 
                {
                    std::string texture_path;
                    iss >> texture_path;

                    auto image = image::load(texture_path);
                    if (image.id == 0)
                    {
                        log::error("Failed to load image at path: {}", texture_path);
                        exit(EXIT_FAILURE);
                    }

                    materials[current_material].texture_ids.push_back(image.id);
                }
            }

            return materials;
        }

        geometry::geometry* parse_obj(geometry::geometry* geom, const std::vector<std::string> buffer, std::unordered_map<std::string, material> materials)
        {
            std::unordered_map<std::string, std::unordered_map<std::string, s32>> used_verts;

            std::unordered_map<std::string, std::vector<glm::vec3>> loaded_verts;
            std::unordered_map<std::string, std::vector<glm::vec2>> loaded_tex;

            std::string current_material;

            for (const auto& line : buffer) 
            {
                std::vector<std::string> tokens = string::split_string(line, " \t\n\r\f\v");

                if (tokens.empty())
                {
                    continue;
                }

                if (tokens[0] == "usemtl")
                {
                    current_material = tokens[1];
                }
                else if (tokens[0] == "v" || tokens[0] == "vn")
                {
                    f32 x = std::stof(tokens[1]);
                    f32 y = std::stof(tokens[2]);
                    f32 z = std::stof(tokens[3]);

                    loaded_verts[tokens[0]].emplace_back(x, y, z);
                }
                else if (tokens[0] == "vt")
                {
                    f32 u = std::stof(tokens[1]);
                    f32 v = 1.0f - std::stof(tokens[2]);

                    loaded_tex[tokens[0]].emplace_back(u, v);
                }
                else if (tokens[0] == "f")
                {
                    for (u64 tri = 3; tri < tokens.size(); ++tri) 
                    {
                        std::vector<u32> face;
                        std::vector<u64> vertex_tokens = { 1, tri - 1, tri };

                        for (const auto& token_index : vertex_tokens) 
                        {
                            std::string vert_string = tokens[token_index];

                            std::vector<std::string> vert_parts_s = string::split_string(vert_string, "/");
                            std::vector<s32> vert_parts;

                            for(const auto& part_s : vert_parts_s)
                            {
                                vert_parts.push_back(part_s.empty() ? 0 : std::stoi(part_s) - 1);
                            }

                            if (used_verts[vert_string].find(current_material) == used_verts[vert_string].end()) 
                            {
                                s32 vert_index = geom->vertex_positions().size();

                                geom->vertex_positions().push_back(loaded_verts["v"][vert_parts[0]]);

                                if (!loaded_tex["vt"].empty() && loaded_tex["vt"].size() >= vert_parts[1])
                                {
                                    geom->vertex_uvs().push_back(loaded_tex["vt"][vert_parts[1]]);
                                }
                                else 
                                {
                                    geom->vertex_uvs().emplace_back(0.0f, 0.0f);
                                }

                                if (!loaded_verts["vn"].empty() && loaded_verts["vn"].size() >= vert_parts[2])
                                {
                                    geom->vertex_normals().push_back(loaded_verts["vn"][vert_parts[2]]);
                                }
                                else 
                                {
                                    geom->vertex_normals().emplace_back(0.0f, 0.0f, 0.0f);
                                }

                                used_verts[vert_string][current_material] = vert_index;

                                face.push_back(vert_index);
                            }
                            else 
                            {
                                face.push_back(used_verts[vert_string][current_material]);
                            }
                        }

                        if (face[0] != face[1] && face[0] != face[2] && face[1] != face[2]) 
                        {
                            render::face f = { face[0],face[1],face[2] };

                            geom->faces().push_back(f);
                        }
                    }
                }
            }

            if (geom->vertex_normals().empty()) 
            {
                geom->compute_normals();
            }

            return geom;
        }
    }
}