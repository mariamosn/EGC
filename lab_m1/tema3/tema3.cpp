#include "lab_m1/tema3/tema3.h"
#include "lab_m1/lab3/object2D.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema3::Tema3()
{
}


Tema3::~Tema3()
{
}


void Tema3::Init()
{
    // Set camera
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(5, 2, 12));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();

    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "textures");

    // Load textures
    {
        mapTextures["random"] = CreateRandomTexture(16, 16);
    }

    // Load meshes
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
    {
        Mesh* mesh = object2D::CreateCone("cone", glm::vec3(0.0f), 1.0f, 1.0f, glm::vec3(1.0f));
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("bunny");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "animals"), "bunny.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("pair");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "characters", "pair"), "pair.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Load shaders
    {
        Shader *shader = new Shader("DiscoBallShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader_Disco.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader_Disco.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("Floor");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader_Floor.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader_Floor.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("Spotlight");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader_Spotlight.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader_Spotlight.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("NineLights");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader_9Lights.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader_9Lights.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Light & material properties
    materialShininess = 30;
    materialKd = 1;
    materialKs = 0.25;

    state = 0;
    spotlights_mode = ON;
    floorlights_mode = ON;
    discolights_mode = ON;

    disco_ball = glm::vec3(5, 3, 5);

    character = BOX;

    jump_cooldown = JUMP_COOLDOWN;
    jumping_dancer = -1;

    Generate_Floor();
    Generate_Dancers();
    Generate_Spotlights();
}

void Tema3::Generate_Floor() {
    for (int i = 0; i <= FLOOR_SIZE + 1; i++) {
        for (int j = 0; j <= FLOOR_SIZE + 1; j++) {
            float r, g, b;
            if (!i || !j || i == FLOOR_SIZE + 1 || j == FLOOR_SIZE + 1) {
                r = g = b = -1;
            }
            else {
                r = (float)(rand() % 256) / 255;
                g = (float)(rand() % 256) / 255;
                b = (float)(rand() % 256) / 255;
            }
            floor[i][j] = glm::vec3(r, g, b);
        }
    }
}

void Tema3::Generate_Dancers() {
    for (int i = 0; i < DANCERS; i++) {
        int a, b;
        a = 1 + rand() % FLOOR_SIZE;
        b = 1 + rand() % FLOOR_SIZE;
        bool ok = true;
        for (int j = 0; j < i && ok; j++) {
            if (x_dancers[j] == a && z_dancers[j] == b) {
                ok = false;
                i--;
            }
        }
        if (ok) {
            x_dancers[i] = a;
            z_dancers[i] = b;
        }
    }
}

void Tema3::GetNewSpotDir(int i, float deltaTimeSeconds) {
    int aux;
    
    aux = rand() % 3;
    if (aux == 0) {
        spot_angle_x_step[i] = -deltaTimeSeconds;
    }
    else if (aux == 0) {
        spot_angle_x_step[i] = 0;
    }
    else {
        spot_angle_x_step[i] = deltaTimeSeconds;
    }

    aux = rand() % 3;
    if (aux == 0) {
        spot_angle_z_step[i] = -deltaTimeSeconds;
    }
    else if (aux == 0) {
        spot_angle_z_step[i] = 0;
    }
    else {
        spot_angle_z_step[i] = deltaTimeSeconds;
    }
}

void Tema3::Generate_Spotlights() {
    for (int i = 0; i < SPOTLIGHTS; i++) {
        int a, b;
        a = 2 + rand() % (FLOOR_SIZE - 1);
        b = 2 + rand() % (FLOOR_SIZE - 1);
        bool ok = true;
        for (int j = 0; j < i && ok; j++) {
            if (spot_pos[j] == glm::vec3(a, SPOT_HEIGHT, b)) {
                ok = false;
                i--;
            }
        }
        if (ok) {
            spot_pos[i] = glm::vec3(a, SPOT_HEIGHT, b);
            float r, g, b;
            r = (float)(rand() % 256) / 255;
            g = (float)(rand() % 256) / 255;
            b = (float)(rand() % 256) / 255;
            spot_colors[i] = glm::vec3(r, g, b);
            spot_dir[i] = glm::vec3(0, -1, 0);
            spot_angle_x[i] = 0;
            spot_angle_z[i] = 0;
        }
    }
}


void Tema3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema3::Update(float deltaTimeSeconds)
{
    // update the direction of the spotlights
    UpdateSpotDir(deltaTimeSeconds);

    // floor
    for (int i = 1; i <= FLOOR_SIZE; i++) {
        for (int j = 1; j <= FLOOR_SIZE; j++) {
            // floor tile
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.5, 0, j + 0.5));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 0.01, 1));
                RenderSimpleMesh_Floor(meshes["box"], shaders["Floor"], modelMatrix, floor[i][j]);
            }
        }
    }

    // walls
    for (int i = 1; i <= FLOOR_SIZE; i++) {
        // left wall
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 2, i + 0.5));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 4, 1));

            int a = 1, b = i;
            RenderMeshCentered(a, b, modelMatrix, meshes["box"]);
        }
        // right wall
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(FLOOR_SIZE + 1, 2, i + 0.5));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 4, 1));
            
            int a = FLOOR_SIZE, b = i;
            RenderMeshCentered(a, b, modelMatrix, meshes["box"]);
        }
        // backside wall
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.5, 2, 1));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 4, 0.1));

            int a = i, b = 1;
            RenderMeshCentered(a, b, modelMatrix, meshes["box"]);
        }
    }

    // ceiling
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(1 + FLOOR_SIZE / 2, SPOT_HEIGHT, 1 + FLOOR_SIZE / 2));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(FLOOR_SIZE, 0.01, FLOOR_SIZE));
        RenderSimpleMesh_Floor(meshes["box"], shaders["Floor"], modelMatrix, glm::vec3(0));
    }

    // dancers
    if (jump_cooldown <= 0) {
        jumping_dancer = rand() % DANCERS;
        jump_cooldown = JUMP_COOLDOWN;
        jump_height = 0;
        jump_state = UP;
    }
    for (int i = 0; i < DANCERS; i++) {
        {
            int a = x_dancers[i] + 0.5, b = z_dancers[i] + 0.5;
            glm::mat4 modelMatrix = glm::mat4(1);

            if (i == jumping_dancer) {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0, jump_height, 0));
            }

            modelMatrix = glm::translate(modelMatrix, glm::vec3(x_dancers[i] + 0.5, 0.5, z_dancers[i] + 0.5));

            if (character == BOX) {
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5, 1, 0.5));
                RenderMeshCentered(a, b, modelMatrix, meshes["box"]);
            }
            else if (character == BUNNY) {
                modelMatrix = glm::rotate(modelMatrix, RADIANS(i * (360 / DANCERS) % 360), glm::vec3(0, 1, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.03, 0.03, 0.03));
                RenderMeshCentered(a, b, modelMatrix, meshes["bunny"]);
            }
            else if (character == PAIR) {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.5, 0));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(i * (360 / DANCERS) % 360), glm::vec3(0, 1, 0));
                modelMatrix = glm::rotate(modelMatrix, RADIANS(-90), glm::vec3(1, 0, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2, 0.2, 0.2));
                RenderMeshCentered(a, b, modelMatrix, meshes["pair"]);
            }
        }

        MoveDancer(i);
    }

    // disco ball
    if (discolights_mode) {
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, disco_ball);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
            RenderSimpleMesh_DiscoBall(meshes["sphere"], shaders["DiscoBallShader"], modelMatrix, mapTextures["random"]);
        }
    }

    // spotlights
    if (spotlights_mode) {
        for (int i = 0; i < SPOTLIGHTS; i++)
        {
            DrawSpotlight(i);
        }
    }

    // jumping state update
    UpdateJump(deltaTimeSeconds);
}


