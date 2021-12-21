#include "lab_m1/tema2/tema2.h"
#include "lab_m1/lab4/transform3D.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <random>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    BuildMaze();

    health = HEALTH_MAX;
    health_cooldown = 0;
    rad_char = 0;
    x_char = -1;
    for (int i = 0; i < N_MAZE && x_char == -1; i++) {
        for (int j = 0; j < M_MAZE && x_char == -1; j++) {
            if (maze[i][j] == FREE) {
                x_char = i;
                y_char = 0;
                z_char = j;
            }
        }
    }

    renderCameraTarget = false;

    camera = new implemented::Camera2();
    // camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    camera->Set(glm::vec3(0, 1.75, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera->TranslateRight(1.25);
    // camera->TranslateUpward(-0.5);
    camera_type = THIRD_PERSON;
    facing = SOUTH;

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // TODO(student): After you implement the changing of the projection
    // parameters, remove hardcodings of these parameters
    right = 10;
    left = 0.01;
    bottom = 0.01;
    top = 10;
    fov = 40;
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, z_near, z_far);

    // Create a simple cube
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(1, -1,  1), glm::vec3(1, 0, 1), glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1, 0, 0), glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3(1,  1,  1), glm::vec3(0, 1, 0), glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1), glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3(1, -1, -1), glm::vec3(0, 1, 1), glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3(1,  1, -1), glm::vec3(0, 0, 1), glm::vec3(0.1, 0.5, 0.8)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };

        CreateMesh("cube", vertices, indices);
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("SkinShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader_Skin.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader_Skin.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("BlueShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader_Blue.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader_Blue.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("GreenShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader_Green.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader_Green.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("MazeShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader_Maze.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader_Maze.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("EnemyShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader_Enemy.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader_Enemy.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("EnemyExplodingShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader_EnemyExplosion.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader_EnemyExplosion.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("ProjectileShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader_Proj.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader_Proj.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // BuildMaze();
    /*
    health = HEALTH_MAX;
    health_cooldown = 0;
    rad_char = 0;
    x_char = -1;
    for (int i = 0; i < N_MAZE && x_char == -1; i++) {
        for (int j = 0; j < M_MAZE && x_char == -1; j++) {
            if (maze[i][j] == FREE) {
                x_char = i;
                y_char = 0;
                z_char = j;
            }
        }
    }
    */
    x_enemy = 0;
    y_enemy = 0;
    speed_enemy = 0.5;

    random = 0;
    random_increase = 0.5;
    cnt = 0;

    proj_cooldown = 0;
    show_proj = false;
}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // ========================================================================
    // This section demonstrates how the GPU vertex shader program
    // receives data.

    // TODO(student): If you look closely in the `Init()` and `Update()`
    // functions, you will see that we have three objects which we load
    // and use in three different ways:
    // - LoadMesh   + LabShader (this lab's shader)
    // - CreateMesh + VertexNormal (this shader is already implemented)
    // - CreateMesh + LabShader (this lab's shader)
    // To get an idea about how they're different from one another, do the
    // following experiments. What happens if you switch the color pipe and
    // normal pipe in this function (but not in the shader)? Now, what happens
    // if you do the same thing in the shader (but not in this function)?
    // Finally, what happens if you do the same thing in both places? Why?

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
    // ========================================================================

    // Unbind the VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    // character
    // head
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char, y_char, z_char));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 2.5, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        RenderMesh(meshes["box"], shaders["SkinShader"], modelMatrix);
    }
    // body
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char, y_char, z_char));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.75, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1, 0.5));
        RenderMesh(meshes["box"], shaders["GreenShader"], modelMatrix);
    }
    // shoulders
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char, y_char, z_char));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 2.15, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8f, 0.2, 0.2));
        RenderMesh(meshes["box"], shaders["GreenShader"], modelMatrix);
    }
    // arms
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char, y_char, z_char));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.4, 1.875, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.75, 0.2));
        RenderMesh(meshes["box"], shaders["GreenShader"], modelMatrix);
    }
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char, y_char, z_char));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.4, 1.875, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.75, 0.2));
        RenderMesh(meshes["box"], shaders["GreenShader"], modelMatrix);
    }
    // hands
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char, y_char, z_char));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.4, 1.375, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
        RenderMesh(meshes["box"], shaders["SkinShader"], modelMatrix);
    }
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char, y_char, z_char));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.4, 1.375, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
        RenderMesh(meshes["box"], shaders["SkinShader"], modelMatrix);
    }
    // legs
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char, y_char, z_char));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.15, 0.75, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 1, 0.25));
        RenderMesh(meshes["box"], shaders["BlueShader"], modelMatrix);
    }
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char, y_char, z_char));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.15, 0.75, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 1, 0.25));
        RenderMesh(meshes["box"], shaders["BlueShader"], modelMatrix);
    }
    // feet
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char, y_char, z_char));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.15, 0.125, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
        RenderMesh(meshes["box"], shaders["SkinShader"], modelMatrix);
    }
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char, y_char, z_char));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.15, 0.125, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
        RenderMesh(meshes["box"], shaders["SkinShader"], modelMatrix);
    }
    if (show_proj) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_proj, y_proj, z_proj));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
        // modelMatrix = glm::rotate(modelMatrix, RADIANS(rad_char), glm::vec3(0, 1, 0));
        // modelMatrix = glm::translate(modelMatrix, glm::vec3(0.15, 0.125, 0));
        // modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
        RenderMesh(meshes["sphere"], shaders["ProjectileShader"], modelMatrix);
        
    }

    //////////////////////////////////////////////////////////////////////

    // maze
    for (int i = 0; i < N_MAZE; i++) {
        for (int j = 0; j < N_MAZE; j++) {
            if (maze[i][j] == WALL) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0, j));
                // modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 1, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
                RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);
            }
            else if (maze[i][j] == ENEMY) {
                {
                    float x_center_enemy = -0.375 + x_enemy + i;
                    float y_center_enemy = -0.375 + y_enemy + j;

                    glm::mat4 modelMatrix = glm::mat4(1);
                    
                    // modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0, j));
                    // modelMatrix = glm::translate(modelMatrix, glm::vec3(x_enemy, 0, y_enemy));
                    // modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.375, 0, -0.375));
                    
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(x_center_enemy, 0, y_center_enemy));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25, 1, 0.25));
                    RenderMeshTest(meshes["box"], shaders["EnemyShader"], modelMatrix);

                    // cout << "enemy: " << x_center_enemy << ' ' << y_center_enemy << endl;
                    // cout << "char: " << x_char << ' ' << z_char << endl << endl;

                    if (!health_cooldown &&
                        x_center_enemy - 0.125 <= x_char + 0.125 &&
                        x_center_enemy + 0.125 >= x_char - 0.125 &&
                        y_center_enemy - 0.125 <= z_char + 0.125 &&
                        y_center_enemy + 0.125 >= z_char - 0.125) {
                        health--;
                        health_cooldown = HEALTH_COOLDOWN;
                    }
                }
            }
            else if (maze[i][j] == HIT) {
                if (!proj_cooldown) {
                    maze[i][j] = FREE;
                }
                else {
                    {
                        float x_center_enemy = -0.375 + x_enemy + i;
                        float y_center_enemy = -0.375 + y_enemy + j;

                        glm::mat4 modelMatrix = glm::mat4(1);
                        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_center_enemy, 0, y_center_enemy));
                        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25, 1, 0.25));

                        RenderMeshTest(meshes["box"], shaders["EnemyExplodingShader"], modelMatrix);
                    }
                }
            }
        }
    }

    // TODO : de corectat
    if (x_enemy <= 0 && y_enemy < 0.75) {
        y_enemy += deltaTimeSeconds * speed_enemy;
    }
    else if (y_enemy >= 0.75 && x_enemy < 0.75) {
        x_enemy += deltaTimeSeconds * speed_enemy;
    }
    else if (x_enemy >= 0.75 && y_enemy > 0) {
        y_enemy -= deltaTimeSeconds * speed_enemy;
    }
    else {
        x_enemy -= deltaTimeSeconds * speed_enemy;
    }

    if (health_cooldown > 0) {
        health_cooldown--;
    }

    /*
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
        // modelMatrix = glm::rotate(modelMatrix, RADIANS(45.0f), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        RenderSimpleMesh(meshes["cube"], shaders["VertexNormal"], modelMatrix);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(2, 0.5f, 0));
        // modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        RenderSimpleMesh(meshes["cube"], shaders["LabShader"], modelMatrix);
    }
    */

    // Render the camera target. This is useful for understanding where
    // the rotation point is, when moving in third-person camera mode.
    if (renderCameraTarget)
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    }

    if (projectionType == 'P') {
        projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, z_near, z_far);
    } else if (projectionType == 'O') {
        projectionMatrix = glm::ortho(left, right, bottom, top, z_near, z_far);
    }

    if (health == 0) {
        cout << "GAME OVER!\n";
        exit(0);
    }

    cnt--;
    if (cnt <= 0) {
        random += random_increase;
        if (random > 10) {
            random_increase = -0.5;
        }
        else if (random < 2) {
            random_increase = 0.5;
        }
        cnt = ENEMY_VANISH_SPEED;
    }

    if (proj_cooldown > 0) {
        proj_cooldown--;
        x_proj += x_dir * deltaTimeSeconds;
        y_proj += y_dir * deltaTimeSeconds;
        z_proj += z_dir * deltaTimeSeconds;
    }
    if (proj_cooldown == 0) {
        show_proj = false;
    }
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::RenderMeshTest(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);
    /*
    // TODO(student): Get shader location for uniform mat4 "Model"
    GLint model_loc = glGetUniformLocation(shader->GetProgramID(), "Model");

    // TODO(student): Set shader uniform "Model" to modelMatrix
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // TODO(student): Get shader location for uniform mat4 "View"
    GLint view_loc = glGetUniformLocation(shader->GetProgramID(), "View");

    // TODO(student): Set shader uniform "View" to viewMatrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // TODO(student): Get shader location for uniform mat4 "Projection"
    GLint proj_loc = glGetUniformLocation(shader->GetProgramID(), "Projection");

    // TODO(student): Set shader uniform "Projection" to projectionMatrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    */
    GLint t_loc = glGetUniformLocation(shader->GetProgramID(), "T");
    glUniform1f(t_loc, (GLfloat)Engine::GetElapsedTime());

    GLint health_loc = glGetUniformLocation(shader->GetProgramID(), "Health");
    glUniform1f(health_loc, (GLfloat)Tema2::health);

    GLint random_loc = glGetUniformLocation(shader->GetProgramID(), "Random");
    glUniform1f(random_loc, (GLfloat)Tema2::random);

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // TODO(student): Get shader location for uniform mat4 "Model"
    GLint model_loc = glGetUniformLocation(shader->GetProgramID(), "Model");

    // TODO(student): Set shader uniform "Model" to modelMatrix
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // TODO(student): Get shader location for uniform mat4 "View"
    GLint view_loc = glGetUniformLocation(shader->GetProgramID(), "View");

    // TODO(student): Set shader uniform "View" to viewMatrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // TODO(student): Get shader location for uniform mat4 "Projection"
    GLint proj_loc = glGetUniformLocation(shader->GetProgramID(), "Projection");

    // TODO(student): Set shader uniform "Projection" to projectionMatrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    GLint t_loc = glGetUniformLocation(shader->GetProgramID(), "T");
    glUniform1f(t_loc, (GLfloat)Engine::GetElapsedTime());

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

