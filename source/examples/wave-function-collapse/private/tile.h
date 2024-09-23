#pragma once

#include "image.h"

#include <vector>
#include <memory>

namespace ppp
{
    namespace wave_function_collapse
    {
        class Tile
        {
        public:
            static std::unique_ptr<Tile> rotate(const std::unique_ptr<Tile>& t, int num_rotations);

            Tile(const image::Image& img, const std::vector<int>& edges);

        public:
            void show_image(int x, int y, int w, int h) const;
            void analyse(const std::vector<std::unique_ptr<Tile>>& other_tiles);

        public:
            using Edges = std::vector<int>;

            image::image_id image_id() const { return m_image.id; }
            int image_width() const { return m_image.width; }
            int image_height() const { return m_image.height; }
            int image_channels() const { return m_image.channels; }

            const Edges& edges() const { return m_edges; }

            const std::vector<int>& up() const { return m_up; }
            const std::vector<int>& right() const { return m_right; }
            const std::vector<int>& down() const { return m_down; }
            const std::vector<int>& left() const { return m_left; }

        private:
            image::Image m_image;
            std::vector<int> m_edges;
            std::vector<int> m_up;
            std::vector<int> m_right;
            std::vector<int> m_down;
            std::vector<int> m_left;
        };
    }
}