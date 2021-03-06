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

bool Tema1::ok_obstacle(float x, float y, float len) {
    for (int i = 0; i < x_o.size(); i++) {
        if (x + len >= x_o[i] && x - len <= x_o[i] + sx_o[i] &&
            y + len >= y_o[i] && y - len <= y_o[i] + sy_o[i])  {
            return false;
        }
    }
    return true;
}

bool Tema1::target_enemy(int id) {
    for (int i = 0; i < x_en.size() && pr_status[id]; i++) {
        if (en_status[i] &&
            x_pr[id] + len_pr / 2 >= x_en[i] - len_en / 2 && x_pr[id] - len_pr / 2 <= x_en[i] + len_en / 2 &&
            y_pr[id] + len_pr / 2 >= y_en[i] - len_en / 2 && y_pr[id] - len_pr / 2 <= y_en[i] + len_en / 2) {
            en_status[i] = false;
            pr_status[id] = false;
            if (score < 10) {
                score++;
            }
            return true;
        }
    }
    return false;
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
    glm::vec3 center3 = glm::vec3(0, 0, 10);
    glm::vec3 corner_map = glm::vec3(-10, 0, -10);

    length = body_rad = 1;
    len_en = 2;
    float map_length = 40;

    // map
    Mesh* square1 = object2D::CreateSquare("square1", corner_map, map_length, glm::vec3(1, 1, 0.5), true);
    AddMeshToList(square1);


    // projectiles
    len_pr = 0.3;
    Mesh* square3 = object2D::CreateSquareCentered("square3", center2, len_pr, glm::vec3(0, 0, 0), true);
    AddMeshToList(square3);

    proj_cooldown = 0;


    // enemies
    Mesh* square4 = object2D::CreateSquareCentered("square4", center2, len_en, glm::vec3(1, 0, 0), true);
    AddMeshToList(square4);

    en_cooldown = 500;


    // healthbar
    Mesh* square5 = object2D::CreateSquare("square5", center3, length, glm::vec3(1, 0, 0.75), false);
    AddMeshToList(square5);

    Mesh* square6 = object2D::CreateSquare("square6", center3, length, glm::vec3(1, 0, 0.75), true);
    AddMeshToList(square6);

    x_hb = logicSpace.x - 7;
    y_hb = logicSpace.height - 2;
    health = 10;


    // score
    Mesh* square7 = object2D::CreateSquare("square7", center3, length, glm::vec3(0, 0, 0.75), false);
    AddMeshToList(square7);

    Mesh* square8 = object2D::CreateSquare("square8", center3, length, glm::vec3(0, 0, 0.75), true);
    AddMeshToList(square8);

    score = 0;
    x_sc = logicSpace.x - 7;
    y_sc = logicSpace.height - 4;


    // character

    // eyes
    Mesh* circle2 = object2D::CreateCircle("circle2", center2, length, glm::vec3(0, 1, 1));
    AddMeshToList(circle2);

    // body
    Mesh* circle1 = object2D::CreateCircle("circle1", corner, body_rad, glm::vec3(0.75, 1, 1));
    AddMeshToList(circle1);

    // tail
    Mesh* triangle = object2D::CreateTriangle("triangle", center1, length, glm::vec3(0.5, 1, 0.5));
    AddMeshToList(triangle);

    x_char = logicSpace.width / 2;
    y_char = logicSpace.height / 2;
    rad_char = 0;


    // healer bonus
    Mesh* circle3 = object2D::CreateCircle("circle3", corner, body_rad / 2, glm::vec3(0, 0.5, 0.2));
    AddMeshToList(circle3);

    heal_time = 500;
    x_heal = y_heal = 25;


    // obstacles
    Mesh* square2 = object2D::CreateSquare("square2", corner, length, glm::vec3(0.75, 0.75, 1), true);
    AddMeshToList(square2);

    // obstracle 1
    x_o.push_back(1);
    y_o.push_back(1);
    sx_o.push_back(1);
    sy_o.push_back(20);

    // obstracle 2
    x_o.push_back(10);
    y_o.push_back(2);
    sx_o.push_back(15);
    sy_o.push_back(2);

    // obstracle 3
    x_o.push_back(0);
    y_o.push_back(20);
    sx_o.push_back(15);
    sy_o.push_back(1);

    // obstracle 4
    x_o.push_back(-5);
    y_o.push_back(30);
    sx_o.push_back(30);
    sy_o.push_back(2);

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

    // Compute uniform 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);
    DrawScene(visMatrix);

    // projectile
    for (int i = 0; i < x_pr.size(); i++) {
        if (pr_status[i]) {
            if (x_pr[i] + deltaTimeSeconds * x_dir_pr[i] + len_pr / 2 <= logicSpace.width + 9.3 &&
                x_pr[i] + deltaTimeSeconds * x_dir_pr[i] - len_pr / 2 >= -9.3 &&
                ok_obstacle(x_pr[i] + deltaTimeSeconds * x_dir_pr[i], y_pr[i], len_pr / 2)) {
                x_pr[i] += deltaTimeSeconds * x_dir_pr[i];
                dist_x_pr[i] += deltaTimeSeconds * x_dir_pr[i];
            }
            else {
                pr_status[i] = false;
                cout << x_pr[i] << ' ' << y_pr[i] << endl;
            }

            if (y_pr[i] + deltaTimeSeconds * y_dir_pr[i] + len_pr / 2 <= viewSpace.height / logicSpace.height + 3.3 &&
                y_pr[i] + deltaTimeSeconds * y_dir_pr[i] - len_pr / 2 >= 0.7 &&
                ok_obstacle(x_pr[i], y_pr[i] + deltaTimeSeconds * y_dir_pr[i], len_pr / 2)) {
                    y_pr[i] += deltaTimeSeconds * y_dir_pr[i];
                    dist_y_pr[i] += deltaTimeSeconds * y_dir_pr[i];
            }
            else {
                pr_status[i] = false;
            }

            target_enemy(i);

            if (dist_x_pr[i] * dist_x_pr[i] + dist_y_pr[i] * dist_y_pr[i] >= MAX_PROJ_DIST) {
                pr_status[i] = false;
            }
        }
    }
    proj_cooldown--;

    // enemy
    en_cooldown--;
    for (int i = 0; i < x_en.size(); i++) {
        if (en_status[i]) {
            if (y_char > y_en[i]) {
                rad_en[i] = -atan((x_char - x_en[i]) / (y_char - y_en[i]));
            }
            else if (y_char < y_en[i]) {
                rad_en[i] = 3.1415 - atan((x_char - x_en[i]) / (y_char - y_en[i]));
            }
            else if (x_char > x_en[i]) {
                rad_en[i] = -3.1415 / 2;
            }
            else if (x_char < x_en[i]) {
                rad_en[i] = 3.1415 / 2;
            }
            x_en[i] += (x_char - x_en[i]) * deltaTimeSeconds * en_speed[i];
            y_en[i] += (y_char - y_en[i]) * deltaTimeSeconds * en_speed[i];

            if (x_char + 1 >= x_en[i] - 1 && x_char - 1 <= x_en[i] + 1 &&
                y_char + 1 >= y_en[i] - 1 && y_char - 1 <= y_en[i] + 1) {
                en_status[i] = false;
                health--;
            }
        }
    }
    if (en_cooldown <= 0) {
        int i = x_en.size();
        float r = ((float) rand() * (logicSpace.width + 15)) / RAND_MAX - 8;
        x_en.push_back(r);
        r = ((float)rand() * viewSpace.height / logicSpace.height) / RAND_MAX + 2;
        y_en.push_back(r);
        en_status.push_back(true);

        r = ((float)rand() * 2) / RAND_MAX;
        en_speed.push_back(r);
        
        if (y_char > y_en[i]) {
            rad_en.push_back(-atan((x_char - x_en[i]) / (y_char - y_en[i])));
        }
        else if (y_char < y_en[i]) {
            rad_en.push_back(3.1415 - atan((x_char - x_en[i]) / (y_char - y_en[i])));
        }
        else if (x_char > x_en[i]) {
            rad_en.push_back(-3.1415 / 2);
        }
        else if (x_char < x_en[i]) {
            rad_en.push_back(3.1415 / 2);
        }
        en_cooldown = 500;
    }

    // game status
    if (health <= 0) {
        cout << "YOU LOST!\n";
        score = 0;
        health = 10;
    }
    if (score >= 10) {
        cout << "YOU WON!\n";
        score = 0;
        health = 10;
    }

    // healer bonus
    heal_time--;
    if (heal_time < 0) {
        x_heal = ((float)rand() * (logicSpace.width + 15)) / RAND_MAX - 8;
        y_heal = ((float)rand() * viewSpace.height / logicSpace.height) / RAND_MAX + 2;
        heal_time = 500;
    }

    if (x_char + body_rad >= x_heal - body_rad / 2 && x_char - body_rad <= x_heal + body_rad / 2 &&
        y_char + body_rad >= y_heal - body_rad / 2 && y_char - body_rad <= y_heal + body_rad / 2) {
        health += 2;
        if (health > 10) {
            health = 10;
        }
        heal_time = 0;
    }

}