// using Aldous-Broder
void Tema2::BuildMaze() {
    auto rng = std::default_random_engine{};
    int state;
    vector<pair<int, int>> neighbors;
    int x = rand() % N_MAZE;
    int y = rand() % M_MAZE;
    if (x % 2 == 0) {
        x++;
        if (x >= N_MAZE) {
            x -= 2;
        }
    }
    if (y % 2 == 0) {
        y++;
        if (y >= M_MAZE) {
            y -= 2;
        }
    }

    int remaining = (N_MAZE / 2) * (M_MAZE / 2) - 1;

    maze[x][y] = FREE;

    while (remaining > 0) {
        neighbors.clear();
        state = WALL;

        if (x > 1 && maze[x - 2][y] == state) {
            neighbors.push_back(make_pair(x - 2, y));
        }
        if (x < N_MAZE - 2 && maze[x + 2][y] == state) {
            neighbors.push_back(make_pair(x + 2, y));
        }
        if (y > 1 && maze[x][y - 2] == state) {
            neighbors.push_back(make_pair(x, y - 2));
        }
        if (y < M_MAZE - 2 && maze[x][y + 2] == state) {
            neighbors.push_back(make_pair(x, y + 2));
        }
        std::shuffle(std::begin(neighbors), std::end(neighbors), rng);



        if (neighbors.size() == 0) {
            neighbors.clear();
            state = FREE;

            if (x > 1 && maze[x - 2][y] == state) {
                neighbors.push_back(make_pair(x - 2, y));
            }
            if (x < N_MAZE - 2 && maze[x + 2][y] == state) {
                neighbors.push_back(make_pair(x + 2, y));
            }
            if (y > 1 && maze[x][y - 2] == state) {
                neighbors.push_back(make_pair(x, y - 2));
            }
            if (y < M_MAZE - 2 && maze[x][y + 2] == state) {
                neighbors.push_back(make_pair(x, y + 2));
            }
            std::shuffle(std::begin(neighbors), std::end(neighbors), rng);



            int index = rand() % neighbors.size();
            x = neighbors[index].first;
            y = neighbors[index].second;
            continue;
        }

        for (int i = 0; i < neighbors.size(); i++) {
            int neigh_x = neighbors[i].first;
            int neigh_y = neighbors[i].second;
            if (maze[neigh_x][neigh_y] != FREE) {
                maze[(neigh_x + x) / 2][(neigh_y + y) / 2] = FREE;
                maze[neigh_x][neigh_y] = FREE;
                remaining--;
                x = neigh_x;
                y = neigh_y;
                break;
            }
        }
    }

    for (int i = 1; i <= numberOfEnemies; i++) {
        int enemy_x = rand() % N_MAZE;
        int enemy_y = rand() % M_MAZE;
        if (maze[enemy_x][enemy_y] == FREE && enemy_x > 1 && enemy_x != N_MAZE - 1 && enemy_y > 1 && enemy_y != M_MAZE - 1) {
            maze[enemy_x][enemy_y] = ENEMY;
        }
        else {
            i--;
        }
    }

    for (int j = 0; j < M_MAZE; j++) {
        if (maze[N_MAZE - 2][j] == FREE) {
            maze[N_MAZE - 1][j] = FREE;
            break;
        }
    }
}

