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
    // build maze
    BuildMaze();

    // initial health state
    health = HEALTH_MAX;
    health_cooldown = 0;

    // place the character
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

    // set third person camera
    camera = new implemented::Camera2();
    camera->Set(glm::vec3(0, 1.75, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera->TranslateRight(1.25);
    camera_type = THIRD_PERSON;
    facing = SOUTH;

    // shapes
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
    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // projection matrix
    right = 10;
    left = 0;
    bottom = 0;
    top = 10;
    fov = 50;
    projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, z_near, z_far);
    orthoProjectionMatrix = glm::ortho(left, right, bottom, top, z_near, z_far);

    // shaders
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
    {
        Shader* shader = new Shader("FloorShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader_Floor.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader_Floor.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // enemy
    x_enemy = 0;
    y_enemy = 0;
    speed_enemy = 1;

    random = 0;
    random_increase = 0.5;
    cnt = 0;

    // projectile
    proj_cooldown = 0;
    show_proj = false;

    // timer
    time = TIME_MAX;

    // health bonus
    bonus_cooldown = BONUS_COOLDOWN;
    x_bonus = y_bonus = z_bonus = 0;
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
    // floor
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(N_MAZE / 2.0 - 0.5, -0.5, M_MAZE / 2.0 - 0.5));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(N_MAZE * 1.0 / 50, 1, M_MAZE * 1.0 / 50));
        RenderMesh(meshes["plane"], shaders["FloorShader"], modelMatrix);
    }


    // HUD

    // health
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char + 1.6, y_char + 9.8, z_char - 2));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.5, 0.5, 2));
        RenderMeshOrtho(meshes["box"], shaders["BlueShader"], modelMatrix);
    }
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char + 0.4 + 2.4 / HEALTH_MAX * health / 2, y_char + 9.8, z_char - 1.9));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.4 / HEALTH_MAX * health, 0.4, 2));
        RenderMeshOrtho(meshes["box"], shaders["GreenShader"], modelMatrix);
    }

    // time
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char + 8.8, y_char + 9.8, z_char - 2));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.5, 0.5, 2));
        RenderMeshOrtho(meshes["box"], shaders["GreenShader"], modelMatrix);
    }
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_char + 7.6 + 2.4 / TIME_MAX * time / 2, y_char + 9.8, z_char - 1.9));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.4 / TIME_MAX * time, 0.4, 2));
        RenderMeshOrtho(meshes["box"], shaders["BlueShader"], modelMatrix);
    }


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

    // projectile
    if (show_proj) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_proj, y_proj, z_proj));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
        RenderMesh(meshes["sphere"], shaders["ProjectileShader"], modelMatrix);
        
    }


    // maze
    for (int i = 0; i < N_MAZE; i++) {
        for (int j = 0; j < N_MAZE; j++) {
            if (maze[i][j] == WALL) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0, j));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
                RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);
            }
            else if (maze[i][j] == ENEMY) {
                {
                    float x_center_enemy = -ENEMY_SIZE / 2 + x_enemy + i;
                    float y_center_enemy = -ENEMY_SIZE / 2 + y_enemy + j;

                    glm::mat4 modelMatrix = glm::mat4(1);                    
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(x_center_enemy, 0, y_center_enemy));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(ENEMY_SIZE, 1, ENEMY_SIZE));
                    RenderMeshTest(meshes["box"], shaders["EnemyShader"], modelMatrix);

                    if (!health_cooldown &&
                        x_center_enemy - ENEMY_SIZE / 2 <= x_char + 0.125 &&
                        x_center_enemy + ENEMY_SIZE / 2 >= x_char - 0.125 &&
                        y_center_enemy - ENEMY_SIZE / 2 <= z_char + 0.125 &&
                        y_center_enemy + ENEMY_SIZE / 2 >= z_char - 0.125) {
                        health--;
                        health_cooldown = HEALTH_COOLDOWN;
                    }

                    if (show_proj &&
                        y_proj >= -0.5 && y_proj <= 0.5 &&
                        x_center_enemy - ENEMY_SIZE / 2 <= x_proj &&
                        x_center_enemy + ENEMY_SIZE / 2 >= x_proj &&
                        y_center_enemy - ENEMY_SIZE / 2 <= z_proj &&
                        y_center_enemy + ENEMY_SIZE / 2 >= z_proj) {
                        maze[i][j] = HIT;
                        explosion_done = EXPLOSION;
                        show_proj = false;
                    }
                }
            }
            else if (maze[i][j] == HIT) {
                if (explosion_done <= 0) {
                    maze[i][j] = FREE;
                }
                else {
                    {
                        float x_center_enemy = -ENEMY_SIZE / 2 + x_enemy + i;
                        float y_center_enemy = -ENEMY_SIZE / 2 + y_enemy + j;

                        glm::mat4 modelMatrix = glm::mat4(1);
                        modelMatrix = glm::translate(modelMatrix, glm::vec3(x_center_enemy, 0, y_center_enemy));
                        modelMatrix = glm::scale(modelMatrix, glm::vec3(ENEMY_SIZE, 1, ENEMY_SIZE));

                        RenderMeshTest(meshes["box"], shaders["EnemyExplodingShader"], modelMatrix);
                    }
                    explosion_done--;
                }
            }
            else if (maze[i][j] == BONUS) {
                if (i - 0.2 / 2 <= x_char + 0.125 &&
                    i + 0.2 / 2 >= x_char - 0.125 &&
                    j - 0.2 / 2 <= z_char + 0.125 &&
                    j + 0.2 / 2 >= z_char - 0.125) {
                    if (health < HEALTH_MAX) {
                        health++;
                    }
                    bonus_cooldown = 0;
                    maze[i][j] = FREE;
                }

                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0, j));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
                RenderMesh(meshes["sphere"], shaders["EnemyShader"], modelMatrix);
            }
        }
    }

    // enemy movement inside a cell
    if (x_enemy <= 0 && y_enemy < 1 - ENEMY_SIZE) {
        y_enemy += deltaTimeSeconds * speed_enemy;
    }
    else if (y_enemy >= 1 - ENEMY_SIZE && x_enemy < 1 - ENEMY_SIZE) {
        x_enemy += deltaTimeSeconds * speed_enemy;
    }
    else if (x_enemy >= 1 - ENEMY_SIZE && y_enemy > 0) {
        y_enemy -= deltaTimeSeconds * speed_enemy;
    }
    else {
        x_enemy -= deltaTimeSeconds * speed_enemy;
    }

    // health update
    if (health_cooldown > 0) {
        health_cooldown--;
    }

    // game status
    if (health == 0 || time == 0) {
        cout << "GAME OVER!\n";
        exit(0);
    }

    if (x_char + 0.5 >= N_MAZE ||  z_char + 0.5 >= M_MAZE) {
        cout << "YOU WON!\n";
        exit(0);
    }

    time--;

    // enemy "explosion"
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

    // projectile movement
    if (proj_cooldown > 0) {
        proj_cooldown--;
        x_proj += x_dir * deltaTimeSeconds;
        y_proj += y_dir * deltaTimeSeconds;
        z_proj += z_dir * deltaTimeSeconds;
        if (WallHit(x_proj, z_proj) && y_proj >= -0.5 && y_proj <= 0.5) {
            show_proj = false;
        }
    }

    if (proj_cooldown == 0) {
        show_proj = false;
    }

    // health bonus
    if (bonus_cooldown <= 0) {
        if (x_bonus) {
            maze[x_bonus][z_bonus] = FREE;
        }
        bonus_cooldown = BONUS_COOLDOWN;
        x_bonus = rand() % N_MAZE;
        z_bonus = rand() % M_MAZE;
        while (!(maze[x_bonus][z_bonus] == FREE && x_bonus > 1 && x_bonus != N_MAZE - 1 && z_bonus > 1 && z_bonus != M_MAZE - 1)) {
            x_bonus = rand() % N_MAZE;
            z_bonus = rand() % M_MAZE;
        }
        maze[x_bonus][z_bonus] = BONUS;
    }

    bonus_cooldown--;
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

