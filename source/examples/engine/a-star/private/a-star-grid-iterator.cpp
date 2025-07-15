#include "a-star-grid-iterator.h"
#include "a-star-grid.h"

namespace ppp
{
    //--------------------------------------------------------------------------
    grid_iterator::grid_iterator()
        :m_grid(nullptr)
        , m_width(0)
        , m_height(0)
        , m_index(0)
    {}
    //--------------------------------------------------------------------------
    grid_iterator::~grid_iterator() = default;
    //--------------------------------------------------------------------------
    grid_iterator::grid_iterator(const grid_iterator& other) = default;
    //--------------------------------------------------------------------------
    grid_iterator::grid_iterator(grid_iterator&& other) = default;
    //--------------------------------------------------------------------------
    grid_iterator::grid_iterator(igrid* grid)
        :m_grid(grid)
        , m_width(grid->width())
        , m_height(grid->height())
        , m_index(0)
    {}

    //--------------------------------------------------------------------------
    grid_iterator& grid_iterator::operator=(const grid_iterator& other) = default;
    //--------------------------------------------------------------------------
    grid_iterator& grid_iterator::operator=(grid_iterator&& other) = default;

    //--------------------------------------------------------------------------
    grid_iterator grid_iterator::operator++()
    {
        next();
        return *this;
    }
    //--------------------------------------------------------------------------
    grid_iterator grid_iterator::operator++(int)
    {
        next();
        return *this;
    }

    //--------------------------------------------------------------------------
    grid_cell* grid_iterator::operator*() const
    {
        vec2i grid_location;

        grid_location.x = m_index % m_width;
        grid_location.y = m_index / m_width;

        return m_grid->get_grid_cell_at_index(grid_location);
    }

    //--------------------------------------------------------------------------
    bool grid_iterator::operator==(const grid_iterator& other) const
    {
        return m_width == other.m_width && m_height == other.m_height && m_index == other.m_index;
    }
    //--------------------------------------------------------------------------
    bool grid_iterator::operator!=(const grid_iterator& other) const
    {
        return !(*this == other);
    }

    //--------------------------------------------------------------------------
    bool grid_iterator::at_end() const
    {
        return m_index >= m_width * m_height;
    }

    //--------------------------------------------------------------------------
    void grid_iterator::next()
    {
        ++m_index;
    }

    //--------------------------------------------------------------------------
    const_grid_iterator::const_grid_iterator()
        : m_grid(nullptr)
        , m_width(0)
        , m_height(0)
        , m_index(0)
    {}
    //--------------------------------------------------------------------------
    const_grid_iterator::~const_grid_iterator() = default;
    //--------------------------------------------------------------------------
    const_grid_iterator::const_grid_iterator(const const_grid_iterator& other) = default;
    //--------------------------------------------------------------------------
    const_grid_iterator::const_grid_iterator(const_grid_iterator&& other) = default;
    //--------------------------------------------------------------------------
    const_grid_iterator::const_grid_iterator(const igrid* grid)
        : m_grid(grid)
        , m_width(grid->width())
        , m_height(grid->height())
        , m_index(0)
    {}

    //--------------------------------------------------------------------------
    const_grid_iterator& const_grid_iterator::operator=(const const_grid_iterator& other) = default;
    //--------------------------------------------------------------------------
    const_grid_iterator& const_grid_iterator::operator=(const_grid_iterator&& other) = default;

    //--------------------------------------------------------------------------
    const_grid_iterator const_grid_iterator::operator++()
    {
        next();
        return *this;
    }
    //--------------------------------------------------------------------------
    const_grid_iterator const_grid_iterator::operator++(int)
    {
        next();
        return *this;
    }

    //--------------------------------------------------------------------------
    const grid_cell* const_grid_iterator::operator*() const
    {
        vec2i grid_location;

        grid_location.x = m_index % m_width;
        grid_location.y = static_cast<int>(m_index / m_width);

        return m_grid->get_grid_cell_at_index(grid_location);
    }

    //--------------------------------------------------------------------------
    bool const_grid_iterator::operator==(const const_grid_iterator& other) const
    {
        return m_width == other.m_width && m_height == other.m_height && m_index == other.m_index;
    }
    //--------------------------------------------------------------------------
    bool const_grid_iterator::operator!=(const const_grid_iterator& other) const
    {
        return !(*this == other);
    }

    //--------------------------------------------------------------------------
    bool const_grid_iterator::at_end() const
    {
        return m_index >= m_width * m_height;
    }

    //--------------------------------------------------------------------------
    void const_grid_iterator::next()
    {
        ++m_index;
    }
}