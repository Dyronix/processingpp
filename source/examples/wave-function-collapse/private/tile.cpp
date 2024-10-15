#include "tile.h"

namespace ppp
{
    namespace wave_function_collapse
    {
        namespace internal
        {
            bool compare_edge(int a, int b) 
            {
                int reverse_a = 0;

                int num_bits = sizeof(a) * 8; // Assuming 32-bit integers

                // Reverse the bits in num1
                for (int i = 0; i < num_bits; ++i) 
                {
                    if (a & (1 << i)) 
                    {
                        reverse_a |= (1 << (num_bits - 1 - i));
                    }
                }

                return reverse_a == b;
            }
        }

        std::unique_ptr<Tile> Tile::rotate(const std::unique_ptr<Tile>& tile, int num_rotations)
        {
            // Rotate image
            image::load_pixels(tile->image_id());
            std::vector<image::pixels_s32> rotated_pixels = image::rotate_clockwise(image::pixels_as_u32(), tile->image_width(), tile->image_height(), num_rotations);
            image::image rotated_image = image::create(tile->image_width(), tile->image_height(), tile->image_channels(), (image::pixels_u8_ptr)rotated_pixels.data());

            // Rotate edges
            std::vector<int> rotated_edges;
            size_t len = tile->edges().size();
            for (int i = 0; i < len; ++i)
            {
                rotated_edges.push_back(tile->edges()[(i - 1 + len) % len]);
            }

            // Create new tile
            return std::make_unique<Tile>(rotated_image, rotated_edges);
        }

        Tile::Tile(const image::image& img, const std::vector<int>& edges)
            :m_image(img)
            ,m_edges(edges)
        {}

        void Tile::show_image(int x, int y, int w, int h) const
        {
            image::draw(m_image.id, x, y, w, h);
        }

        void Tile::analyse(const std::vector<std::unique_ptr<Tile>>& other_tiles)
        {
            for (int i = 0; i < other_tiles.size(); i++) 
            {
                const std::unique_ptr<Tile>& tile = other_tiles[i];
                if (tile.get() == this)
                {
                    // Tile 5 can't match itself
                    continue;
                }

                // UP
                if (internal::compare_edge(tile->edges()[2], m_edges[0])) {
                    m_up.push_back(i);
                }
                // RIGHT
                if (internal::compare_edge(tile->edges()[3], m_edges[1])) {
                    m_right.push_back(i);
                }
                // DOWN
                if (internal::compare_edge(tile->edges()[0], m_edges[2])) {
                    m_down.push_back(i);
                }
                // LEFT
                if (internal::compare_edge(tile->edges()[1], m_edges[3])) {
                    m_left.push_back(i);
                }
            }
        }
    }
}