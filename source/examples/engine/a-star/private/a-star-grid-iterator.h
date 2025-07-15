#pragma once

namespace ppp
{
    class igrid;

    class grid_iterator
    {
    public:
        grid_iterator();
        ~grid_iterator();
        grid_iterator(const grid_iterator& other);
        grid_iterator(grid_iterator&& other);
        grid_iterator(igrid* grid);

        grid_iterator& operator=(const grid_iterator& other);
        grid_iterator& operator=(grid_iterator&& other);

        grid_iterator operator++();
        grid_iterator operator++(int);

        class grid_cell* operator*() const;

        bool operator==(const grid_iterator& other) const;
        bool operator!=(const grid_iterator& other) const;

        bool at_end() const;

    private:
        void next();

        igrid* m_grid;

        int m_width;
        int m_height;

        int m_index;
    };

    class const_grid_iterator
    {
    public:
        const_grid_iterator();
        ~const_grid_iterator();
        const_grid_iterator(const const_grid_iterator& other);
        const_grid_iterator(const_grid_iterator&& other);
        const_grid_iterator(const igrid* grid);

        const_grid_iterator& operator=(const const_grid_iterator& other);
        const_grid_iterator& operator=(const_grid_iterator&& other);

        const_grid_iterator operator++();
        const_grid_iterator operator++(int);

        const grid_cell* operator*() const;

        bool operator==(const const_grid_iterator& other) const;
        bool operator!=(const const_grid_iterator& other) const;

        bool at_end() const;

    private:
        void next();

        const igrid* m_grid;

        int m_width;
        int m_height;

        int m_index;
    };
}