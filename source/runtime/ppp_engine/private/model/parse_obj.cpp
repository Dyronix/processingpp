#include "model/parse_obj.h"

#include "render/render.h"

#include "image.h"

#include "geometry/geometry.h"

#include "util/string_ops.h"

#include <fstream>
#include <sstream>

namespace ppp
{
    namespace model
    {
        geometry::geometry* parse_obj(geometry::geometry* geom, const frame_vector<std::string_view>& buffer)
        {
            frame_hash_map<std::string_view, frame_hash_map<std::string_view, s32>> used_verts;

            frame_hash_map<std::string_view, frame_vector<glm::vec3>> loaded_verts;
            frame_hash_map<std::string_view, frame_vector<glm::vec2>> loaded_tex;

            frame_string current_material;

            for (std::string_view line : buffer) 
            {
                auto tokens = string::split_string<memory::tags::frame>(line, " \t\n\r\f\v");

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
                    f32 x = std::stof(tokens[1].c_str());
                    f32 y = std::stof(tokens[2].c_str());
                    f32 z = std::stof(tokens[3].c_str());

                    loaded_verts[tokens[0]].emplace_back(x, y, z);
                }
                else if (tokens[0] == "vt")
                {
                    f32 u = std::stof(tokens[1].c_str());
                    f32 v = 1.0f - std::stof(tokens[2].c_str());

                    loaded_tex[tokens[0]].emplace_back(u, v);
                }
                else if (tokens[0] == "f")
                {
                    for (u64 tri = 3; tri < tokens.size(); ++tri) 
                    {
                        frame_vector<u32> face;
                        frame_vector<u64> vertex_tokens = { 1, tri - 1, tri };

                        for (const auto& token_index : vertex_tokens) 
                        {
                            std::string_view vert_string = tokens[token_index];

                            auto vert_parts_s = string::split_string<memory::tags::frame>(vert_string, "/");
                            auto vert_parts = frame_vector<s32>{};

                            for(const auto& part_s : vert_parts_s)
                            {
                                vert_parts.push_back(part_s.empty() ? 0 : std::stoi(part_s.c_str()) - 1);
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