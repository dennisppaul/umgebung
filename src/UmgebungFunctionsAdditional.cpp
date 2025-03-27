/*
 * Umgebung
 *
 * This file is part of the *Umgebung* library (https://github.com/dennisppaul/umgebung).
 * Copyright (c) 2025 Dennis P Paul.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <filesystem>

#if defined(__APPLE__) || defined(__linux__)
#include <dlfcn.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#include "tiny_obj_loader.h"

#include "Umgebung.h"

namespace umgebung {

    bool begins_with(const std::string& str, const std::string& prefix) {
        if (prefix.size() > str.size()) {
            return false;
        }
        return str.substr(0, prefix.size()) == prefix;
    }

    bool ends_with(const std::string& str, const std::string& suffix) {
        if (suffix.size() > str.size()) {
            return false;
        }
        return str.substr(str.size() - suffix.size()) == suffix;
    }

    void color_inv(const uint32_t color, float& r, float& g, float& b, float& a) {
        a = static_cast<float>((color >> 24) & 0xFF) / 255.0f;
        b = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
        g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
        r = static_cast<float>(color & 0xFF) / 255.0f;
    }

    bool exists(const std::string& file_path) {
        const std::filesystem::path path(file_path);
        return std::filesystem::exists(path);
    }

    std::string find_file_in_paths(const std::vector<std::string>& paths, const std::string& filename) {
        for (const auto& path: paths) {
            std::filesystem::path full_path = std::filesystem::path(path) / filename;
            if (std::filesystem::exists(full_path)) {
                return full_path.string();
            }
        }
        return "";
    }

    std::string find_in_environment_path(const std::string& filename) {
        std::string path;
        char*       env = getenv("PATH");
        if (env != nullptr) {
            std::string path_env(env);
            // On Windows, PATH entries are separated by ';', on Linux/macOS by ':'
#if defined(_WIN32)
            char path_separator = ';';
#else
            char path_separator = ':';
#endif

            std::istringstream ss(path_env);
            std::string        token;

            while (std::getline(ss, token, path_separator)) {
                std::filesystem::path candidate = std::filesystem::path(token) / filename;
                if (std::filesystem::exists(candidate)) {
                    path = candidate.string();
                    break;
                }
            }
        }
        return path;
    }

    std::string get_executable_location() {
#if defined(__APPLE__) || defined(__linux__)
        Dl_info info;
        // Get the address of a function within the library (can be any function)
        if (dladdr((void*) &get_executable_location, &info)) {
            std::filesystem::path lib_path(info.dli_fname);                                      // Full path to the library
            return lib_path.parent_path().string() + std::filesystem::path::preferred_separator; // Return the directory without the library name
        } else {
            std::cerr << "Could not retrieve library location (dladdr)" << std::endl;
            return "";
        }
#elif defined(_WIN32)
        HMODULE hModule = nullptr; // Handle to the DLL
        char    path[MAX_PATH];

        if (GetModuleFileNameA(hModule, path, MAX_PATH) != 0) {
            std::filesystem::path lib_path(path);                                                // Full path to the DLL
            return lib_path.parent_path().string() + std::filesystem::path::preferred_separator; // Add the separator
        } else {
            std::cerr << "Could not retrieve library location (GetModuleFileName)" << std::endl;
            return "";
        }
#endif
    }

    std::vector<std::string> get_files(const std::string& directory, const std::string& extension) {
        std::vector<std::string> files;
        for (const auto& entry: std::filesystem::directory_iterator(directory)) {
            if (entry.path().extension() == extension || extension == "*" || extension == "*.*" || extension.empty()) {
                files.push_back(entry.path().string());
            }
        }
        return files;
    }

    int get_int_from_argument(const std::string& argument) {
        /* get an int value from an argument formated like e.g this "value=23" */
        const std::size_t pos = argument.find('=');
        if (pos == std::string::npos) {
            throw std::invalid_argument("no '=' character found in argument.");
        }
        const std::string valueStr = argument.substr(pos + 1);
        return std::stoi(valueStr);
    }

    std::string get_string_from_argument(const std::string& argument) {
        /* get a string value from an argument formated like e.g this "value=twentythree" */
        std::size_t pos = argument.find('=');
        if (pos == std::string::npos) {
            throw std::invalid_argument("no '=' character found in argument.");
        }
        std::string valueStr = argument.substr(pos + 1);
        return valueStr;
    }

    std::string timestamp() {
        const auto         now   = std::chrono::system_clock::now();
        const auto         ms    = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        const auto         timer = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&timer), "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    void audio(const int input_channels,
               const int output_channels,
               const int sample_rate,
               const int buffer_size,
               const int input_device,
               const int output_device) {
        if (is_initialized()) {
            warning("`audio()` must be called before or within `settings()`.");
            return;
        }
        umgebung::enable_audio           = true;
        umgebung::input_channels         = input_channels;
        umgebung::output_channels        = output_channels;
        umgebung::sample_rate            = sample_rate;
        umgebung::audio_buffer_size      = buffer_size;
        umgebung::audio_input_device_id  = input_device;
        umgebung::audio_output_device_id = output_device;
    }

    void audio(const int          input_channels,
               const int          output_channels,
               const int          sample_rate,
               const int          buffer_size,
               const std::string& input_device_name,
               const std::string& output_device_name) {
        if (is_initialized()) {
            warning("`audio()` must be called before or within `settings()`.");
            return;
        }
        umgebung::enable_audio             = true;
        umgebung::input_channels           = input_channels;
        umgebung::output_channels          = output_channels;
        umgebung::sample_rate              = sample_rate;
        umgebung::audio_buffer_size        = buffer_size;
        umgebung::audio_input_device_id    = AUDIO_DEVICE_FIND_BY_NAME;
        umgebung::audio_output_device_id   = AUDIO_DEVICE_FIND_BY_NAME;
        umgebung::audio_input_device_name  = input_device_name;
        umgebung::audio_output_device_name = output_device_name;
    }
    void audio(const AudioUnitInfo& info) {
        if (is_initialized()) {
            warning("`audio()` must be called before or within `settings()`.");
            return;
        }
        umgebung::enable_audio             = true;
        umgebung::audio_input_device_id    = info.input_device_id;
        umgebung::audio_input_device_name  = info.input_device_name;
        umgebung::input_channels           = info.input_channels;
        umgebung::audio_output_device_id   = info.output_device_id;
        umgebung::audio_output_device_name = info.output_device_name;
        umgebung::output_channels          = info.output_channels;
        umgebung::audio_buffer_size        = info.buffer_size;
        umgebung::sample_rate              = info.sample_rate;
    }

    void audio_start(PAudio* device) {
        if (device == nullptr) {
            subsystem_audio->start(a);
        } else {
            subsystem_audio->start(device);
        }
    }

    void audio_stop(PAudio* device) {
        if (device == nullptr) {
            subsystem_audio->stop(a);
        } else {
            subsystem_audio->stop(device);
        }
    }


    std::vector<Vertex> loadOBJ(const std::string& filename) {
        tinyobj::ObjReader       reader;
        tinyobj::ObjReaderConfig config;
        config.triangulate = true;

        if (!reader.ParseFromFile(filename, config)) {
            std::cerr << "Failed to load OBJ: " << reader.Error() << std::endl;
            return {};
        }

        std::vector<Vertex> vertices;

        const auto& attrib    = reader.GetAttrib();
        const auto& shapes    = reader.GetShapes();
        const auto& materials = reader.GetMaterials();

        for (const auto& shape: shapes) {
            size_t face_index = 0; // Face counter (reset per shape)

            for (size_t i = 0; i < shape.mesh.indices.size(); i++) {
                const auto& index = shape.mesh.indices[i];
                Vertex      vertex;

                // Vertex position
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                    1.0f};

                // Texture coordinate (if available)
                if (index.texcoord_index >= 0) {
                    vertex.tex_coord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1] // Flip Y-axis
                    };
                } else {
                    vertex.tex_coord = {0.0f, 0.0f}; // No texture coordinates
                }

                // Normals
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                    0.0};

                // Get correct material index per face
                if (i % 3 == 0) { // Every 3 vertices = new face
                    face_index++;
                }
                int material_id = (face_index < shape.mesh.material_ids.size()) ? shape.mesh.material_ids[face_index - 1] : -1;

                if (material_id >= 0 && material_id < materials.size()) {
                    const auto& material = materials[material_id];

                    // Assign diffuse color
                    vertex.color = glm::vec4(
                        material.diffuse[0], // R
                        material.diffuse[1], // G
                        material.diffuse[2], // B
                        1.0f                 // A (full opacity)
                    );
                } else {
                    // default color white
                    vertex.color = glm::vec4(1.0f);
                }

                vertices.push_back(vertex);
            }
        }
        return vertices;
    }

    std::vector<Vertex> loadOBJ_NoMaterial(const std::string& filename) {
        tinyobj::ObjReader       reader;
        tinyobj::ObjReaderConfig config;
        config.triangulate = true; // Ensure we only get triangles

        if (!reader.ParseFromFile(filename, config)) {
            if (!reader.Error().empty()) {
                std::cerr << "OBJ Loader Error: " << reader.Error() << std::endl;
            }
            return {};
        }

        std::vector<Vertex> vertices;

        if (!reader.Warning().empty()) {
            std::cout << "OBJ Loader Warning: " << reader.Warning() << std::endl;
        }

        const tinyobj::attrib_t&             attrib = reader.GetAttrib();
        const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
        // const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

        // Loop over shapes
        for (const auto& shape: shapes) {
            for (const auto& index: shape.mesh.indices) {
                const tinyobj::real_t vx = attrib.vertices[3 * index.vertex_index + 0];
                const tinyobj::real_t vy = attrib.vertices[3 * index.vertex_index + 1];
                const tinyobj::real_t vz = attrib.vertices[3 * index.vertex_index + 2];
                // TODO add normals once Vertex is upgraded
                // const tinyobj::real_t nx = attrib.normals[3 * index.normal_index + 0];
                // const tinyobj::real_t ny = attrib.normals[3 * index.normal_index + 1];
                // const tinyobj::real_t nz = attrib.normals[3 * index.normal_index + 2];
                const tinyobj::real_t tx = attrib.texcoords[2 * index.texcoord_index + 0];
                const tinyobj::real_t ty = attrib.texcoords[2 * index.texcoord_index + 1];
                // Optional: vertex colors
                const tinyobj::real_t red   = attrib.colors[3 * index.vertex_index + 0];
                const tinyobj::real_t green = attrib.colors[3 * index.vertex_index + 1];
                const tinyobj::real_t blue  = attrib.colors[3 * index.vertex_index + 2];

                vertices.emplace_back(Vertex(glm::vec3(vx, vy, vz),
                                             glm::vec4(red, green, blue, 1.0f),
                                             glm::vec2(tx, ty)));
            }
        }
        return vertices;
    }
} // namespace umgebung
