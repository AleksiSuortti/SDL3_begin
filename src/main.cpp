#include <iostream>
#include <SDL3/SDL.h>
//#include <SDL3_ttf/SDL_ttf.h>
#include "renderer.h"
#include <math.h>
#include <string>

#define WIDTH 600
#define HEIGHT 400

int main()
{
    
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    // Create the window
    SDL_Window* window = SDL_CreateWindow("SDL3 Testproject", WIDTH, HEIGHT, 0);
    
    if(!window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = initRenderer(window);

    if(!renderer)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Event event;

    bool program_running = 1;
    float fov = 90;
    float camX = 0, camY = 3, camZ = 0;
    float r = 10;
    float angle = 0;
    Vector3 target = {0,0,0};

    Object::Wireframe cube;

    cube.vertices = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
        {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}
    };

    cube.edges = {
        {0,1}, {1,2}, {2,3}, {3,0},  // Back face
        {4,5}, {5,6}, {6,7}, {7,4},  // Front face
        {0,4}, {1,5}, {2,6}, {3,7}   // Connecting edges
    };

    Object::Wireframe cube2;
    cube2 = cube;
    cube2.location = {5,5,5};
    Object::Wireframe plane;

    plane.vertices = {
        {-5,-1,-5}, {5,-1,-5}, {5,-1,5}, {-5,-1,5}
    };

    plane.edges = {
        {0,1},{1,2},{2,3},{3,0}
    };

    Object::Wireframe_tri plane_tri;

    plane_tri.addTriangles({{Vector3(-5, -1, -5), Vector3(5, -1, -5), Vector3(5, -1, 5)}});
    plane_tri.addTriangles({{Vector3(-5, -1, -5), Vector3(-5, -1, 5), Vector3(5, -1, 5)}});
    while(program_running)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_EVENT_QUIT)
            {
                program_running = 0;
            }
            if(event.type == SDL_EVENT_MOUSE_WHEEL)
            {
                if(angle == 360 || angle == -360) {
                    angle = 0;
                }
                fov += event.wheel.y;
                printf("FOV: %f\n", fov);
                angle += 3 * event.wheel.x;
                camX = r * sin(angle*(2*M_PI/360));
                camZ = r * cos(angle*(2*M_PI/360));
                
                //cube2.location = cube2.location - (Vector3){event.wheel.x,event.wheel.x,event.wheel.x};
                printf("Viewpoint loc: x=%f, y=%f, z=%f, angle=%f\n", camX, camY, camZ, angle);
            }
        }
    
        SDL_SetRenderViewport(renderer, NULL);
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderFillRect(renderer, NULL);
        SDL_SetRenderDrawColor(renderer, 255,255,255,0);

        Object::renderWireframe(renderer, &cube, camX, camY, camZ, fov, WIDTH, HEIGHT,
            target, COLOR_WHITE);
        Object::renderWireframe(renderer, &cube2, camX, camY, camZ, fov, WIDTH, HEIGHT,
            target, COLOR_RED);
        Object::renderWireframe(renderer, &plane, camX, camY, camZ, fov, WIDTH, HEIGHT,
            target, COLOR_WHITE);

        Object::renderWireframe_tri(renderer, &plane_tri, camX, camY, camZ, fov, WIDTH, HEIGHT,
        target, COLOR_RED);

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}