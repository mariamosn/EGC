#include "lab_m1/lab3/lab32.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab32::Lab32()
{
}


Lab32::~Lab32()
{
}


void Lab32::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;

    // Initialize tx and ty (the translation steps)
    translateX = 0;
    translateY = 0;

    // Initialize sx and sy (the scale factors)
    scaleX = 1;
    scaleY = 1;

    // Initialize angularStep
    angularStep1 = 0;
    angularStep2 = 0;

    Mesh* square1 = object2D::CreateSquare("square1", corner, squareSide, glm::vec3(1, 0, 0), true);
    AddMeshToList(square1);

    Mesh* square2 = object2D::CreateSquare("square2", corner, squareSide, glm::vec3(0, 1, 0), true);
    AddMeshToList(square2);

    Mesh* square3 = object2D::CreateSquare("square3", corner, squareSide, glm::vec3(0, 0, 1), true);
    AddMeshToList(square3);

    x1 = resolution.x / 2 - squareSide / 2;
    y1 = resolution.y / 2 - squareSide / 2;

    cx = x1 + squareSide / 2;
    cy = y1 + squareSide / 2;

    r1 = 200;
    x2 = cx + r1;
    y2 = y1;

    cx2 = x2 + squareSide / 2;
    cy2 = y2 + squareSide / 2;

    r2 = 100;
    x3 = cx2 + r2;
    y3 = y2;
}


void Lab32::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab32::Update(float deltaTimeSeconds)
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(x1, y1);

    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);


    modelMatrix = glm::mat3(1);

    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Rotate(angularStep1);
    modelMatrix *= transform2D::Translate(-cx, -cy);
    modelMatrix *= transform2D::Translate(x2, y2);

    RenderMesh2D(meshes["square2"], shaders["VertexColor"], modelMatrix);


    modelMatrix = glm::mat3(1);

    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Rotate(angularStep1);
    modelMatrix *= transform2D::Translate(-cx, -cy);

    modelMatrix *= transform2D::Translate(cx2, cy2);
    modelMatrix *= transform2D::Rotate(angularStep2);
    modelMatrix *= transform2D::Translate(-cx2, -cy2);
    modelMatrix *= transform2D::Translate(x3, y3);

    RenderMesh2D(meshes["square3"], shaders["VertexColor"], modelMatrix);


    angularStep1 += deltaTimeSeconds;
    angularStep2 += deltaTimeSeconds * 2;
}


void Lab32::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab32::OnInputUpdate(float deltaTime, int mods)
{
}


void Lab32::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Lab32::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab32::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab32::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab32::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab32::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab32::OnWindowResize(int width, int height)
{
}