void Tema3::FrameEnd()
{
    DrawCoordinateSystem();
}

void Tema3::UpdateSpotDir(float deltaTimeSeconds) {
    for (int i = 0; i < SPOTLIGHTS; i++) {
        while ((double)spot_angle_x[i] + (double)spot_angle_x_step[i] * SPOT_SPEED <= SPOT_ANGLE_LIMIT_MIN ||
            (double)spot_angle_x[i] + (double)spot_angle_x_step[i] * SPOT_SPEED >= SPOT_ANGLE_LIMIT_MAX ||
            (double)spot_angle_z[i] + (double)spot_angle_z_step[i] * SPOT_SPEED <= SPOT_ANGLE_LIMIT_MIN ||
            (double)spot_angle_z[i] + (double)spot_angle_z_step[i] * SPOT_SPEED >= SPOT_ANGLE_LIMIT_MAX) {
            GetNewSpotDir(i, deltaTimeSeconds);
        }

        spot_angle_x[i] += spot_angle_x_step[i] * SPOT_SPEED;
        spot_angle_z[i] += spot_angle_z_step[i] * SPOT_SPEED;

        glm::mat4 rot = glm::mat4(1);
        rot = glm::rotate(rot, RADIANS(spot_angle_x[i]), glm::vec3(1, 0, 0));
        rot = glm::rotate(rot, RADIANS(spot_angle_z[i]), glm::vec3(0, 0, 1));
        spot_dir[i] = glm::vec3(0, -1, 0);
        spot_dir[i] = glm::vec3(rot * glm::vec4(spot_dir[i], 0));
    }
}

