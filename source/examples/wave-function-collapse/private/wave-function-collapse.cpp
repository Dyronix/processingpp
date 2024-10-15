#include "entrypoint.h"

#include "shapes.h"
#include "color.h"
#include "rendering.h"
#include "events.h"
#include "structure.h"
#include "transform.h"
#include "environment.h"
#include "image.h"

#include <algorithm>
#include <vector>
#include <unordered_map>

#include <time.h>

namespace ppp
{
    namespace internal
    {
        void check_valid(std::vector<int>& arr, const std::vector<int>& valid)
        {
            for (int i = arr.size() - 1; i >= 0; i--)
            {
                // VALID: [BLANK, RIGHT]
                // ARR: [BLANK, UP, RIGHT, DOWN, LEFT]
                // result in removing UP, DOWN, LEFT

                if (std::find(valid.begin(), valid.end(), arr[i]) == valid.end())
                {
                    arr.erase(std::remove_if(arr.begin(), arr.end(),
                        [i](int x)
                    {
                        return x == i;
                    }), arr.end());
                }
            }
        }
    }

    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

    constexpr int BLANK = 0;
    constexpr int UP = 1;
    constexpr int RIGHT = 2;
    constexpr int DOWN = 3;
    constexpr int LEFT = 4;

    constexpr int _dim = 2;

    struct Cell
    {
        bool collapsed;
        std::vector<int> options;
    };

    std::vector<image::image> _images;
    std::vector<Cell> _grid;

    std::vector<std::vector<std::vector<int>>> _rules =
    {
        {
            {BLANK, UP},
            {BLANK, RIGHT},
            {BLANK, DOWN},
            {BLANK, LEFT}
        },
        {
            {RIGHT, LEFT, DOWN},
            {LEFT, UP, DOWN},
            {BLANK, DOWN},
            {RIGHT, UP, DOWN}
        },
        {
            {RIGHT, LEFT, DOWN},
            {LEFT, UP, DOWN},
            {RIGHT, LEFT, UP},
            {BLANK, LEFT}
        },
        {
            {BLANK, UP},
            {LEFT, UP, DOWN},
            {RIGHT, LEFT, UP},
            {RIGHT, UP, DOWN}
        },
        {
            {RIGHT, LEFT, DOWN},
            {BLANK, RIGHT},
            {RIGHT, LEFT, UP},
            {UP, DOWN, RIGHT}
        },
    };

    void setup_canvas()
    {
        rendering::create_canvas((_window_width / 2) - (_canvas_width / 2), (_window_height / 2) - (_canvas_height / 2), _canvas_width, _canvas_height);

        color::background(151);
    }
    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::KeyCode::KEY_ESCAPE);

        mouse::add_mouse_pressed_callback(
            [](mouse::MouseCode code)
        {
            if (code == mouse::MouseCode::BUTTON_LEFT)
            {
                structure::redraw();
            }
        });
    }

    void load_images()
    {
        _images.push_back(image::load("tiles/demo/blank.png"));
        _images.push_back(image::load("tiles/demo/up.png"));
        _images.push_back(image::load("tiles/demo/right.png"));
        _images.push_back(image::load("tiles/demo/down.png"));
        _images.push_back(image::load("tiles/demo/left.png"));

        image::image_mode(image::image_mode_type::CORNER);
    }

    AppParams entry()
    {
        AppParams app_params;

        app_params.window_width = 1280;
        app_params.window_height = 720;

        return app_params;
    }

    void setup()
    {
        srand(time(NULL));

        setup_canvas();
        setup_input_events();

        load_images();

        for (int i = 0; i < _dim * _dim; ++i)
        {
            _grid.push_back(
            {
                false, 
                {BLANK, UP, RIGHT, DOWN, LEFT}
            });
        }
    }

    void draw()
    {
        color::background(0);

        const int w = _canvas_width / _dim;
        const int h = _canvas_height / _dim;

        for (int j = 0; j < _dim; ++j)
        {
            for (int i = 0; i < _dim; ++i)
            {
                auto cell = _grid[i + j * _dim];
                if (cell.collapsed)
                {
                    int index = cell.options[0];
                    image::draw(_images[index].id, i * w, j * h, w, h);
                }
                else
                {
                    color::fill(0);
                    color::stroke(255);
                    shapes::rect(i * w, j * h, w, h);
                }
            }
        }

        std::vector<Cell*> copy_grid;
        for (Cell& c : _grid)
        {
            if (c.collapsed)
            {
                continue;
            }

            copy_grid.push_back(&c);
        }

        if (copy_grid.size() == 0)
        {
            return;
        }

        std::vector<Cell*> min_elements;

        Cell* min_value = *std::min_element(copy_grid.begin(), copy_grid.end(),
            [](const Cell* left, const Cell* right) 
        {
            return left->options.size() < right->options.size();
        });

        for (Cell* cell : copy_grid)
        {
            if (cell->options.size() == min_value->options.size()) 
            {
                min_elements.push_back(cell);
            }
        }

        int cell_index = rand() % min_elements.size();
        Cell* cell = min_elements[cell_index];
        cell->collapsed = true;
        int option_index = rand() % cell->options.size();
        int pick = cell->options[option_index];
        cell->options = { pick };

        // Next generation of tiles
        std::vector<Cell> next_grid(_dim * _dim, Cell());

        for (int j = 0; j < _dim; ++j)
        {
            for (int i = 0; i < _dim; ++i)
            {
                auto index = i + j * _dim;
                if (_grid[index].collapsed)
                {
                    next_grid[index] = _grid[index];
                }
                else
                {
                    std::vector<int> options = { BLANK, UP, RIGHT, DOWN, LEFT };
                
                    // Look up
                    if (j > 0)
                    {
                        const Cell& up = _grid[i + (j - 1) * _dim];
                        std::vector<int> valid_options = {};
                        for (int option : up.options)
                        {
                            const std::vector<int>& valid = _rules[option][2];
                            valid_options.insert(valid_options.end(), valid.begin(), valid.end());
                        }
                        internal::check_valid(options, valid_options);
                    }
                    // Look right
                    if (i < _dim - 1)
                    {
                        const Cell& right = _grid[i + 1 + j * _dim];
                        std::vector<int> valid_options = {};
                        for (int option : right.options)
                        {
                            const std::vector<int>& valid = _rules[option][3];
                            valid_options.insert(valid_options.end(), valid.begin(), valid.end());
                        }
                        internal::check_valid(options, valid_options);
                    }
                    // Look down
                    if (j < _dim - 1)
                    {
                        const Cell& down = _grid[i + (j + 1) * _dim];
                        std::vector<int> valid_options = {};
                        for (int option : down.options)
                        {
                            const std::vector<int>& valid = _rules[option][0];
                            valid_options.insert(valid_options.end(), valid.begin(), valid.end());
                        }
                        internal::check_valid(options, valid_options);
                    }
                    // Look left
                    if (i > 0)
                    {
                        const Cell& left = _grid[i - 1 + j * _dim];
                        std::vector<int> valid_options = {};
                        for (int option : left.options)
                        {
                            const std::vector<int>& valid = _rules[option][1];
                            valid_options.insert(valid_options.end(), valid.begin(), valid.end());
                        }
                        internal::check_valid(options, valid_options);
                    }

                    next_grid[index].options = options;
                    next_grid[index].collapsed = false;
                }
            }
        }

        _grid = next_grid;

        structure::no_loop();
    }
}