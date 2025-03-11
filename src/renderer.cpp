
// Renderer.cpp

#include <SDL3/SDL.h>
#include "renderer.h"
#include <iostream>
#include <math.h>

Matrix4 Matrix4::identity() {
    Matrix4 mat = {};
    for (int i = 0; i < 4; i++)
        mat.m[i][i] = 1;
    return mat;
}

Matrix4 Matrix4::translate(float tx, float ty, float tz) {
    Matrix4 mat = Matrix4::identity();
    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;
    return mat;
}

Matrix4 Matrix4::lookAt(Vector3 eye, Vector3 target, Vector3 up) {
    Vector3 z = (eye - target).normalize();  // Forward (Z-axis) direction where the "camera" looks
    Vector3 x = up.cross(z).normalize();     // Right (X-axis) direction perpendicular to forward and up
    Vector3 y = z.cross(x).normalize();      // Up (Y-axis) direction perpendicular to right and forward

    // This looks confusing trust the process
    Matrix4 mat = Matrix4::identity();
    mat.m[0][0] = x.x; mat.m[0][1] = x.y; mat.m[0][2] = x.z; mat.m[0][3] = -x.dot(eye);
    mat.m[1][0] = y.x; mat.m[1][1] = y.y; mat.m[1][2] = y.z; mat.m[1][3] = -y.dot(eye);
    mat.m[2][0] = z.x; mat.m[2][1] = z.y; mat.m[2][2] = z.z; mat.m[2][3] = -z.dot(eye);

    return mat;
}

Matrix4 Matrix4::perspective(float fov, float aspect, float z_near, float z_far) {
    float tanHalfFOV = tan((fov * M_PI / 180.0f)/ 2.0f);
    Matrix4 mat = {};
    mat.m[0][0] = (1.0f / (aspect *  tanHalfFOV));
    mat.m[1][1] = 1.0f / tanHalfFOV;
    mat.m[2][2] = z_far / (z_far - z_near);
    mat.m[2][3] = (-z_far * z_near) / (z_far - z_near);     
    mat.m[3][2] = -1.0f;                                    // -1 instead of 1, because rendering window coordinates have reversed y-axis
    return mat;
}

void Object::renderWireframe(SDL_Renderer* renderer, Object::Wireframe* wireframe, float camX, float camY, float camZ,
    float fov, float width, float height, Vector3 target, const uint8_t* color) {

    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);

    Matrix4 model = Matrix4::translate(wireframe->location.x, wireframe->location.y, wireframe->location.z);
    Vector3 viewpoint = Vector3(camX, camY, camZ);
    Vector3 up = Vector3(0,1,0);
    Matrix4 view = Matrix4::lookAt(viewpoint, target, up);
    Matrix4 projection = Matrix4::perspective(fov, width/height, 0.1f, 50.0f);

    Matrix4 MVP = projection * view * model;

    int verticeCount = wireframe->vertices.size();
    int edgeCount = wireframe->edges.size();

    Vector2 projectedVertices[verticeCount];
    
    for (int i = 0; i < verticeCount; i++) {
        Vector3 projected = MVP * wireframe->vertices[i];
    
        projectedVertices[i].x = (projected.x + 1) * width/2;
        projectedVertices[i].y = (1 - projected.y) * height/2;   
    }
    for(int i = 0; i < edgeCount; i++)
    {
        SDL_RenderLine(renderer, projectedVertices[wireframe->edges[i].first].x, projectedVertices[wireframe->edges[i].first].y,
            projectedVertices[wireframe->edges[i].second].x, projectedVertices[wireframe->edges[i].second].y);
    }
}

void Object::renderWireframe_tri(SDL_Renderer* renderer, Object::Wireframe_tri* wireframe, float camX, float camY, float camZ,
    float fov, float width, float height, Vector3 target, const uint8_t* color) {

    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);

    Matrix4 model = Matrix4::translate(wireframe->origin.x, wireframe->origin.y, wireframe->origin.z);
    Vector3 viewpoint = Vector3(camX, camY, camZ);
    Vector3 up = Vector3(0,1,0);
    Matrix4 view = Matrix4::lookAt(viewpoint, target, up);
    Matrix4 projection = Matrix4::perspective(fov, width/height, 0.1f, 50.0f);

    Matrix4 MVP = projection * view * model;

    for (int i = 0; i < wireframe->triangles.size(); i++) {
        Triangle& tri = wireframe->triangles[i];

        // Maybe a little scuffed way to compute the view direction from the viewport to a triangle
        Vector3 viewDir = (tri.vertices[0] - viewpoint).normalize();

        float view_normal_alignment = viewDir.dot(tri.normal.normalize());

        if(view_normal_alignment < 0) {

            Vector2 projectedVertices[3];

            for(int k = 0; k < 3; k++) {
                
                Vector3 projected = MVP * tri.vertices[k];

                projectedVertices[k].x = (projected.x + 1) * width/2;
                projectedVertices[k].y = (1 - projected.y) * height/2; 
            }

            SDL_RenderLine(renderer, projectedVertices[0].x, projectedVertices[0].y, projectedVertices[1].x, projectedVertices[1].y);
            SDL_RenderLine(renderer, projectedVertices[0].x, projectedVertices[0].y, projectedVertices[2].x, projectedVertices[2].y);
            SDL_RenderLine(renderer, projectedVertices[1].x, projectedVertices[1].y, projectedVertices[2].x, projectedVertices[2].y);
        }
    }    
}

SDL_Renderer* initRenderer(SDL_Window* window)
{
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if(!renderer)
    {
        SDL_Log("Error creating renderer: %s", SDL_GetError());
        return nullptr;
    }
    return renderer;
}

void drawLine(SDL_Renderer* renderer, float x, float y, double angle, double length, const uint8_t* color)
{
    float x_end = x + length * SDL_cos(angle);
    float y_end = y + length * SDL_sin(angle);

    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);

    SDL_RenderLine(renderer, x, y, x_end, y_end);
}

void drawLine_pixel(SDL_Renderer* renderer, float x, float y, float x_end, float y_end) {

    SDL_RenderPoint(renderer, x, y);
}

void cleanupRenderer(SDL_Renderer* renderer)
{
    if(renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
}