void Tema3::MoveDancer(int i) {
    if (x_dir_dancers[i] < -1 || x_dir_dancers[i] > 1 ||
        x_dancers[i] + x_dir_dancers[i] < 1 || x_dancers[i] + x_dir_dancers[i] > FLOOR_SIZE ||
        z_dir_dancers[i] < -1 || z_dir_dancers[i] > 1 ||
        z_dancers[i] + z_dir_dancers[i] < 1 || z_dancers[i] + z_dir_dancers[i] > FLOOR_SIZE) {
        // change direction
        int x_movement = rand() % 2;
        if (x_movement == 0) {
            x_dir_dancers[i] = -(float)(rand() % 1000) / 999;
        }
        else {
            x_dir_dancers[i] = (float)(rand() % 1000) / 999;
        }

        int z_movement = rand() % 2;
        if (z_movement == 0) {
            z_dir_dancers[i] = -(float)(rand() % 1000) / 999;
        }
        else {
            z_dir_dancers[i] = (float)(rand() % 1000) / 999;
        }
    }
    else {
        // keep direction
        x_dancers[i] += x_dir_dancers[i] * dancers_speed;
        z_dancers[i] += z_dir_dancers[i] * dancers_speed;
    }
}

void Tema3::DrawSpotlight(int i) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, spot_pos[i]);
    modelMatrix = glm::rotate(modelMatrix, RADIANS(spot_angle_x[i]), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(spot_angle_z[i]), glm::vec3(0, 0, 1));
    float angle = SPOTLIGHT_ANGLE * 3.1415 / 180;
    modelMatrix = glm::scale(modelMatrix, glm::vec3(tan(angle) * (SPOT_HEIGHT + SPOT_EXTRA_HEIGHT), 1 * (SPOT_HEIGHT + SPOT_EXTRA_HEIGHT), tan(angle) * (SPOT_HEIGHT + SPOT_EXTRA_HEIGHT)));

    // draw only the front part
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // don't write in the depth buffer
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw cone
    RenderSimpleMesh_Floor(meshes["cone"], shaders["Spotlight"], modelMatrix, spot_colors[i]);

    // undo changes
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
}

void Tema3::UpdateJump(float deltaTimeSeconds) {
    if (jump_cooldown > 0) {
        jump_cooldown--;
    }

    if (jump_state == UP) {
        if (jump_height + deltaTimeSeconds <= JUMP_H) {
            jump_height += deltaTimeSeconds;
        }
        else {
            jump_state = DOWN;
        }
    }
    else if (jump_state == DOWN) {
        if (jump_height - deltaTimeSeconds >= 0) {
            jump_height -= deltaTimeSeconds;
        }
        else {
            jumping_dancer = -1;
        }
    }
}

