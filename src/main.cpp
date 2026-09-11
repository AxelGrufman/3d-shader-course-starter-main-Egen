#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

    constexpr int WindowWidth = 900;
    constexpr int WindowHeight = 600;

    constexpr unsigned int WaterResolution = 128;
    constexpr float WaterSize = 8.0f;

    struct WaterVertex
    {
        float px;
        float py;
        float pz;

        float nx;
        float ny;
        float nz;

        float u;
        float v;
    };

    struct WaterMesh
    {
        std::vector<WaterVertex> vertices;
        std::vector<unsigned int> indices;
    };

    struct Camera
    {
        glm::vec3 position{ 0.0f, 2.8f, 5.5f };
        glm::vec3 front{ 0.0f, 0.0f, -1.0f };
        glm::vec3 up{ 0.0f, 1.0f, 0.0f };
        glm::vec3 right{ 1.0f, 0.0f, 0.0f };

        const glm::vec3 worldUp{ 0.0f, 1.0f, 0.0f };

        float yaw = -90.0f;
        float pitch = -20.0f;

        float movementSpeed = 3.5f;
        float mouseSensitivity = 0.10f;

        bool firstMouseMovement = true;
        double previousMouseX = 0.0;
        double previousMouseY = 0.0;
    };

    void updateCameraVectors(Camera& camera)
    {
        const float yawRadians =
            glm::radians(camera.yaw);

        const float pitchRadians =
            glm::radians(camera.pitch);

        glm::vec3 front;

        front.x =
            std::cos(yawRadians) *
            std::cos(pitchRadians);

        front.y =
            std::sin(pitchRadians);

        front.z =
            std::sin(yawRadians) *
            std::cos(pitchRadians);

        camera.front =
            glm::normalize(front);

        camera.right =
            glm::normalize(
                glm::cross(
                    camera.front,
                    camera.worldUp));

        camera.up =
            glm::normalize(
                glm::cross(
                    camera.right,
                    camera.front));
    }

    void glfwErrorCallback(
        int error,
        const char* description)
    {
        std::cerr
            << "GLFW error ("
            << error
            << "): "
            << description
            << '\n';
    }

    std::string readTextFile(
        const std::string& path)
    {
        std::ifstream file(path);

        if (!file)
        {
            throw std::runtime_error(
                "Could not open file: " + path);
        }

        std::ostringstream contents;
        contents << file.rdbuf();

        return contents.str();
    }

    GLuint compileShader(
        GLenum type,
        const std::string& source,
        const std::string& label)
    {
        const GLuint shader =
            glCreateShader(type);

        const char* sourcePointer =
            source.c_str();

        glShaderSource(
            shader,
            1,
            &sourcePointer,
            nullptr);

        glCompileShader(shader);

        GLint success = GL_FALSE;

        glGetShaderiv(
            shader,
            GL_COMPILE_STATUS,
            &success);

        if (success == GL_FALSE)
        {
            GLint logLength = 0;

            glGetShaderiv(
                shader,
                GL_INFO_LOG_LENGTH,
                &logLength);

            std::string log(
                static_cast<std::size_t>(logLength),
                '\0');

            glGetShaderInfoLog(
                shader,
                logLength,
                nullptr,
                log.data());

            glDeleteShader(shader);

            throw std::runtime_error(
                "Shader compilation failed (" +
                label +
                "):\n" +
                log);
        }

        return shader;
    }

    GLuint createShaderProgram(
        const std::string& vertexPath,
        const std::string& fragmentPath)
    {
        const std::string vertexSource =
            readTextFile(vertexPath);

        const std::string fragmentSource =
            readTextFile(fragmentPath);

        GLuint vertexShader = 0;
        GLuint fragmentShader = 0;
        GLuint program = 0;

        try
        {
            vertexShader =
                compileShader(
                    GL_VERTEX_SHADER,
                    vertexSource,
                    vertexPath);

            fragmentShader =
                compileShader(
                    GL_FRAGMENT_SHADER,
                    fragmentSource,
                    fragmentPath);

            program =
                glCreateProgram();

            glAttachShader(
                program,
                vertexShader);

            glAttachShader(
                program,
                fragmentShader);

            glLinkProgram(program);

            GLint success =
                GL_FALSE;

            glGetProgramiv(
                program,
                GL_LINK_STATUS,
                &success);

            if (success == GL_FALSE)
            {
                GLint logLength = 0;

                glGetProgramiv(
                    program,
                    GL_INFO_LOG_LENGTH,
                    &logLength);

                std::string log(
                    static_cast<std::size_t>(logLength),
                    '\0');

                glGetProgramInfoLog(
                    program,
                    logLength,
                    nullptr,
                    log.data());

                throw std::runtime_error(
                    "Shader program link failed:\n" +
                    log);
            }
        }
        catch (...)
        {
            if (program != 0)
            {
                glDeleteProgram(program);
            }

            if (vertexShader != 0)
            {
                glDeleteShader(vertexShader);
            }

            if (fragmentShader != 0)
            {
                glDeleteShader(fragmentShader);
            }

            throw;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }

    WaterMesh createWaterGrid(
        unsigned int resolution,
        float size)
    {
        if (resolution == 0)
        {
            throw std::invalid_argument(
                "Water grid resolution must be greater than zero.");
        }

        WaterMesh mesh;

        const unsigned int verticesPerSide =
            resolution + 1;

        mesh.vertices.reserve(
            static_cast<std::size_t>(
                verticesPerSide) *
            verticesPerSide);

        mesh.indices.reserve(
            static_cast<std::size_t>(
                resolution) *
            resolution *
            6);

        for (
            unsigned int z = 0;
            z <= resolution;
            ++z)
        {
            for (
                unsigned int x = 0;
                x <= resolution;
                ++x)
            {
                const float u =
                    static_cast<float>(x) /
                    static_cast<float>(
                        resolution);

                const float v =
                    static_cast<float>(z) /
                    static_cast<float>(
                        resolution);

                const float worldX =
                    (u - 0.5f) * size;

                const float worldZ =
                    (v - 0.5f) * size;

                mesh.vertices.push_back({
                    worldX,
                    0.0f,
                    worldZ,

                    0.0f,
                    1.0f,
                    0.0f,

                    u,
                    v
                    });
            }
        }

        for (
            unsigned int z = 0;
            z < resolution;
            ++z)
        {
            for (
                unsigned int x = 0;
                x < resolution;
                ++x)
            {
                const unsigned int topLeft =
                    z * verticesPerSide + x;

                const unsigned int topRight =
                    topLeft + 1;

                const unsigned int bottomLeft =
                    (z + 1) *
                    verticesPerSide +
                    x;

                const unsigned int bottomRight =
                    bottomLeft + 1;

                mesh.indices.push_back(
                    topLeft);

                mesh.indices.push_back(
                    bottomLeft);

                mesh.indices.push_back(
                    topRight);

                mesh.indices.push_back(
                    topRight);

                mesh.indices.push_back(
                    bottomLeft);

                mesh.indices.push_back(
                    bottomRight);
            }
        }

        return mesh;
    }

    void framebufferSizeCallback(
        GLFWwindow*,
        int width,
        int height)
    {
        glViewport(
            0,
            0,
            width,
            height);
    }

    void mouseCallback(
        GLFWwindow* window,
        double mouseX,
        double mouseY)
    {
        auto* camera =
            static_cast<Camera*>(
                glfwGetWindowUserPointer(
                    window));

        if (camera == nullptr)
        {
            return;
        }

        if (camera->firstMouseMovement)
        {
            camera->previousMouseX =
                mouseX;

            camera->previousMouseY =
                mouseY;

            camera->firstMouseMovement =
                false;

            return;
        }

        double xOffset =
            mouseX -
            camera->previousMouseX;

        double yOffset =
            camera->previousMouseY -
            mouseY;

        camera->previousMouseX =
            mouseX;

        camera->previousMouseY =
            mouseY;

        xOffset *=
            camera->mouseSensitivity;

        yOffset *=
            camera->mouseSensitivity;

        camera->yaw +=
            static_cast<float>(
                xOffset);

        camera->pitch +=
            static_cast<float>(
                yOffset);

        camera->pitch =
            std::clamp(
                camera->pitch,
                -89.0f,
                89.0f);

        updateCameraVectors(
            *camera);
    }

    void processInput(
        GLFWwindow* window,
        Camera& camera,
        float deltaTime)
    {
        if (
            glfwGetKey(
                window,
                GLFW_KEY_ESCAPE) ==
            GLFW_PRESS)
        {
            glfwSetWindowShouldClose(
                window,
                GLFW_TRUE);
        }

        const float velocity =
            camera.movementSpeed *
            deltaTime;

        if (
            glfwGetKey(
                window,
                GLFW_KEY_W) ==
            GLFW_PRESS)
        {
            camera.position +=
                camera.front *
                velocity;
        }

        if (
            glfwGetKey(
                window,
                GLFW_KEY_S) ==
            GLFW_PRESS)
        {
            camera.position -=
                camera.front *
                velocity;
        }

        if (
            glfwGetKey(
                window,
                GLFW_KEY_A) ==
            GLFW_PRESS)
        {
            camera.position -=
                camera.right *
                velocity;
        }

        if (
            glfwGetKey(
                window,
                GLFW_KEY_D) ==
            GLFW_PRESS)
        {
            camera.position +=
                camera.right *
                velocity;
        }

        if (
            glfwGetKey(
                window,
                GLFW_KEY_SPACE) ==
            GLFW_PRESS)
        {
            camera.position +=
                camera.worldUp *
                velocity;
        }

        if (
            glfwGetKey(
                window,
                GLFW_KEY_LEFT_SHIFT) ==
            GLFW_PRESS)
        {
            camera.position -=
                camera.worldUp *
                velocity;
        }
    }

} 

