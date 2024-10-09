#include "material.h"

namespace ppp
{
    namespace material
    {
        void ambient_color(float r, float g, float b)
        {
            
        }

        void texture(image::image_id id)
        {

        }

        void shader(shader_program_id id)
        {

        }
        
        void reset()
        {

        }

        void normal_material()
        {

        }

        void specular_material()
        {

        }

        shader_program_id create_shader(const std::string& vertex_source, const std::string& fragment_source)
        {
            return -1;
        }

        shader_program_id load_shader(const std::string& vertex_path, const std::string& fragment_path)
        {
            return -1;
        }
    }
}