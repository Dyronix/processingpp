#include "model/parse_stl.h"

namespace ppp
{
    namespace model
    {
        void parse_stl(geometry::geometry* geom, const std::vector<u8>& buffer)
        {

        }

        //void parseBinarySTL(Geometry& model, const std::vector<uint8_t>& buffer) {
        //    if (buffer.size() < 84) {
        //        throw std::runtime_error("Invalid binary STL file: too small");
        //    }

        //    uint32_t faces;
        //    std::memcpy(&faces, &buffer[80], sizeof(faces));

        //    const size_t dataOffset = 84;
        //    const size_t faceLength = 50; // 12*4 bytes for vertices + 2 bytes for attributes

        //    for (uint32_t face = 0; face < faces; ++face) {
        //        size_t offset = dataOffset + face * faceLength;

        //        float normalX, normalY, normalZ;
        //        std::memcpy(&normalX, &buffer[offset], sizeof(normalX));
        //        std::memcpy(&normalY, &buffer[offset + 4], sizeof(normalY));
        //        std::memcpy(&normalZ, &buffer[offset + 8], sizeof(normalZ));

        //        Vector newNormal(normalX, normalY, normalZ);

        //        for (int i = 0; i < 3; ++i) {
        //            float vertexX, vertexY, vertexZ;
        //            size_t vertexOffset = offset + 12 + i * 12;
        //            std::memcpy(&vertexX, &buffer[vertexOffset], sizeof(vertexX));
        //            std::memcpy(&vertexY, &buffer[vertexOffset + 4], sizeof(vertexY));
        //            std::memcpy(&vertexZ, &buffer[vertexOffset + 8], sizeof(vertexZ));

        //            Vector newVertex(vertexX, vertexY, vertexZ);
        //            model.vertices.push_back(newVertex);
        //            model.vertexNormals.push_back(newNormal);
        //        }

        //        model.faces.push_back({ 3 * face, 3 * face + 1, 3 * face + 2 });
        //        model.uvs.push_back({ 0.0f, 0.0f });
        //        model.uvs.push_back({ 0.0f, 0.0f });
        //        model.uvs.push_back({ 0.0f, 0.0f });
        //    }
        //}


        //void parseASCIISTL(Geometry& model, const std::vector<std::string>& lines) {
        //    std::string state = "";
        //    std::array<int, 3> curVertexIndex = { -1, -1, -1 };
        //    Vector newNormal(0, 0, 0);
        //    Vector newVertex(0, 0, 0);

        //    for (const auto& line : lines) {
        //        std::string trimmedLine = line;
        //        trimmedLine.erase(trimmedLine.find_last_not_of(" \t\n\r") + 1);

        //        std::istringstream iss(trimmedLine);
        //        std::vector<std::string> parts;
        //        std::string part;
        //        while (iss >> part) {
        //            parts.push_back(part);
        //        }

        //        if (parts.empty()) {
        //            continue;
        //        }

        //        if (state == "") { // First run
        //            if (parts[0] != "solid") {
        //                throw std::runtime_error("Invalid state \"" + parts[0] + "\", should be \"solid\"");
        //            }
        //            state = "solid";
        //        }
        //        else if (state == "solid") { // First face
        //            if (parts[0] != "facet" || parts[1] != "normal") {
        //                throw std::runtime_error("Invalid state \"" + parts[0] + "\", should be \"facet normal\"");
        //            }
        //            newNormal = Vector(std::stof(parts[2]), std::stof(parts[3]), std::stof(parts[4]));
        //            model.vertexNormals.push_back(newNormal);
        //            model.vertexNormals.push_back(newNormal);
        //            model.vertexNormals.push_back(newNormal);
        //            state = "facet normal";
        //        }
        //        else if (state == "facet normal") { // After normal is defined
        //            if (parts[0] != "outer" || parts[1] != "loop") {
        //                throw std::runtime_error("Invalid state \"" + parts[0] + "\", should be \"outer loop\"");
        //            }
        //            state = "vertex";
        //        }
        //        else if (state == "vertex") {
        //            if (parts[0] == "vertex") {
        //                newVertex = Vector(std::stof(parts[1]), std::stof(parts[2]), std::stof(parts[3]));
        //                model.vertices.push_back(newVertex);
        //                model.uvs.push_back({ 0.0f, 0.0f });
        //                curVertexIndex = { static_cast<int>(model.vertices.size() - 1), curVertexIndex[1], curVertexIndex[2] };
        //            }
        //            else if (parts[0] == "endloop") {
        //                model.faces.push_back({ curVertexIndex[0], curVertexIndex[1], curVertexIndex[2] });
        //                state = "endloop";
        //            }
        //            else {
        //                throw std::runtime_error("Invalid state \"" + parts[0] + "\", should be \"vertex\" or \"endloop\"");
        //            }
        //        }
        //        else if (state == "endloop") {
        //            if (parts[0] != "endfacet") {
        //                throw std::runtime_error("Invalid state \"" + parts[0] + "\", should be \"endfacet\"");
        //            }
        //            state = "endfacet";
        //        }
        //        else if (state == "endfacet") {
        //            if (parts[0] == "endsolid") {
        //                // End of solid
        //            }
        //            else if (parts[0] == "facet" && parts[1] == "normal") {
        //                newNormal = Vector(std::stof(parts[2]), std::stof(parts[3]), std::stof(parts[4]));
        //                model.vertexNormals.push_back(newNormal);
        //                model.vertexNormals.push_back(newNormal);
        //                model.vertexNormals.push_back(newNormal);
        //                state = "facet normal";
        //            }
        //            else {
        //                throw std::runtime_error("Invalid state \"" + parts[0] + "\", should be \"endsolid\" or \"facet normal\"");
        //            }
        //        }
        //        else {
        //            throw std::runtime_error("Invalid state \"" + state + "\"");
        //        }
        //    }
        //}

        //// Helper function to match data at a specific offset
        //bool matchDataAt(const std::vector<uint8_t>& query, const std::vector<uint8_t>& buffer, size_t offset) {
        //    for (size_t i = 0; i < query.size(); ++i) {
        //        if (offset + i >= buffer.size() || query[i] != buffer[offset + i]) {
        //            return false;
        //        }
        //    }
        //    return true;
        //}

        //// Function to determine if the STL file is binary
        //bool isBinary(const std::vector<uint8_t>& buffer) {
        //    const std::vector<uint8_t> solid = { 115, 111, 108, 105, 100 }; // ASCII for "solid"

        //    for (size_t offset = 0; offset < 5; ++offset) {
        //        if (matchDataAt(solid, buffer, offset)) {
        //            return false; // ASCII STL detected
        //        }
        //    }

        //    return true; // Binary STL detected
        //}

        //// Function to parse STL file
        //std::vector<std::string> parseSTL(std::vector<std::string>& model, const std::vector<uint8_t>& buffer) 
        //{
        //    if (isBinary(buffer)) {
        //        parseBinarySTL(model, buffer);
        //    }
        //    else {
        //        std::string decoded(buffer.begin(), buffer.end()); // UTF-8 decoding equivalent
        //        std::istringstream stream(decoded);
        //        std::vector<std::string> lines;
        //        std::string line;

        //        while (std::getline(stream, line)) {
        //            lines.push_back(line);
        //        }

        //        parseASCIISTL(model, lines);
        //    }

        //    return model;
        //}
    }
}