#pragma once

#include "a-star-grid-cell.h"

#include "vector.h"

#include <vector>
#include <functional>
#include <optional>

namespace ppp
{
    class igrid
    {
    public:
        igrid() = default;
        virtual ~igrid() = default;

        igrid(const igrid& other) = default;
        igrid(igrid&& other) = default;

        igrid& operator=(const igrid& other) = default;
        igrid& operator=(igrid&& other) = default;

    public:
        //-------------------------------------------------------------------------
        virtual void trigger_grid_cell_changed_at_index(int x, int y) const = 0;
        //-------------------------------------------------------------------------
        virtual void trigger_grid_cell_changed_at_index(const vec2i& index) const = 0;
        //-------------------------------------------------------------------------
        virtual void trigger_grid_cell_changed_at_world_location(const vec2& location) const = 0;

        //-------------------------------------------------------------------------
        virtual grid_cell* get_grid_cell_at_index(int x, int y) const = 0;
        //-------------------------------------------------------------------------
        virtual grid_cell* get_grid_cell_at_index(const vec2i& index) const = 0;
        //-------------------------------------------------------------------------
        virtual grid_cell* get_grid_cell_at_world_location(const vec2& location) const = 0;

        //-------------------------------------------------------------------------
        virtual std::optional<vec2> get_cell_world_location(int x, int y) const = 0;
        //-------------------------------------------------------------------------
        virtual std::optional<vec2> get_cell_world_location_safe(int x, int y) const = 0;
        //-------------------------------------------------------------------------
        virtual std::optional<vec2> get_cell_center_world_location(int x, int y) const = 0;
        //-------------------------------------------------------------------------
        virtual std::optional<vec2> get_cell_center_world_location_safe(int x, int y) const = 0;

        //-------------------------------------------------------------------------
        virtual std::optional<vec2i> get_cell_index_from_world_location(const vec2& world_location) const = 0;

        //-------------------------------------------------------------------------
        virtual bool is_valid_index(int x, int y) const = 0;
        //-------------------------------------------------------------------------
        virtual bool is_valid_index(const vec2i& index) const = 0;

    public:
        //-------------------------------------------------------------------------
        virtual void subscribe_on_changed(const std::function<void(const vec2i&)>& func) = 0;

        //-------------------------------------------------------------------------
        virtual int width() const = 0;
        //-------------------------------------------------------------------------
        virtual int height() const = 0;

        //-------------------------------------------------------------------------
        virtual float cell_size() const = 0;

        //-------------------------------------------------------------------------
        virtual const vec2& origin() const = 0;
    };

    template<typename TGridCell>
    class grid : public igrid
    {
    public:
        grid()
            :igrid()
        {

        }
        ~grid() override = default;

        grid(const grid& other) = default;
        grid(grid&& other) noexcept = default;

        grid& operator=(const grid& other) = default;
        grid& operator=(grid&& other) noexcept = default;

    public:
        //-------------------------------------------------------------------------
        void initialize(int width, int height, float cell_size, const vec2& origin)
        {
            if (m_cells.empty() == false)
            {
                m_cells.clear();
            }

            m_width = width;
            m_height = height;
            m_cell_size = cell_size;
            m_origin = origin;

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    auto world_location = get_cell_world_location(x, y);
                    if (!world_location)
                    {
                        continue;
                    }

                    auto cell = create_new_grid_cell<TGridCell>(this, x, y);

                    m_cells.push_back(std::move(cell));
                }
            }