void Tema1::FrameEnd()
{
}


void Tema1::DrawScene(glm::mat3 visMatrix)
{
    // character

    // body
    modelMatrix = visMatrix * transform2D::Translate(x_char, y_char) * transform2D::Rotate(rad_char) * transform2D::Translate(0, 0);
    RenderMesh2D(meshes["circle1"], shaders["VertexColor"], modelMatrix);

    // eyes
    modelMatrix = visMatrix * transform2D::Translate(x_char, y_char) * transform2D::Rotate(rad_char) * transform2D::Translate(-0.3, 0.9) * transform2D::Scale(0.2, 0.2);
    RenderMesh2D(meshes["circle2"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(x_char, y_char) * transform2D::Rotate(rad_char) * transform2D::Translate(0.3, 0.9) * transform2D::Scale(0.2, 0.2);
    RenderMesh2D(meshes["circle2"], shaders["VertexColor"], modelMatrix);

    // tail
    modelMatrix = visMatrix * transform2D::Translate(x_char, y_char) * transform2D::Rotate(rad_char) * transform2D::Translate(-0.6, -1.2) * transform2D::Scale(1.2, 1.2);
    RenderMesh2D(meshes["triangle"], shaders["VertexColor"], modelMatrix);


    // map
    modelMatrix = visMatrix;
    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);


    // obstacles
    for (int i = 0; i < x_o.size(); i++) {
        modelMatrix = visMatrix * transform2D::Translate(x_o[i], y_o[i]) * transform2D::Scale(sx_o[i], sy_o[i]);
        RenderMesh2D(meshes["square2"], shaders["VertexColor"], modelMatrix);
    }


    // projectile
    for (int i = 0; i < x_pr.size(); i++) {
        if (pr_status[i]) {
            modelMatrix = visMatrix * transform2D::Translate(x_pr[i], y_pr[i]) * transform2D::Rotate(rad_pr[i]) * transform2D::Translate(0, 1);
            RenderMesh2D(meshes["square3"], shaders["VertexColor"], modelMatrix);
        }
    }


    // enemy
    for (int i = 0; i < x_en.size(); i++) {
        if (en_status[i]) {
            modelMatrix = visMatrix * transform2D::Translate(x_en[i], y_en[i]) * transform2D::Rotate(rad_en[i]) * transform2D::Translate(1, 1) * transform2D::Scale(0.15, 0.15);
            RenderMesh2D(meshes["square4"], shaders["VertexColor"], modelMatrix);

            modelMatrix = visMatrix * transform2D::Translate(x_en[i], y_en[i]) * transform2D::Rotate(rad_en[i]) * transform2D::Translate(-1, 1) * transform2D::Scale(0.15, 0.15);
            RenderMesh2D(meshes["square4"], shaders["VertexColor"], modelMatrix);

            modelMatrix = visMatrix * transform2D::Translate(x_en[i], y_en[i]) * transform2D::Rotate(rad_en[i]);
            RenderMesh2D(meshes["square4"], shaders["VertexColor"], modelMatrix);
        }
    }


    // healthbar
    modelMatrix = visMatrix * transform2D::Translate(x_hb, y_hb) * transform2D::Scale(10, 1.5);
    RenderMesh2D(meshes["square5"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(x_hb, y_hb) * transform2D::Scale(health, 1.5);
    RenderMesh2D(meshes["square6"], shaders["VertexColor"], modelMatrix);


    // score
    modelMatrix = visMatrix * transform2D::Translate(x_sc, y_sc) * transform2D::Scale(10, 1.5);
    RenderMesh2D(meshes["square7"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * transform2D::Translate(x_sc, y_sc) * transform2D::Scale(score, 1.5);
    RenderMesh2D(meshes["square8"], shaders["VertexColor"], modelMatrix);


    // healer bonus
    modelMatrix = visMatrix * transform2D::Translate(x_heal, y_heal);
    RenderMesh2D(meshes["circle3"], shaders["VertexColor"], modelMatrix);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    float speed = 10;
    if (window->KeyHold(GLFW_KEY_W)) {
        if (y_char + deltaTime * speed <=  viewSpace.height / logicSpace.height + 3 &&
            ok_obstacle(x_char, y_char + deltaTime * speed, body_rad)) {
            y_char += deltaTime * speed;
            y_hb += deltaTime * speed;
            y_sc += deltaTime * speed;
            logicSpace.y += deltaTime * speed;
        }
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        if (x_char - deltaTime * speed - body_rad >= -10 &&
            ok_obstacle(x_char - deltaTime * speed, y_char, body_rad)) {
            x_char -= deltaTime * speed;
            x_hb -= deltaTime * speed;
            x_sc -= deltaTime * speed;
            logicSpace.x -= deltaTime * speed;
        }
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        if (y_char - deltaTime * speed - body_rad >= 0 &&
            ok_obstacle(x_char, y_char - deltaTime * speed, body_rad)) {
            y_char -= deltaTime * speed;
            y_hb -= deltaTime * speed;
            y_sc -= deltaTime * speed;
            logicSpace.y -= deltaTime * speed;
        }
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        if (x_char + deltaTime * speed + body_rad <= logicSpace.width + 10 &&
            ok_obstacle(x_char + deltaTime * speed, y_char, body_rad)) {
            x_char += deltaTime * speed;
            x_hb += deltaTime * speed;
            x_sc += deltaTime * speed;
            logicSpace.x += deltaTime * speed;
        }
    }
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
    float x_mouse = logicSpace.x + ((float)mouseX) * logicSpace.width / viewSpace.width;
    float y_mouse = logicSpace.height + logicSpace.y - mouseY * logicSpace.height / viewSpace.height;
    if (y_mouse > y_char) {
        rad_char = -atan((x_mouse - x_char) / (y_mouse - y_char));
    }
    else if (y_mouse < y_char) {
        rad_char = 3.1415 - atan((x_mouse - x_char) / (y_mouse - y_char));
    }
    else if (x_mouse > x_char) {
        rad_char = -3.1415 / 2;
    }
    else if (x_mouse < x_char) {
        rad_char = 3.1415 / 2;
    }
    
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        if (proj_cooldown <= 0) {
            proj_cooldown = 100;

            float x_mouse = logicSpace.x + ((float)mouseX) * logicSpace.width / viewSpace.width;
            float y_mouse = logicSpace.height + logicSpace.y - mouseY * logicSpace.height / viewSpace.height;

            x_pr.push_back(x_char);
            y_pr.push_back(y_char);
            rad_pr.push_back(rad_char);
            x_dir_pr.push_back(x_mouse - x_char);
            y_dir_pr.push_back(y_mouse - y_char);
            pr_status.push_back(true);
            dist_x_pr.push_back(0);
            dist_y_pr.push_back(0);
        }
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}
