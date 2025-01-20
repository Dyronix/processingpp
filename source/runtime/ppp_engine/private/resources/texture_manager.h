#pragma once

#include "util/types.h"

namespace ppp
{
    namespace texture_manager
    {
        void add_image(const std::string& shader_tag, render::texture_id image);

        const std::vector<render::texture_id>& images(const std::string& shader_tag);

        void reset_images(const std::string& shader_tag);
    }
}