            assign_neighbours(m_cells, m_width, m_height);
        }

        //-------------------------------------------------------------------------
        void trigger_grid_cell_changed_at_index(int x, int y) const override
        {
            trigger_grid_cell_changed_at_index(vec2i(x, y));
        }
        //-------------------------------------------------------------------------
        void trigger_grid_cell_changed_at_index(const vec2i& index) const override
        {
            if (is_valid_index(index.x, index.y) == false)
            {
                return;
            }

            if (m_on_grid_cell_changed.empty() == false)
            {
                for (auto& f : m_on_grid_cell_changed)
                {
                    f(index);
                }
            }
        }
        //-------------------------------------------------------------------------
        void trigger_grid_cell_changed_at_world_location(const vec2& location) const override
        {
            auto index = get_cell_index_from_world_location(location);
            if (!index)
            {
                return;
            }

            return trigger_grid_cell_changed_at_index(*index);
        }

        //-------------------------------------------------------------------------
        bool set_grid_cell_at_index(int x, int y, std::unique_ptr<TGridCell> cell)
        {
            return set_grid_cell_at_index(vec2i(x, y), std::move(cell));
        }
        //-------------------------------------------------------------------------
        bool set_grid_cell_at_index(const vec2i& index, std::unique_ptr<TGridCell> cell)
        {
            if (is_valid_index(index.x, index.y) == false)
            {
                return false;
            }

            int linear_index = index.x + (m_width * index.y);

            m_cells[linear_index] = std::move(cell);

            trigger_grid_cell_changed_at_index(index);

            return true;
        }
        //-------------------------------------------------------------------------
        bool set_grid_cell_at_world_location(const vec2& world_location, std::unique_ptr<TGridCell> cell)
        {
            auto index = get_cell_index_from_world_location(world_location);
            if (!index)
            {
                return false;
            }

            return set_grid_cell_at_index(*index, std::move(cell));
        }

        //-------------------------------------------------------------------------
        grid_cell* get_grid_cell_at_index(int x, int y) const override
        {
            return get_grid_cell_at_index(vec2i(x, y));
        }
        //-------------------------------------------------------------------------
        grid_cell* get_grid_cell_at_index(const vec2i& index) const override
        {
            if (is_valid_index(index.x, index.y) == false)
            {
                return nullptr;
            }

            int linear_index = index.x + (m_width * index.y);

            return m_cells[linear_index].get();
        }
        //-------------------------------------------------------------------------
        grid_cell* get_grid_cell_at_world_location(const vec2& location) const override
        {
            auto index = get_cell_index_from_world_location(location);
            if (!index)
            {
                return nullptr;
            }

            return get_grid_cell_at_index(*index);
        }

        //-------------------------------------------------------------------------
        std::optional<vec2> get_cell_world_location(int x, int y) const override
        {
            vec2 world_location = vec2(x, y);

            return (world_location * m_cell_size) + m_origin;
        }
        //-------------------------------------------------------------------------
        std::optional<vec2> get_cell_world_location_safe(int x, int y) const override
        {
            if (is_valid_index(x, y) == false)
            {
                return {};
            }

            vec2 world_location = vec2(x, y);

            return (world_location * m_cell_size) + m_origin;
        }
        //-------------------------------------------------------------------------
        std::optional<vec2> get_cell_center_world_location(int x, int y) const override
        {
            vec2 world_location = *get_cell_world_location(x, y);
            vec2 half_cell_size = vec2(m_cell_size, m_cell_size) * 0.5f;

            return world_location + half_cell_size;
        }
        //-------------------------------------------------------------------------
        std::optional<vec2> get_cell_center_world_location_safe(int x, int y) const override
        {
            if (is_valid_index(x, y) == false)
            {
                return {};
            }

            vec2 world_location = *get_cell_world_location(x, y);
            vec2 half_cell_size = vec2(m_cell_size, m_cell_size) * 0.5f;

            return world_location + half_cell_size;
        }

        //-------------------------------------------------------------------------
        std::optional<vec2i> get_cell_index_from_world_location(const vec2& world_location) const override
        {
            vec2i index;

            index.x = std::floor((world_location - m_origin).x / m_cell_size);
            index.y = std::floor((world_location - m_origin).y / m_cell_size);

            if (is_valid_index(index.x, index.y) == false)
            {
                return {};
            }

            return index;
        }

        //-------------------------------------------------------------------------
        bool is_valid_index(int x, int y) const override
        {
            return x >= 0 && y >= 0 && x < m_width && y < m_height;
        }
        //-------------------------------------------------------------------------
        bool is_valid_index(const vec2i& index) const override
        {
            return is_valid_index(index.x, index.y);
        }

    public:
        //-------------------------------------------------------------------------
        void subscribe_on_changed(const std::function<void(const vec2i&)>& func) override
        {
            m_on_grid_cell_changed.push_back(func);
        }

        //-------------------------------------------------------------------------
        int width() const override
        {
            return m_width;
        }
        //-------------------------------------------------------------------------
        int height() const override
        {
            return m_height;
        }

        //-------------------------------------------------------------------------
        float cell_size() const override
        {
            return m_cell_size;
        }

        //-------------------------------------------------------------------------
        const vec2& origin() const override
        {
            return m_origin;
        }

        //-------------------------------------------------------------------------
        std::vector<const TGridCell*> cells() const
        {
            std::vector<const TGridCell*> all_cells;
            all_cells.reserve(m_cells.size());

            for (auto& cell : m_cells)
            {
                all_cells.push_back(cell.get());
            }

            return all_cells;
        }

    private:
        using unique_grid_cells = std::vector<std::unique_ptr<TGridCell>>;

        //--------------------------------------------------------------------------
        std::vector<grid_cell*> get_neighbours(unique_grid_cells& cells, int x, int y, int width, int height)
        {
            auto get_cell = [&cells, width](int x, int y)
            {
                int index = x + (y * width);

                return cells[index].get();
            };

            std::vector<grid_cell*> neighbours;

            if (x - 1 >= 0)
            {
                // left
                neighbours.push_back(get_cell(x - 1, y));
                // left down
                if (y - 1 >= 0) neighbours.push_back(get_cell(x - 1, y - 1));
                // left up
                if (y + 1 < height) neighbours.push_back(get_cell(x - 1, y + 1));
            }

            if (x + 1 < width)
            {
                // right
                neighbours.push_back(get_cell(x + 1, y));
                // right down
                if (y - 1 >= 0) neighbours.push_back(get_cell(x + 1, y - 1));
                // right up
                if (y + 1 < height) neighbours.push_back(get_cell(x + 1, y + 1));
            }

            // down
            if (y - 1 >= 0) neighbours.push_back(get_cell(x, y - 1));
            // up
            if (y + 1 < height) neighbours.push_back(get_cell(x, y + 1));

            return neighbours;
        }
        //--------------------------------------------------------------------------
        void assign_neighbours(unique_grid_cells& cells, int width, int height)
        {
            for (int x = 0; x < width; ++x)
            {
                for (int y = 0; y < height; ++y)
                {
                    int index = x + (y * width);

                    cells[index]->assign_neighbours(get_neighbours(cells, x, y, width, height));
                }
            }
        }

    private:
        int m_width { 0 };
        int m_height { 0 };
        float m_cell_size { 0 };
        vec2 m_origin {};
        unique_grid_cells m_cells;

    private:
        std::vector<std::function<void(const vec2i&)>> m_on_grid_cell_changed;
    };
}