void Tema3::RenderSimpleMesh_Floor(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

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

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    GLuint spot_pos_loc = glGetUniformLocation(shader->program, "spotLightPos");
    glUniform3fv(spot_pos_loc, SPOTLIGHTS, glm::value_ptr(spot_pos[0]));

    GLuint spot_col_loc = glGetUniformLocation(shader->program, "spotLightCol");
    glUniform3fv(spot_col_loc, SPOTLIGHTS, glm::value_ptr(spot_colors[0]));

    GLuint spot_dir_loc = glGetUniformLocation(shader->program, "spotLightDir");
    glUniform3fv(spot_dir_loc, SPOTLIGHTS, glm::value_ptr(spot_dir[0]));

    int spot_loc = glGetUniformLocation(shader->program, "spot_mode");
    glUniform1f(spot_loc, spotlights_mode);

    int floor_loc = glGetUniformLocation(shader->program, "floor_mode");
    glUniform1f(floor_loc, floorlights_mode);

    int disco_loc = glGetUniformLocation(shader->program, "disco_mode");
    glUniform1f(disco_loc, discolights_mode);

    GLuint disco_pos_loc = glGetUniformLocation(shader->program, "disco_pos");
    glUniform3f(disco_pos_loc, disco_ball.x, disco_ball.y, disco_ball.z);

    GLint loc_time = glGetUniformLocation(shader->program, "time");
    glUniform1f(loc_time, (GLfloat)Engine::GetElapsedTime());

    // - activate texture location 0
    glActiveTexture(GL_TEXTURE0);
    // - bind the texture ID
    glBindTexture(GL_TEXTURE_2D, mapTextures["random"]->GetTextureID());
    // - send theuniform value
    glUniform1i(glGetUniformLocation(shader->program, "gen_texture"), 0);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderMeshCentered(int a, int b, const glm::mat4& modelMatrix, Mesh* mesh) {
    RenderSimpleMesh_9Lights(mesh, shaders["NineLights"], modelMatrix,
        floor[a][b], glm::vec3(a + 0.5, 0, b + 0.5),
        floor[a][b + 1], glm::vec3(a + 0.5, 0, b + 1 + 0.5),
        floor[a][b - 1], glm::vec3(a + 0.5, 0, b - 1 + 0.5),
        floor[a + 1][b], glm::vec3(a + 1 + 0.5, 0, b + 0.5),
        floor[a + 1][b + 1], glm::vec3(a + 1 + 0.5, 0, b + 1 + 0.5),
        floor[a + 1][b - 1], glm::vec3(a + 1 + 0.5, 0, b - 1 + 0.5),
        floor[a - 1][b], glm::vec3(a - 1 + 0.5, 0, b + 0.5),
        floor[a - 1][b + 1], glm::vec3(a - 1 + 0.5, 0, b + 1 + 0.5),
        floor[a - 1][b - 1], glm::vec3(a - 1 + 0.5, 0, b - 1 + 0.5)
    );
}

void Tema3::RenderSimpleMesh_9Lights(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix,
                                        const glm::vec3& color1, const glm::vec3& lightPosition1,
                                        const glm::vec3& color2, const glm::vec3& lightPosition2,
                                        const glm::vec3& color3, const glm::vec3& lightPosition3,
                                        const glm::vec3& color4, const glm::vec3& lightPosition4,
                                        const glm::vec3& color5, const glm::vec3& lightPosition5,
                                        const glm::vec3& color6, const glm::vec3& lightPosition6,
                                        const glm::vec3& color7, const glm::vec3& lightPosition7,
                                        const glm::vec3& color8, const glm::vec3& lightPosition8,
                                        const glm::vec3& color9, const glm::vec3& lightPosition9
)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light properties

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

    glm::vec3 light_pos[9];
    light_pos[0] = lightPosition1;
    light_pos[1] = lightPosition2;
    light_pos[2] = lightPosition3;
    light_pos[3] = lightPosition4;
    light_pos[4] = lightPosition5;
    light_pos[5] = lightPosition6;
    light_pos[6] = lightPosition7;
    light_pos[7] = lightPosition8;
    light_pos[8] = lightPosition9;

    GLuint light_pos_loc = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(light_pos_loc, 9, glm::value_ptr(light_pos[0]));

    glm::vec3 obj_col[9];
    obj_col[0] = color1;
    obj_col[1] = color2;
    obj_col[2] = color3;
    obj_col[3] = color4;
    obj_col[4] = color5;
    obj_col[5] = color6;
    obj_col[6] = color7;
    obj_col[7] = color8;
    obj_col[8] = color9;

    GLuint light_col_loc = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(light_col_loc, 9, glm::value_ptr(obj_col[0]));

    GLuint spot_pos_loc = glGetUniformLocation(shader->program, "spotLightPos");
    glUniform3fv(spot_pos_loc, SPOTLIGHTS, glm::value_ptr(spot_pos[0]));

    GLuint spot_col_loc = glGetUniformLocation(shader->program, "spotLightCol");
    glUniform3fv(spot_col_loc, SPOTLIGHTS, glm::value_ptr(spot_colors[0]));

    GLuint spot_dir_loc = glGetUniformLocation(shader->program, "spotLightDir");
    glUniform3fv(spot_dir_loc, SPOTLIGHTS, glm::value_ptr(spot_dir[0]));

    int spot_loc = glGetUniformLocation(shader->program, "spot_mode");
    glUniform1f(spot_loc, spotlights_mode);

    int floor_loc = glGetUniformLocation(shader->program, "floor_mode");
    glUniform1f(floor_loc, floorlights_mode);

    int disco_loc = glGetUniformLocation(shader->program, "disco_mode");
    glUniform1f(disco_loc, discolights_mode);

    GLuint disco_pos_loc = glGetUniformLocation(shader->program, "disco_pos");
    glUniform3f(disco_pos_loc, disco_ball.x, disco_ball.y, disco_ball.z);

    GLint loc_time = glGetUniformLocation(shader->program, "time");
    glUniform1f(loc_time, (GLfloat)Engine::GetElapsedTime());

    // - activate texture location 0
    glActiveTexture(GL_TEXTURE0);
    // - bind the texture ID
    glBindTexture(GL_TEXTURE_2D, mapTextures["random"]->GetTextureID());
    // - send theuniform value
    glUniform1i(glGetUniformLocation(shader->program, "gen_texture"), 0);
 
    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema3::RenderSimpleMesh_DiscoBall(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Set any other shader uniforms that you need
    GLint loc_time = glGetUniformLocation(shader->program, "time");
    glUniform1f(loc_time, (GLfloat)Engine::GetElapsedTime());


    if (texture)
    {
        // - activate texture location 0
        glActiveTexture(GL_TEXTURE0);
        // - bind the texture1 ID
        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
        // - send theuniform value
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

    }

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


Texture2D* Tema3::CreateRandomTexture(unsigned int width, unsigned int height)
{
    GLuint textureID = 0;
    unsigned int channels = 3;
    unsigned int size = width * height * channels;
    unsigned char* data = new unsigned char[size];

    // Generate random texture data
    for (int i = 0; i < size; i++) {
        data[i] = rand() % 256;
    }

    // Generate and bind the new texture ID
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    if (GLEW_EXT_texture_filter_anisotropic) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
    }

    // Set the texture parameters (MIN_FILTER, MAG_FILTER and WRAPPING MODE) using glTexParameteri
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    CheckOpenGLError();

    // Use glTexImage2D to set the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // TGenerate texture mip-maps
    glGenerateMipmap(GL_TEXTURE_2D);

    CheckOpenGLError();

    // Save the texture into a wrapper Texture2D class for using easier later during rendering phase
    Texture2D* texture = new Texture2D();
    texture->Init(textureID, width, height, channels);

    SAFE_FREE_ARRAY(data);
    return texture;
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema3::OnInputUpdate(float deltaTime, int mods)
{
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = GetSceneCamera()->m_transform->GetLocalOXVector();
        glm::vec3 forward = GetSceneCamera()->m_transform->GetLocalOZVector();
        forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));
    }
}


void Tema3::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_Z) {
        floorlights_mode = 1 - floorlights_mode;
    }
    if (key == GLFW_KEY_X) {
        spotlights_mode = 1 - spotlights_mode;
    }
    if (key == GLFW_KEY_C) {
        discolights_mode = 1 - discolights_mode;
    }

    if (key == GLFW_KEY_SPACE) {
        if (state == 0) {
            state++;
            floorlights_mode = ON;
            spotlights_mode = OFF;
            discolights_mode = OFF;
        }
        else if (state == 1) {
            state++;
            floorlights_mode = OFF;
            spotlights_mode = ON;
            discolights_mode = OFF;
        }
        else if (state == 2) {
            state++;
            floorlights_mode = OFF;
            spotlights_mode = OFF;
            discolights_mode = ON;
        }
        else if (state == 3) {
            state = 0;
            floorlights_mode = ON;
            spotlights_mode = ON;
            discolights_mode = ON;
        }
    }

    if (key == GLFW_KEY_M) {
        if (character + 1 < NUM_OF_CHARS) {
            character++;
        }
        else {
            character = 0;
        }
    }
}


void Tema3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema3::OnWindowResize(int width, int height)
{
}
