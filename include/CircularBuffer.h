/*
 * Umfeld
 *
 * This file is part of the *Umfeld* library (https://github.com/dennisppaul/umfeld).
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

#pragma once

#include <vector>
    #include <iostream>
    #include <algorithm>

    class CircularBuffer {
public:
    CircularBuffer(size_t size) : buffer(size), max_size(size), head(0), count(0) {}

    void push(const std::vector<float>& chunk) {
        const size_t chunk_size = chunk.size();
        if (chunk_size >= max_size) {
            // If chunk is larger than buffer, only keep the last max_size elements
            std::copy(chunk.end() - max_size, chunk.end(), buffer.begin());
            head  = 0;
            count = max_size;
            return;
        }

        // if chunk overflows the buffer, adjust `count`
        if (count + chunk_size > max_size) {
            count = max_size;
        } else {
            count += chunk_size;
        }

        // Insert chunk into buffer
        for (size_t i = 0; i < chunk_size; ++i) {
            buffer[head] = chunk[i];
            head         = (head + 1) % max_size;
        }
    }

    // Get the most recent `chunk_size` elements as a contiguous block into `latestChunk`
    void getLatestChunk(size_t chunk_size, std::vector<float>& latestChunk) const {
        if (chunk_size > count) {
            chunk_size = count; // Clamp to available data
        }

        latestChunk.resize(chunk_size); // Ensure the vector is the correct size
        const size_t start = (head + max_size - chunk_size) % max_size;

        // Check if the requested chunk wraps around
        if (start + chunk_size <= max_size) {
            // Contiguous case: Direct copy
            std::copy_n(buffer.begin() + start, chunk_size, latestChunk.begin());
            // std::copy(buffer.begin() + start, buffer.begin() + start + chunk_size, latestChunk.begin());
        } else {
            // Wrap-around case: Copy in two parts
            const size_t first_part = max_size - start;
            std::copy(buffer.begin() + start, buffer.end(), latestChunk.begin());
            std::copy_n(buffer.begin(), (chunk_size - first_part), latestChunk.begin() + first_part);
            // std::copy(buffer.begin(), buffer.begin() + (chunk_size - first_part), latestChunk.begin() + first_part);
        }
    }

    void printBuffer() const {
        for (size_t i = 0; i < count; ++i) {
            const size_t index = (head + max_size - count + i) % max_size;
            std::cout << buffer[index] << " ";
        }
        std::cout << "\n";
    }

private:
    std::vector<float> buffer;
    size_t             max_size;
    size_t             head;
    size_t             count;
};

inline int test() {
        CircularBuffer cb(5);
        std::vector<float> latestChunk;

        cb.push({1.0, 2.0, 3.0});
        cb.printBuffer();  // Expected: 1.0 2.0 3.0

        cb.push({4.0, 5.0, 6.0});
        cb.printBuffer();  // Expected: 2.0 3.0 4.0 5.0 6.0

        cb.getLatestChunk(3, latestChunk);
        for (const float f : latestChunk) {
            std::cout << f << " ";  // Expected: 4.0 5.0 6.0
        }
        std::cout << "\n";

        cb.push({7.0, 8.0});
        cb.getLatestChunk(4, latestChunk);
        for (const float f : latestChunk) {
            std::cout << f << " ";  // Expected: 5.0 6.0 7.0 8.0
        }
        std::cout << "\n";

        return 0;
    }