int main()
{
    glfwSetErrorCallback(
        glfwErrorCallback);

    if (glfwInit() != GLFW_TRUE)
    {
        std::cerr
            << "Failed to initialize GLFW.\n";

        return 1;
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3);

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GLFW_TRUE);
#endif

    GLFWwindow* window =
        glfwCreateWindow(
            WindowWidth,
            WindowHeight,
            "Realistic Water",
            nullptr,
            nullptr);

    if (window == nullptr)
    {
        std::cerr
            << "Failed to create a GLFW window.\n";

        glfwTerminate();

        return 1;
    }

    glfwMakeContextCurrent(
        window);

    glfwSetFramebufferSizeCallback(
        window,
        framebufferSizeCallback);

    glfwSwapInterval(1);

    const int loadedVersion =
        gladLoadGL(
            glfwGetProcAddress);

    if (loadedVersion == 0)
    {
        std::cerr
            << "Failed to load OpenGL functions with GLAD.\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return 1;
    }

    std::cout
        << "OpenGL: "
        << glGetString(GL_VERSION)
        << '\n';

    std::cout
        << "Renderer: "
        << glGetString(GL_RENDERER)
        << '\n';

    glEnable(
        GL_DEPTH_TEST);

    Camera camera;

    updateCameraVectors(
        camera);

    glfwSetWindowUserPointer(
        window,
        &camera);

    glfwSetCursorPosCallback(
        window,
        mouseCallback);


    glfwSetInputMode(
        window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED);

    WaterMesh waterMesh;

    try
    {
        waterMesh =
            createWaterGrid(
                WaterResolution,
                WaterSize);
    }
    catch (
        const std::exception& exception)
    {
        std::cerr
            << exception.what()
            << '\n';

        glfwDestroyWindow(window);
        glfwTerminate();

        return 1;
    }

    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    glGenVertexArrays(
        1,
        &vao);

    glGenBuffers(
        1,
        &vbo);

    glGenBuffers(
        1,
        &ebo);

    glBindVertexArray(
        vao);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(
            waterMesh.vertices.size() *
            sizeof(WaterVertex)),
        waterMesh.vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        ebo);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(
            waterMesh.indices.size() *
            sizeof(unsigned int)),
        waterMesh.indices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(WaterVertex),
        reinterpret_cast<void*>(
            offsetof(
                WaterVertex,
                px)));

    glEnableVertexAttribArray(
        0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(WaterVertex),
        reinterpret_cast<void*>(
            offsetof(
                WaterVertex,
                nx)));

    glEnableVertexAttribArray(
        1);

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(WaterVertex),
        reinterpret_cast<void*>(
            offsetof(
                WaterVertex,
                u)));

    glEnableVertexAttribArray(
        2);

    glBindVertexArray(
        0);

    GLuint shaderProgram = 0;

    try
    {
        shaderProgram =
            createShaderProgram(
                "shaders/basic.vert",
                "shaders/basic.frag");
    }
    catch (
        const std::exception& exception)
    {
        std::cerr
            << exception.what()
            << '\n';

        glDeleteBuffers(
            1,
            &ebo);

        glDeleteBuffers(
            1,
            &vbo);

        glDeleteVertexArrays(
            1,
            &vao);

        glfwDestroyWindow(window);
        glfwTerminate();

        return 1;
    }

    const GLint modelLocation =
        glGetUniformLocation(
            shaderProgram,
            "model");

    const GLint viewLocation =
        glGetUniformLocation(
            shaderProgram,
            "view");

    const GLint projectionLocation =
        glGetUniformLocation(
            shaderProgram,
            "projection");

    const GLint normalMatrixLocation =
        glGetUniformLocation(
            shaderProgram,
            "normalMatrix");

    const GLint timeLocation =
        glGetUniformLocation(
            shaderProgram,
            "time");

    const GLint lightDirectionLocation =
        glGetUniformLocation(
            shaderProgram,
            "lightDirection");

    const GLint lightColorLocation =
        glGetUniformLocation(
            shaderProgram,
            "lightColor");

    const GLint viewPositionLocation =
        glGetUniformLocation(
            shaderProgram,
            "viewPosition");

    const GLint baseColorLocation =
        glGetUniformLocation(
            shaderProgram,
            "baseColor");

    const GLint ambientStrengthLocation =
        glGetUniformLocation(
            shaderProgram,
            "ambientStrength");

    const GLint specularStrengthLocation =
        glGetUniformLocation(
            shaderProgram,
            "specularStrength");

    const GLint shininessLocation =
        glGetUniformLocation(
            shaderProgram,
            "shininess");

    const glm::mat4 model(
        1.0f);

    const glm::mat3 normalMatrix =
        glm::transpose(
            glm::inverse(
                glm::mat3(
                    model)));

    const glm::vec3 lightDirection =
        glm::normalize(
            glm::vec3(
                0.35f,
                1.0f,
                0.45f));

    const glm::vec3 lightColor(
        1.0f,
        0.97f,
        0.90f);

    const glm::vec3 baseColor(
        0.015f,
        0.22f,
        0.32f);

    const float ambientStrength =
        0.12f;

    const float specularStrength =
        0.45f;

    const float shininess =
        96.0f;

    const float fieldOfView =
        glm::radians(
            45.0f);

    const float nearPlane =
        0.1f;

    const float farPlane =
        100.0f;

    const GLsizei indexCount =
        static_cast<GLsizei>(
            waterMesh.indices.size());

    float previousFrameTime =
        static_cast<float>(
            glfwGetTime());

    while (
        glfwWindowShouldClose(
            window) ==
        GLFW_FALSE)
    {
        const float currentFrameTime =
            static_cast<float>(
                glfwGetTime());

        const float deltaTime =
            currentFrameTime -
            previousFrameTime;

        previousFrameTime =
            currentFrameTime;

        processInput(
            window,
            camera,
            deltaTime);

        int framebufferWidth = 0;
        int framebufferHeight = 0;

        glfwGetFramebufferSize(
            window,
            &framebufferWidth,
            &framebufferHeight);

        if (
            framebufferWidth == 0 ||
            framebufferHeight == 0)
        {
            glfwPollEvents();
            continue;
        }

        const float aspectRatio =
            static_cast<float>(
                framebufferWidth) /
            static_cast<float>(
                framebufferHeight);

        const glm::mat4 projection =
            glm::perspective(
                fieldOfView,
                aspectRatio,
                nearPlane,
                farPlane);

        const glm::mat4 view =
            glm::lookAt(
                camera.position,
                camera.position +
                camera.front,
                camera.up);

        glViewport(
            0,
            0,
            framebufferWidth,
            framebufferHeight);

        glClearColor(
            0.025f,
            0.055f,
            0.09f,
            1.0f);

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT);

        glUseProgram(
            shaderProgram);

        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            glm::value_ptr(
                model));

        glUniformMatrix4fv(
            viewLocation,
            1,
            GL_FALSE,
            glm::value_ptr(
                view));

        glUniformMatrix4fv(
            projectionLocation,
            1,
            GL_FALSE,
            glm::value_ptr(
                projection));

        glUniformMatrix3fv(
            normalMatrixLocation,
            1,
            GL_FALSE,
            glm::value_ptr(
                normalMatrix));

        glUniform1f(
            timeLocation,
            currentFrameTime);

        glUniform3fv(
            lightDirectionLocation,
            1,
            glm::value_ptr(
                lightDirection));

        glUniform3fv(
            lightColorLocation,
            1,
            glm::value_ptr(
                lightColor));


        glUniform3fv(
            viewPositionLocation,
            1,
            glm::value_ptr(
                camera.position));

        glUniform3fv(
            baseColorLocation,
            1,
            glm::value_ptr(
                baseColor));

        glUniform1f(
            ambientStrengthLocation,
            ambientStrength);

        glUniform1f(
            specularStrengthLocation,
            specularStrength);

        glUniform1f(
            shininessLocation,
            shininess);

        glBindVertexArray(
            vao);

        glDrawElements(
            GL_TRIANGLES,
            indexCount,
            GL_UNSIGNED_INT,
            nullptr);

        glBindVertexArray(
            0);

        glfwSwapBuffers(
            window);

        glfwPollEvents();
    }

    glDeleteProgram(
        shaderProgram);

    glDeleteBuffers(
        1,
        &ebo);

    glDeleteBuffers(
        1,
        &vbo);

    glDeleteVertexArrays(
        1,
        &vao);

    glfwDestroyWindow(
        window);

    glfwTerminate();

    return 0;
}