void Tema2::RenderMeshOrtho(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(orthoProjectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::RenderMeshTest(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);
    
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

    // Get shader location for uniform mat4 "Model"
    GLint model_loc = glGetUniformLocation(shader->GetProgramID(), "Model");

    // Set shader uniform "Model" to modelMatrix
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Get shader location for uniform mat4 "View"
    GLint view_loc = glGetUniformLocation(shader->GetProgramID(), "View");

    // Set shader uniform "View" to viewMatrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Get shader location for uniform mat4 "Projection"
    GLint proj_loc = glGetUniformLocation(shader->GetProgramID(), "Projection");

    // Set shader uniform "Projection" to projectionMatrix
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

    // place enemies
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

    // place exit
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

    // character movement
    if (window->KeyHold(GLFW_KEY_W) && !WallHit(x_char, z_char - deltaTime)) {
        z_char -= deltaTime;
        rad_char = 0;
        // Translate the camera forward
        camera->MoveForward(cameraSpeed * deltaTime);
        facing = NORTH;
    }

    if (window->KeyHold(GLFW_KEY_A) && !WallHit(x_char - deltaTime, z_char)) {
        x_char -= deltaTime;
        rad_char = 90;
        // Translate the camera to the left
        camera->TranslateRight(-cameraSpeed * deltaTime);
        facing = WEST;
    }

    if (window->KeyHold(GLFW_KEY_S) && !WallHit(x_char, z_char + deltaTime)) {
        z_char += deltaTime;
        rad_char = 0;
        // Translate the camera backward
        camera->MoveForward(-cameraSpeed * deltaTime);
        facing = SOUTH;
    }

    if (window->KeyHold(GLFW_KEY_D) && !WallHit(x_char + deltaTime, z_char)) {
        x_char += deltaTime;
        rad_char = 90;
        // Translate the camera to the right
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

            camera->position = glm::vec3(x_char, y_char + 0.5, z_char + 0.25);

            if (facing == SOUTH) {
                camera->Set(glm::vec3(x_char, y_char + 0.5, z_char + 0.25), glm::vec3(x_char, y_char + 0.5, z_char + 4), glm::vec3(0, 1, 0));
            }
            else if (facing == NORTH) {
                camera->Set(glm::vec3(x_char, y_char + 0.5, z_char - 0.25), glm::vec3(x_char, y_char + 0.5, z_char - 4), glm::vec3(0, 1, 0));
            }
            else if (facing == EAST) {
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
    if (window->KeyHold(GLFW_KEY_LEFT_CONTROL) && key == GLFW_KEY_SPACE) {
        if (proj_cooldown == 0) {
            proj_cooldown = PROJ_COOLDOWN;
            x_proj = x_char;
            y_proj = y_char + 0.5;
            z_proj = z_char;
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
    
    if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL && camera_type == FIRST_PERSON) {

        camera->RotateFirstPerson_OX(-sensivityOX * deltaY);
        camera->RotateFirstPerson_OY(-sensivityOY * deltaX);
        if (!proj_cooldown) {
            x_dir = camera->forward.x;
            y_dir = camera->forward.y;
            z_dir = camera->forward.z;
        }
    }
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