bool Tema2::WallHit(float x, float y) {
    for (int i = 0; i < N_MAZE; i++) {
        for (int j = 0; j < M_MAZE; j++) {
            if (maze[i][j] == WALL &&
                i + 0.5 >= x - 0.125 && i - 0.5 <= x + 0.125 &&
                j - 0.5 <= y + 0.125 && j + 0.5 >= y - 0.125) {
                return true;
            }
        }
    }
    return false;
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float cameraSpeed = 1;
    // move the camera only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {

        if (window->KeyHold(GLFW_KEY_W)) {
            // TODO(student): Translate the camera forward
            camera->TranslateForward(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // TODO(student): Translate the camera to the left
            camera->TranslateRight(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // TODO(student): Translate the camera backward
            camera->TranslateForward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // TODO(student): Translate the camera to the right
            camera->TranslateRight(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            // TODO(student): Translate the camera downward
            camera->TranslateUpward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            // TODO(student): Translate the camera upward
            camera->TranslateUpward(cameraSpeed * deltaTime);
        }
    }

    // TODO(student): Change projection parameters. Declare any extra
    // variables you might need in the class header. Inspect this file
    // for any hardcoded projection arguments (can you find any?) and
    // replace them with those extra variables.
    if (window->KeyHold(GLFW_KEY_Y)) {
        // fov
        fov -= deltaTime * cameraSpeed;
    }
    if (window->KeyHold(GLFW_KEY_U)) {
        // fov
        fov += deltaTime * cameraSpeed;
    }
    if (window->KeyHold(GLFW_KEY_L)) {
        // right
        right -= deltaTime;
        left += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_K)) {
        // left
        right += deltaTime;
        left -= deltaTime;
    }

    // character movement
    if (window->KeyHold(GLFW_KEY_W) && !WallHit(x_char, z_char - deltaTime)) {
        z_char -= deltaTime;
        rad_char = 0;
        // TODO(student): Translate the camera forward
        // camera->TranslateForward(cameraSpeed * deltaTime);
        camera->MoveForward(cameraSpeed * deltaTime);
        facing = NORTH;
    }

    if (window->KeyHold(GLFW_KEY_A) && !WallHit(x_char - deltaTime, z_char)) {
        x_char -= deltaTime;
        rad_char = 90;
        // TODO(student): Translate the camera to the left
        camera->TranslateRight(-cameraSpeed * deltaTime);
        facing = WEST;
    }

    if (window->KeyHold(GLFW_KEY_S) && !WallHit(x_char, z_char + deltaTime)) {
        z_char += deltaTime;
        rad_char = 0;
        // TODO(student): Translate the camera backward
        // camera->TranslateForward(-cameraSpeed * deltaTime);
        camera->MoveForward(-cameraSpeed * deltaTime);
        facing = SOUTH;
    }

    if (window->KeyHold(GLFW_KEY_D) && !WallHit(x_char + deltaTime, z_char)) {
        x_char += deltaTime;
        rad_char = 90;
        // TODO(student): Translate the camera to the right
        camera->TranslateRight(cameraSpeed * deltaTime);
        facing = EAST;
    }


    if (window->KeyHold(GLFW_KEY_LEFT_CONTROL)) {
        if (camera_type == THIRD_PERSON) {
            third_person_position = camera->position;
            third_person_forward = camera->forward;
            third_person_distanceToTarget = camera->distanceToTarget;
            third_person_right = camera->right;
            third_person_up = camera->up;

            // camera->position = camera->GetTargetPosition();
            camera->position = glm::vec3(x_char, y_char + 0.5, z_char + 0.25);
            // camera->forward = glm::vec1(-1) * third_person_forward;
            if (facing == SOUTH) {
                /*
                camera->position = glm::vec3(x_char, y_char + 0.5, z_char + 0.25);
                camera->forward = glm::vec3(0, 0, 3.5);
                */
                camera->Set(glm::vec3(x_char, y_char + 0.5, z_char + 0.25), glm::vec3(x_char, y_char + 0.5, z_char + 4), glm::vec3(0, 1, 0));
            }
            else if (facing == NORTH) {
                /*
                camera->position = glm::vec3(x_char, y_char + 0.5, z_char - 0.25);
                camera->forward = glm::vec3(0, 0, -3.5);
                */
                camera->Set(glm::vec3(x_char, y_char + 0.5, z_char - 0.25), glm::vec3(x_char, y_char + 0.5, z_char - 4), glm::vec3(0, 1, 0));
            }
            else if (facing == EAST) {
                /*
                camera->position = glm::vec3(x_char + 0.25, y_char + 0.5, z_char);
                camera->forward = glm::vec3(0, 0, 3.5);
                camera->RotateFirstPerson_OY(90);
                */
                camera->Set(glm::vec3(x_char + 0.25, y_char + 0.5, z_char), glm::vec3(x_char + 4, y_char + 0.5, z_char), glm::vec3(0, 1, 0));
            }
            else if (facing == WEST) {
                camera->Set(glm::vec3(x_char - 0.25, y_char + 0.5, z_char), glm::vec3(x_char - 4, y_char + 0.5, z_char), glm::vec3(0, 1, 0));
            }

            camera_type = FIRST_PERSON;
        }
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }
    // TODO(student): Switch projections
    if (key == GLFW_KEY_P) {
        projectionType = 'P';
        //projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, z_near, z_far);
        //fov
    }
    if (key == GLFW_KEY_O) {
        projectionType = 'O';
        //projectionMatrix = glm::ortho(left, right, bottom, top, z_near, z_far);
        // left right
    }
    /*
    if (key == GLFW_KEY_SPACE) {
        if (camera_type == THIRD_PERSON) {
            third_person_position = camera->position;
            third_person_forward = camera->forward;
            third_person_distanceToTarget = camera->distanceToTarget;
            third_person_right = camera->right;
            third_person_up = camera->up;

            // camera->position = camera->GetTargetPosition();
            camera->position = glm::vec3(x_char, y_char + 0.5, z_char + 0.25);
            // camera->forward = glm::vec1(-1) * third_person_forward;
            if (facing == SOUTH) {
                // camera->position = glm::vec3(x_char, y_char + 0.5, z_char + 0.25);
                // camera->forward = glm::vec3(0, 0, 3.5);
                camera->Set(glm::vec3(x_char, y_char + 0.5, z_char + 0.25), glm::vec3(x_char, y_char + 0.5, z_char + 4), glm::vec3(0, 1, 0));
            }
            else if (facing == NORTH) {
                // camera->position = glm::vec3(x_char, y_char + 0.5, z_char - 0.25);
                // camera->forward = glm::vec3(0, 0, -3.5);
                camera->Set(glm::vec3(x_char, y_char + 0.5, z_char - 0.25), glm::vec3(x_char, y_char + 0.5, z_char - 4), glm::vec3(0, 1, 0));
            }
            else if (facing == EAST) {
                // camera->position = glm::vec3(x_char + 0.25, y_char + 0.5, z_char);
                // camera->forward = glm::vec3(0, 0, 3.5);
                // camera->RotateFirstPerson_OY(90);
                camera->Set(glm::vec3(x_char + 0.25, y_char + 0.5, z_char), glm::vec3(x_char + 4, y_char + 0.5, z_char), glm::vec3(0, 1, 0));
            }
            else if (facing == WEST) {
                camera->Set(glm::vec3(x_char - 0.25, y_char + 0.5, z_char), glm::vec3(x_char - 4, y_char + 0.5, z_char), glm::vec3(0, 1, 0));
            }
            
            camera_type = FIRST_PERSON;
        }
        else if (camera_type == FIRST_PERSON) {
            camera->position = third_person_position;
            camera->forward = third_person_forward;
            camera->up = third_person_up;
            camera->distanceToTarget = third_person_distanceToTarget;
            camera->right = third_person_right;

            camera_type = THIRD_PERSON;
        }
    }
    */
    if (window->KeyHold(GLFW_KEY_LEFT_CONTROL) && key == GLFW_KEY_SPACE) {
        if (proj_cooldown == 0) {
            proj_cooldown = PROJ_COOLDOWN;
            x_proj = x_char;
            y_proj = y_char + 0.5;
            z_proj = z_char + 0.25;
            show_proj = true;
        }
    }

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
    if (key == GLFW_KEY_LEFT_CONTROL) {
        if (camera_type == FIRST_PERSON) {
            camera->position = third_person_position;
            camera->forward = third_person_forward;
            camera->up = third_person_up;
            camera->distanceToTarget = third_person_distanceToTarget;
            camera->right = third_person_right;

            camera_type = THIRD_PERSON;
        }
    }
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    float sensivityOX = 0.001f;
    float sensivityOY = 0.001f;
    /*
    if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL && camera_type == THIRD_PERSON) {
        if (camera_type == THIRD_PERSON) {
            third_person_position = camera->position;
            third_person_forward = camera->forward;
            third_person_distanceToTarget = camera->distanceToTarget;
            third_person_right = camera->right;
            third_person_up = camera->up;

            // camera->position = camera->GetTargetPosition();
            camera->position = glm::vec3(x_char, y_char + 0.5, z_char + 0.25);
            // camera->forward = glm::vec1(-1) * third_person_forward;
            if (facing == SOUTH) {
                // camera->position = glm::vec3(x_char, y_char + 0.5, z_char + 0.25);
                // camera->forward = glm::vec3(0, 0, 3.5);
                camera->Set(glm::vec3(x_char, y_char + 0.5, z_char + 0.25), glm::vec3(x_char, y_char + 0.5, z_char + 4), glm::vec3(0, 1, 0));
            }
            else if (facing == NORTH) {
                // camera->position = glm::vec3(x_char, y_char + 0.5, z_char - 0.25);
                // camera->forward = glm::vec3(0, 0, -3.5);
                camera->Set(glm::vec3(x_char, y_char + 0.5, z_char - 0.25), glm::vec3(x_char, y_char + 0.5, z_char - 4), glm::vec3(0, 1, 0));
            }
            else if (facing == EAST) {
                // camera->position = glm::vec3(x_char + 0.25, y_char + 0.5, z_char);
                // camera->forward = glm::vec3(0, 0, 3.5);
                // camera->RotateFirstPerson_OY(90);
                camera->Set(glm::vec3(x_char + 0.25, y_char + 0.5, z_char), glm::vec3(x_char + 4, y_char + 0.5, z_char), glm::vec3(0, 1, 0));
            }
            else if (facing == WEST) {
                camera->Set(glm::vec3(x_char - 0.25, y_char + 0.5, z_char), glm::vec3(x_char - 4, y_char + 0.5, z_char), glm::vec3(0, 1, 0));
            }

            camera_type = FIRST_PERSON;
        }
        else if (camera_type == FIRST_PERSON) {
            camera->position = third_person_position;
            camera->forward = third_person_forward;
            camera->up = third_person_up;
            camera->distanceToTarget = third_person_distanceToTarget;
            camera->right = third_person_right;

            camera_type = THIRD_PERSON;
        }
    }
    */
    if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL && camera_type == FIRST_PERSON) {
        renderCameraTarget = false;
        // TODO(student): Rotate the camera in first-person mode around
        // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
        // variables for setting up the rotation speed.
        camera->RotateFirstPerson_OX(-sensivityOX * deltaY);
        camera->RotateFirstPerson_OY(-sensivityOY * deltaX);
        if (!proj_cooldown) {
            x_dir = camera->forward.x;
            y_dir = camera->forward.y;
            z_dir = camera->forward.z;
        }
    }
    
    /*
    if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
        renderCameraTarget = true;
        // TODO(student): Rotate the camera in third-person mode around
        // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
        // variables for setting up the rotation speed.
        camera->RotateThirdPerson_OX(-sensivityOX * deltaY);
        camera->RotateThirdPerson_OY(-sensivityOY * deltaX);
    }
    */
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
