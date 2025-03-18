#pragma once

#include "file_reader.h"
#include "nonogram_puzzle.h"

#include <iostream>
#include <vector>

namespace ppp
{
    // NONOGRAM PARSER
// Responsible for reading and parsing the binary file to build a puzzle.
    class nonogram_parser
    {
    public:
        nonogram_puzzle parse(const std::vector<std::byte>& treasure_data)
        {
            file_reader reader(treasure_data);
            nonogram_puzzle puzzle;
            puzzle.grid_size = 0;
            std::vector<std::vector<int>> x_records_clues;
            std::vector<std::vector<int>> y_records_clues;

            // Parse header, ID, and magic string.
            uint32_t file_header = reader.read_uint32();
            std::cout << "File header (uint32_t): " << file_header << "\n\n";

            uint8_t id_length = static_cast<uint8_t>(reader.read_byte());
            std::string id;
            for (int i = 0; i < id_length; i++)
            {
                id.push_back(reader.read_byte());
            }
            reader.read_byte(); // Skip null terminator.
            std::cout << "ID: " << id << "\n\n";

            uint32_t magic_len = reader.read_uint32();
            std::string magic;
            for (uint32_t i = 0; i < magic_len; i++)
            {
                magic.push_back(reader.read_byte());
            }
            if (!magic.empty() && magic.back() == '\0')
            {
                magic.pop_back();
            }
            std::cout << "Magic string: " << magic << "\n\n";

            // Parse X Section (row clues).
            uint8_t x_marker = static_cast<uint8_t>(reader.read_byte());
            std::string x_section_name = reader.read_c_string();
            std::cout << "Section: " << x_section_name << "\n";
            uint32_t x_block_size = reader.read_uint32();
            std::cout << "X section block size: " << x_block_size << "\n\n";
            std::cout << "X Section Records:\n";

            while (!reader.eof() && static_cast<uint8_t>(reader.peek_byte()) != 0x03)
            {
                uint8_t record_marker = static_cast<uint8_t>(reader.read_byte());
                if (record_marker != 0x04)
                {
                    std::cerr << "Unexpected record marker in x section: " << static_cast<int>(record_marker) << "\n";
                    break;
                }
                std::string record_label = reader.read_c_string();
                uint32_t record_size = reader.read_uint32();
                if (puzzle.grid_size == 0)
                {
                    puzzle.grid_size = record_size;
                }
                std::cout << "Record Label: " << record_label << ", Record Size: " << record_size << "\n";

                std::vector<int> clues;
                for (int i = 0; i < 3; i++)
                {
                    uint8_t field_marker = static_cast<uint8_t>(reader.read_byte());
                    if (field_marker != 0x10)
                    {
                        std::cerr << "Unexpected field marker: " << static_cast<int>(field_marker) << "\n";
                        break;
                    }
                    std::string field_label = reader.read_c_string();
                    uint32_t field_value = reader.read_uint32();
                    std::cout << "  Field " << field_label << " = " << field_value << "\n";
                    clues.push_back(field_value);
                }
                uint8_t terminator = static_cast<uint8_t>(reader.read_byte());
                if (terminator != 0x00)
                {
                    std::cerr << "Expected record terminator 0x00, got: " << static_cast<int>(terminator) << "\n";
                }
                std::cout << "\n";
                x_records_clues.push_back(clues);
            }
            if (!reader.eof() && static_cast<uint8_t>(reader.peek_byte()) == 0x03)
            {
                reader.read_byte(); // Consume marker.
            }

            // Parse Y Section (column clues).
            uint8_t y_marker = static_cast<uint8_t>(reader.read_byte());
            std::string y_section_name = reader.read_c_string();
            std::cout << "Section: " << y_section_name << "\n";
            uint32_t y_block_size = reader.read_uint32();
            std::cout << "Y section block size: " << y_block_size << "\n\n";
            std::cout << "Y Section Records:\n";

            while (!reader.eof())
            {
                uint8_t record_marker = static_cast<uint8_t>(reader.read_byte());
                if (record_marker != 0x04)
                {
                    std::cerr << "Unexpected record marker in y section: " << static_cast<int>(record_marker) << "\n";
                    break;
                }
                std::string record_label = reader.read_c_string();
                uint32_t record_size = reader.read_uint32();
                std::cout << "Record Label: " << record_label << ", Record Size: " << record_size << "\n";

                std::vector<int> clues;
                for (int i = 0; i < 3; i++)
                {
                    uint8_t field_marker = static_cast<uint8_t>(reader.read_byte());
                    if (field_marker != 0x10)
                    {
                        std::cerr << "Unexpected field marker in y section: " << static_cast<int>(field_marker) << "\n";
                        break;
                    }
                    std::string field_label = reader.read_c_string();
                    uint32_t field_value = reader.read_uint32();
                    std::cout << "  Field " << field_label << " = " << field_value << "\n";
                    clues.push_back(field_value);
                }
                uint8_t terminator = static_cast<uint8_t>(reader.read_byte());
                if (terminator != 0x00)
                {
                    std::cerr << "Expected record terminator 0x00 in y section, got: " << static_cast<int>(terminator) << "\n";
                }
                std::cout << "\n";
                y_records_clues.push_back(clues);
            }

            if (puzzle.grid_size == 0)
            {
                std::cerr << "Grid size not determined from file. Exiting.\n";
            }
            std::cout << "Grid size determined as: " << puzzle.grid_size << "\n";

            // Center the clues in a grid of size grid_size.
            int clue_count = x_records_clues.size();
            int start_pos = (puzzle.grid_size - clue_count) / 2;
            puzzle.row_clues.resize(puzzle.grid_size);
            puzzle.col_clues.resize(puzzle.grid_size);

            for (int i = 0; i < puzzle.grid_size; i++)
            {
                if (i >= start_pos && i < start_pos + clue_count)
                {
                    puzzle.row_clues[i] = { x_records_clues[i - start_pos] };
                }
                else
                {
                    puzzle.row_clues[i] = { {} };
                }
                if (i >= start_pos && i < start_pos + clue_count)
                {
                    puzzle.col_clues[i] = { y_records_clues[i - start_pos] };
                }
                else
                {
                    puzzle.col_clues[i] = { {} };
                }
            }

            return puzzle;
        }
    };
}