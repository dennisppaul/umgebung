#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Renderer
{
public:
    Renderer(GLuint shaderProgram, int width, int height)
        : shaderProgram(shaderProgram), currentMatrix(glm::mat4(1.0f))
    {
        initBuffers();
        aspectRatio = (float)width / (float)height;
        //         projection = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f);
        projection2D = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

        float fov = glm::radians(90.0f);
        float cameraDistance = (height / 2.0f) / tan(fov / 2.0f);

        // Perspective projection
        projection3D = glm::perspective(fov, (float)width / (float)height, 0.1f, 10000.0f);

        // View matrix
        //         viewMatrix = glm::lookAt(
        //             glm::vec3(0.0f, 0.0f, cameraDistance),  // Camera at calculated distance
        //             glm::vec3(0.0f, 0.0f, 0.0f),  // Looking at the origin
        //             glm::vec3(0.0f, 1.0f, 0.0f)   // Up direction
        //         );

        //         viewMatrix = glm::lookAt(
        //             glm::vec3(width / 2.0f, height / 2.0f, cameraDistance),  // Position camera at center
        //             glm::vec3(width / 2.0f, height / 2.0f, 0.0f),  // Look at the center of the scene
        //             glm::vec3(0.0f, -1.0f, 0.0f)  // Invert Y to match screen space
        //         );

        //         // Camera position and target
        //         glm::vec3 eye(width / 2.0f, height / 2.0f, cameraDistance);
        //         glm::vec3 center(width / 2.0f, height / 2.0f, 0.0f);
        //         glm::vec3 up(0.0f, -1.0f, 0.0f);  // Inverted Y
        //
        //         // Correct Right Vector (Fixes X-Flipping)
        //         glm::vec3 forward = glm::normalize(center - eye);
        //         glm::vec3 right = glm::normalize(glm::cross(up, forward));
        //         glm::vec3 correctedUp = glm::cross(forward, right);
        //
        //         // Construct the View Matrix Manually
        //         viewMatrix = glm::mat4(
        //             glm::vec4(right, 0.0f),
        //             glm::vec4(correctedUp, 0.0f),
        //             glm::vec4(-forward, 0.0f),
        //             glm::vec4(-glm::dot(right, eye), -glm::dot(correctedUp, eye), glm::dot(forward, eye), 1.0f)
        //         );

        viewMatrix = glm::lookAt(
            glm::vec3(width / 2.0f, height / 2.0f, -cameraDistance), // Flip Z to fix X-axis
            glm::vec3(width / 2.0f, height / 2.0f, 0.0f), // Look at the center
            glm::vec3(0.0f, -1.0f, 0.0f) // Keep Y-up as normal
        );
    }

    void pushMatrix()
    {
        matrixStack.push_back(currentMatrix);
    }

    void popMatrix()
    {
        if (!matrixStack.empty())
        {
            currentMatrix = matrixStack.back();
            matrixStack.pop_back();
        }
    }

    void translate(float x, float y, float z = 0)
    {
        currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, z));
    }

    void scale(float sx, float sy, float sz = 1)
    {
        currentMatrix = glm::scale(currentMatrix, glm::vec3(sx, sy, sz));
    }

    void rotate(float angle)
    {
        currentMatrix = glm::rotate(currentMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void rotate(float angle, glm::vec3 axis)
    {
        currentMatrix = glm::rotate(currentMatrix, angle, axis);
    }

    void line(float x1, float y1, float x2, float y2, glm::vec3 color)
    {
        addVertex(x1, y1, color);
        addVertex(x2, y2, color);
        numVertices += 2;
    }

    void rect(float x, float y, float w, float h, glm::vec3 color)
    {
        addVertex(x, y, color);
        addVertex(x + w, y, color);
        addVertex(x + w, y, color);
        addVertex(x + w, y + h, color);
        addVertex(x + w, y + h, color);
        addVertex(x, y + h, color);
        addVertex(x, y + h, color);
        addVertex(x, y, color);
        numVertices += 8;
    }

    void flush()
    {
        if (numVertices == 0) return;

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

        glUseProgram(shaderProgram);

        // Upload the projection matrix (set this once)
        GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
        //         glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection2D));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection3D));
        GLint viewLoc = glGetUniformLocation(shaderProgram, "uViewMatrix");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        // Upload the model matrix per shape
        GLint matrixLoc = glGetUniformLocation(shaderProgram, "uModelMatrix");
        glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(currentMatrix));

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, numVertices);
        glBindVertexArray(0);

        vertices.clear();
        numVertices = 0;
    }

    void print_matrix()
    {
        printMatrix(currentMatrix);
    }

    void cleanup()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

private:
    GLuint VAO, VBO;
    GLuint shaderProgram;
    glm::mat4 currentMatrix;
    std::vector<glm::mat4> matrixStack;
    std::vector<float> vertices;
    int numVertices = 0;
    float aspectRatio;
    glm::mat4 projection2D;
    glm::mat4 projection3D;
    glm::mat4 viewMatrix;

    void printMatrix(const glm::mat4& matrix)
    {
        for (int i = 0; i < 4; ++i)
        {
            // Iterate over rows
            for (int j = 0; j < 4; ++j)
            {
                // Iterate over columns
                std::cout << matrix[j][i] << "\t"; // Access column-major elements
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void initBuffers()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 1024 * 1024, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    //     void resizeBuffer(size_t newSize) {
    //         std::vector<float> newVertices;
    //         newVertices.reserve(newSize);
    //
    //         // Copy existing vertices to the new buffer
    //         newVertices.insert(newVertices.end(), vertices.begin(), vertices.end());
    //
    //         // Swap the old buffer with the new buffer
    //         vertices.swap(newVertices);
    //         maxBufferSize = newSize * sizeof(float);
    //     }
    //
    //     void addVertex(float x, float y, glm::vec3 color) {
    //         if (vertices.size() + 6 > maxBufferSize / sizeof(float)) {
    //             resizeBuffer(vertices.size() * 1.5); // Resize to 1.5x current size
    //         }
    //
    //         glm::vec4 transformed = currentMatrix * glm::vec4(x, y, 0.0f, 1.0f);  // Apply transformation
    //
    //         vertices.push_back(transformed.x);
    //         vertices.push_back(transformed.y);
    //         vertices.push_back(transformed.z);
    //         vertices.push_back(color.r);
    //         vertices.push_back(color.g);
    //         vertices.push_back(color.b);
    //         numVertices++;
    //     }

    void addVertex(float x, float y, glm::vec3 color)
    {
        glm::vec4 transformed = currentMatrix * glm::vec4(x, y, 0.0f, 1.0f); // Apply transformation

        vertices.push_back(transformed.x);
        vertices.push_back(transformed.y);
        vertices.push_back(transformed.z); // Keep Z in case of 3D use
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
    }
};
