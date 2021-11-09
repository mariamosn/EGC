#include "lab_m1/tema1/tema1.h"

#include <vector>
#include <iostream>
#include <math.h>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 0;       // logic x
    logicSpace.y = 0;       // logic y
    logicSpace.width = 20;   // logic width
    logicSpace.height = 20;  // logic height

    glm::vec3 corner = glm::vec3(0, 0, 0);
    glm::vec3 center1 = glm::vec3(0, 0, -0.1);
    glm::vec3 center2 = glm::vec3(0, 0, 0.1);
    length = 0.99f;

    Mesh* square1 = object2D::CreateSquare("square1", corner, length, glm::vec3(1, 1, 1), true);
    AddMeshToList(square1);

    Mesh* circle2 = object2D::CreateCircle("circle2", center2, length, glm::vec3(0, 1, 1));
    AddMeshToList(circle2);

    Mesh* circle1 = object2D::CreateCircle("circle1", corner, length, glm::vec3(1, 1, 1));
    AddMeshToList(circle1);

    Mesh* triangle = object2D::CreateTriangle("triangle", center1, length, glm::vec3(0, 1, 0));
    AddMeshToList(triangle);

    x_char = logicSpace.width / 2;
    y_char = logicSpace.height / 2;
    rad_char = 0;
}


// 2D visualization matrix
glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
    float sx, sy, tx, ty;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    tx = viewSpace.x - sx * logicSpace.x;
    ty = viewSpace.y - sy * logicSpace.y;

    return glm::transpose(glm::mat3(
        sx, 0.0f, tx,
        0.0f, sy, ty,
        0.0f, 0.0f, 1.0f));
}


// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}


void Tema1::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Tema1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();

    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0), true);

    vx_char = x_char;
    vy_char = y_char + length / 2;

    // Compute uniform 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);
    DrawScene(visMatrix);

}


void Tema1::FrameEnd()
{
}


void Tema1::DrawScene(glm::mat3 visMatrix)
{
    modelMatrix = visMatrix * transform2D::Translate(x_char, y_char) * transform2D::Rotate(rad_char) * transform2D::Translate(0, 0);
    RenderMesh2D(meshes["circle1"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(x_char, y_char) * transform2D::Rotate(rad_char) * transform2D::Translate(-0.3, 0.9) * transform2D::Scale(0.2, 0.2);
    RenderMesh2D(meshes["circle2"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(x_char, y_char) * transform2D::Rotate(rad_char) * transform2D::Translate(0.3, 0.9) * transform2D::Scale(0.2, 0.2);
    RenderMesh2D(meshes["circle2"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(x_char, y_char) * transform2D::Rotate(rad_char) * transform2D::Translate(-0.6, -1.2) * transform2D::Scale(1.2, 1.2);
    RenderMesh2D(meshes["triangle"], shaders["VertexColor"], modelMatrix);

    /*
    modelMatrix = visMatrix * transform2D::Translate(3, 0);
    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(1.5, 1.5);
    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(0, 3);
    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(3, 3);
    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);
    */
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    if (window->KeyHold(GLFW_KEY_W)) {
        y_char += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        x_char -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        y_char -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        x_char += deltaTime;
    }
    /*
    // TODO(student): Move the logic window with W, A, S, D (up, left, down, right)
    if (window->KeyHold(GLFW_KEY_W)) {
        logicSpace.y += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        logicSpace.x -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        logicSpace.y -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        logicSpace.x += deltaTime;
    }

    // TODO(student): Zoom in and zoom out logic window with Z and X
    if (window->KeyHold(GLFW_KEY_Z)) {
        logicSpace.height -= deltaTime;
        logicSpace.width -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_X)) {
        logicSpace.height += deltaTime;
        logicSpace.width += deltaTime;
    }
    */
}


void Tema1::OnKeyPress(int key, int mods)
{
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    float x_mouse = mouseX / logicSpace.width / 2 - logicSpace.width / 4;
    float y_mouse = logicSpace.height - (mouseY / logicSpace.height) / 2;
    if (y_mouse > y_char) {
        rad_char = -atan((x_mouse - x_char) / (y_mouse - y_char));
    }
    else {
        rad_char = 3.1415 - atan((x_mouse - x_char) / (y_mouse - y_char));
    }
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}
