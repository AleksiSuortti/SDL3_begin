
// renderer.h

#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
//#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>
#include <vector>
#include <array>

/**
 * RBGA color representation of white {R, G, B, A} 
 */
constexpr uint8_t COLOR_WHITE[4] = {255,255,255,255};
constexpr uint8_t COLOR_BLACK[4] = {0,0,0,255};
constexpr uint8_t COLOR_RED[4] = {255,0,0,255};
constexpr uint8_t COLOR_GREEN[4] = {0,255,0,255};
constexpr uint8_t COLOR_BLUE[4] = {0,0,255,255};

struct Z_buffer {
    float* z_buffer;
    int width;
    int height;

    Z_buffer(int w, int h) : width(w), height(h) {

        z_buffer = new float[width * height];
        std::fill(z_buffer, z_buffer + (width * height), 1.0f); 
    }

    ~Z_buffer() {
        delete[] z_buffer;
    }

    float& get(int x, int y) {
        return z_buffer[y * width + x];
    }
};

struct Vector3 {

    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    /**
     * @brief Overload the - operator for 3d-vectors
     * \param v Vector to subtract from this
     * \return Vector3 struct
     */
    Vector3 operator-(const Vector3& v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    /**
     * @brief Make a unit vector of a 3d-vector
     * \return Normalized unit vector of Vector3
     */
    Vector3 normalize() const {
        float SS = x * x + y * y + z * z;
        float magnitude = std::sqrt(SS);
        return Vector3(x/magnitude, y/magnitude, z/magnitude);
    }

    /**
     * @brief Crossproduct of two 3d-vectors
     * \param v Vector to calculate cross product with this
     * \return 
     */
    Vector3 cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    /**
     * @brief Dotproduct of two 3d-vectors
     * \param v Vector to calculate dot product with this
     * \return Dot product float
     */
    float dot(const Vector3& v) const {
        return x * v.x + y* v.y + z * v.z;
    }
};


namespace Object {

    struct Wireframe {

        std::vector<Vector3> vertices;
        std::vector<std::pair<int, int>> edges;
        Vector3 location;

        Wireframe() : vertices(), edges(), location(0,0,0) {}
        Wireframe(std::vector<Vector3> vertices, std::vector<std::pair<int, int>> edges, Vector3 location)
        : vertices(vertices), edges(edges), location(location) {}
    };


    struct Triangle {

        std::array<Vector3, 3> vertices;
        Vector3 normal;

        Triangle() : vertices(), normal() {}

        // This implementation is a little awkward, change when necessary
        Triangle(const std::array<Vector3, 3> &verts) : vertices(verts),
            normal((verts[1] - verts[0]).cross(verts[2] - verts[0])) {}
    };

    struct Wireframe_tri {

        std::vector<Triangle> triangles;
        Vector3 origin;

        Wireframe_tri() : triangles(), origin() {}
        Wireframe_tri(std::vector<Triangle> triangles, Vector3 origin) :
            triangles(triangles), origin(origin) {}

        void addTriangles(std::array<Vector3, 3> add) {
            
            Object::Triangle temp = {add};
            triangles.push_back(temp);
        }
    };

    /**
     * @brief Renders wireframe objects to renderer
     * \param renderer rendering context, where image is rendered
     * \param wireframe Object to be rendered
     * \param camX, camY, camZ "Camera" or viewport location in the scene
     * \param target Point in the scene which the viewport is facing at
     * \param fov Field of view of the viewport
     * \param width, height Viewing window width and height
     * \param color RGBA color for drawing the object
     */
    void renderWireframe(SDL_Renderer* renderer, Object::Wireframe* wireframe, float camX, float camY, float camZ,
        float fov, float width, float height, Vector3 target, const uint8_t* color);

    /**
     * @brief Renders triangle-based objects to the renderer
     */
    void renderWireframe_tri(SDL_Renderer* renderer, Z_buffer* z_buffer, Object::Wireframe_tri* wireframe, float camX, float camY, float camZ,
        float fov, float width, float height, Vector3 target, const uint8_t* color);

}


struct Vector2 {
    float x, y;

    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
};

struct Matrix4 {

    float m[4][4];

    /**
     * @brief Override the * operator for Matrix4 type, for matrix multiplication
     * \param m_2 Matrix to be multiplied with this
     * \return Matrix4 type product
     */
    Matrix4 operator*(const Matrix4& m_2) const {
        Matrix4 result = {};
        
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                result.m[i][j] = 0;
                for(int k = 0; k < 4; k++)
                {
                    result.m[i][j] += m[i][k] * m_2.m[k][j];
                }
            }   
        }
        return result;
    }

    /**
     * @brief Override the * operator for Vector3 for matrix-vector multiplication
     * \param v Vector to multiply with matrix
     * \return Vector3, the multiplication product
     */
    Vector3 operator*(const Vector3& v) const {
        float x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3];
        float y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3];
        float z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3];
        float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3];

        if (w != 0.0f) {
            x /= w;
            y /= w;
            z /= w;
        }

        return Vector3(x, y, z);
    }
    /**
     * @brief Calculate identity matrix of Matrix4
     * \return Matrix4, the identity matrix
     */
    static Matrix4 identity();

    /**
     * @brief Create a translation (model) matrix for a matrix
     * \param tx Translation parameter
     * \param ty -||-
     * \param tz -||-
     */
    static Matrix4 translate(float tx, float ty, float tz);

    /**
     * @brief Create a view-matrix
     * \param eye "Camera" position, where the camera is in
     * \param target Where the "camera" is pointing
     * \param up What direction is up in the frame of reference of the "camera"
     */
    static Matrix4 lookAt(Vector3 eye, Vector3 target, Vector3 up);

    /**
     * @brief Projection matrix (perspective)
     */
    static Matrix4 perspective(float fov, float aspect, float near, float far);
};

/**
 * @brief Initializes a renderer
 * 
 * \param window window where rendering is shown
 * \return returns pointer to initialized renderer
 */
SDL_Renderer* initRenderer(SDL_Window* window);

/**
 * @brief Draws line on the rendering buffer
 * 
 * \param renderer renderer in use
 * \param x x-coordinate of the line starting point
 * \param y y-coordinate of the line starting point
 * \param angle angle of the line in radians
 * \param length length of the line in pixels
 * \param color uint8_t array, {R, G, B, A} values of a color
 */
void drawLine(SDL_Renderer* renderer, float x, float y, double angle, double length, const uint8_t* color);

/**
 * @brief Draws a line pixel by pixel
 */
void drawLine_pixel(SDL_Renderer* renderer, float x, float y, float x_end, float y_end);

/**
 * @brief Draws a single pixel
 */
void drawPixel(SDL_Renderer* renderer, float x, float y);

void cleanupRenderer(SDL_Renderer * renderer);

#endif // RENDERER_H
