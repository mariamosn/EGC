#include "lab_m1/tema3/tema3.h"

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
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(4, 5, 12));
    camera->SetRotation(glm::vec3(-0.4, 0, 0));
    camera->Update();

    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "textures");

    // Load textures
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "grass_bilboard.png").c_str(), GL_REPEAT);
        mapTextures["grass"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "crate.jpg").c_str(), GL_REPEAT);
        mapTextures["crate"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "earth.png").c_str(), GL_REPEAT);
        mapTextures["earth"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "vegetation", "bamboo", "bamboo.png").c_str(), GL_REPEAT);
        mapTextures["bamboo"] = texture;
    }

    {
        mapTextures["random"] = CreateRandomTexture(25, 25);
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
        Mesh* mesh = new Mesh("bamboo");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "vegetation", "bamboo"), "bamboo.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a simple quad
    {
        vector<glm::vec3> vertices
        {
            glm::vec3(0.5f,   0.5f, 0.0f),    // top right
            glm::vec3(0.5f,  -0.5f, 0.0f),    // bottom right
            glm::vec3(-0.5f, -0.5f, 0.0f),    // bottom left
            glm::vec3(-0.5f,  0.5f, 0.0f),    // top left
        };

        vector<glm::vec3> normals
        {
            glm::vec3(0, 1, 1),
            glm::vec3(1, 0, 1),
            glm::vec3(1, 0, 0),
            glm::vec3(0, 1, 0)
        };

        vector<glm::vec2> textureCoords
        {
            // TODO(student): Complete texture coordinates for the square
            glm::vec2(0.0f, 0.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 0.0f)

        };

        vector<unsigned int> indices =
        {
            0, 1, 3,
            1, 2, 3
        };

        Mesh* mesh = new Mesh("square");
        mesh->InitFromData(vertices, normals, textureCoords, indices);
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader *shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("FloorPlane");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader_Plane.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader_Plane.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("FloorLight");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader_FloorLight.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader_FloorLight.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("OneLight");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader_1Light.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader_1Light.glsl"), GL_FRAGMENT_SHADER);
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
    {
        Shader* shader = new Shader("NineLights_2");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader_9Lights_2.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader_9Lights_2.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    mix = false;
    earth = false;

    // Light & material properties
    {
        // lightPosition = glm::vec3(0, 1, 1);
        lightDirection = glm::vec3(0, -1, 0);
        materialShininess = 30;
        materialKd = 0.5;
        materialKs = 0.5;

        isSpotlight = 0;
        cutOff = 45;
        angleOX = 0;
        angleOY = 0;
    }

    Generate_Floor();
}

void Tema3::Generate_Floor() {
    for (int i = 0; i < FLOOR_SIZE; i++) {
        for (int j = 0; j < FLOOR_SIZE; j++) {
            float r, g, b;
            r = (float)(rand() % 256) / 255;
            g = (float)(rand() % 256) / 255;
            b = (float)(rand() % 256) / 255;
            floor[i][j] = glm::vec3(r, g, b);
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
    // floor
    for (int i = 0; i < FLOOR_SIZE; i++) {
        for (int j = 0; j < FLOOR_SIZE; j++) {
            // floor tile
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.5, 0, j + 0.5));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 0.01, 1));
                RenderSimpleMesh_Floor(meshes["box"], shaders["FloorPlane"], modelMatrix, floor[i][j]);
            }
        }
    }

    // walls
    for (int i = 0; i < FLOOR_SIZE; i++) {
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 2, i + 0.5));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 4, 1));

            if (i == 0) {
                RenderSimpleMesh_9Lights(meshes["box"], shaders["NineLights_2"], modelMatrix,
                    floor[0][i], glm::vec3(0 + 0.5, 0, i + 0.5),
                    floor[0][i + 1], glm::vec3(0 + 0.5, 0, i + 1 + 0.5),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1)
                );
            }
            else if (i == FLOOR_SIZE - 1) {
                RenderSimpleMesh_9Lights(meshes["box"], shaders["NineLights_2"], modelMatrix,
                    floor[0][i], glm::vec3(0 + 0.5, 0, i + 0.5),
                    floor[0][i - 1], glm::vec3(0 + 0.5, 0, i - 1 + 0.5),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1)
                );
            }
            else {
                RenderSimpleMesh_9Lights(meshes["box"], shaders["NineLights_2"], modelMatrix,
                    floor[0][i], glm::vec3(0 + 0.5, 0, i + 0.5),
                    floor[0][i - 1], glm::vec3(0 + 0.5, 0, i - 1 + 0.5),
                    floor[0][i + 1], glm::vec3(0 + 0.5, 0, i + 1 + 0.5),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1)
                );
            }
        }
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(FLOOR_SIZE, 2, i + 0.5));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 4, 1));
            
            if (i == 0) {
                RenderSimpleMesh_9Lights(meshes["box"], shaders["NineLights_2"], modelMatrix,
                    floor[FLOOR_SIZE - 1][i], glm::vec3(FLOOR_SIZE - 1 + 0.5, 0, i + 0.5),
                    floor[FLOOR_SIZE - 1][i + 1], glm::vec3(FLOOR_SIZE - 1 + 0.5, 0, i + 1 + 0.5),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1)
                );
            }
            else if (i == FLOOR_SIZE - 1) {
                RenderSimpleMesh_9Lights(meshes["box"], shaders["NineLights_2"], modelMatrix,
                    floor[FLOOR_SIZE - 1][i], glm::vec3(FLOOR_SIZE - 1 + 0.5, 0, i + 0.5),
                    floor[FLOOR_SIZE - 1][i - 1], glm::vec3(FLOOR_SIZE - 1 + 0.5, 0, i - 1 + 0.5),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1)
                );
            }
            else {
                RenderSimpleMesh_9Lights(meshes["box"], shaders["NineLights_2"], modelMatrix,
                    floor[FLOOR_SIZE - 1][i], glm::vec3(FLOOR_SIZE - 1 + 0.5, 0, i + 0.5),
                    floor[FLOOR_SIZE - 1][i - 1], glm::vec3(FLOOR_SIZE - 1 + 0.5, 0, i - 1 + 0.5),
                    floor[FLOOR_SIZE - 1][i + 1], glm::vec3(FLOOR_SIZE - 1 + 0.5, 0, i + 1 + 0.5),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1)
                );
            }
        }
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.5, 2, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 4, 0.1));

            if (i == 0) {
                RenderSimpleMesh_9Lights(meshes["box"], shaders["NineLights_2"], modelMatrix,
                    floor[i][0], glm::vec3(i + 0.5, 0, 0 + 0.5),
                    floor[i + 1][0], glm::vec3(i + 1 + 0.5, 0, 0 + 0.5),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1)
                );
            }
            else if (i == FLOOR_SIZE - 1) {
                RenderSimpleMesh_9Lights(meshes["box"], shaders["NineLights_2"], modelMatrix,
                    floor[i][0], glm::vec3(i + 0.5, 0, 0 + 0.5),
                    floor[i - 1][0], glm::vec3(i - 1 + 0.5, 0, 0 + 0.5),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1)
                );
            }
            else {
                RenderSimpleMesh_9Lights(meshes["box"], shaders["NineLights_2"], modelMatrix,
                    floor[i][0], glm::vec3(i + 0.5, 0, 0 + 0.5),
                    floor[i + 1][0], glm::vec3(i + 1 + 0.5, 0, 0 + 0.5),
                    floor[i - 1][0], glm::vec3(i - 1 + 0.5, 0, 0 + 0.5),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1),
                    glm::vec3(-1), glm::vec3(-1)
                );
            }
        }
    }

    /*
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 1, -3));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
        earth = true;
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, mapTextures["earth"]);
        earth = false;
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(2, 0.5f, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["crate"]);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-2, 0.5f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(75.0f), glm::vec3(1, 1, 0));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["random"]);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        mix = true;
        RenderSimpleMesh(meshes["square"], shaders["LabShader"], modelMatrix, mapTextures["grass"], mapTextures["crate"]);
        mix = false;
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-2, -0.5f, -3));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderSimpleMesh(meshes["bamboo"], shaders["LabShader"], modelMatrix, mapTextures["bamboo"]);
    }
    */
}


void Tema3::FrameEnd()
{
    DrawCoordinateSystem();
}

void Tema3::RenderSimpleMesh_Floor(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
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

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderSimpleMesh_1Light(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, const glm::vec3& lightPosition)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light properties
    int light_position = glGetUniformLocation(shader->program, "light_position");
    glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

    /*
    int light_direction = glGetUniformLocation(shader->program, "light_direction");
    glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);
    */
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

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);
    /*
    // TODO(student): Set any other shader uniforms that you need
    GLint type = glGetUniformLocation(shader->program, "is_spotlight");
    glUniform1i(type, isSpotlight);

    GLfloat cut_off = glGetUniformLocation(shader->program, "cut_off_angle");
    glUniform1f(cut_off, cutOff);
    */
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

void Tema3::RenderSimpleMesh_9Lights(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix,
                                        const glm::vec3& color1 = glm::vec3(-1), const glm::vec3& lightPosition1 = glm::vec3(-1),
                                        const glm::vec3& color2 = glm::vec3(-1), const glm::vec3& lightPosition2 = glm::vec3(-1),
                                        const glm::vec3& color3 = glm::vec3(-1), const glm::vec3& lightPosition3 = glm::vec3(-1),
                                        const glm::vec3& color4 = glm::vec3(-1), const glm::vec3& lightPosition4 = glm::vec3(-1),
                                        const glm::vec3& color5 = glm::vec3(-1), const glm::vec3& lightPosition5 = glm::vec3(-1),
                                        const glm::vec3& color6 = glm::vec3(-1), const glm::vec3& lightPosition6 = glm::vec3(-1),
                                        const glm::vec3& color7 = glm::vec3(-1), const glm::vec3& lightPosition7 = glm::vec3(-1),
                                        const glm::vec3& color8 = glm::vec3(-1), const glm::vec3& lightPosition8 = glm::vec3(-1),
                                        const glm::vec3& color9 = glm::vec3(-1), const glm::vec3& lightPosition9 = glm::vec3(-1)
)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light properties
    int light_position1 = glGetUniformLocation(shader->program, "light_position1");
    glUniform3f(light_position1, lightPosition1.x, lightPosition1.y, lightPosition1.z);

    int light_position2 = glGetUniformLocation(shader->program, "light_position2");
    glUniform3f(light_position2, lightPosition2.x, lightPosition2.y, lightPosition2.z);

    int light_position3 = glGetUniformLocation(shader->program, "light_position3");
    glUniform3f(light_position3, lightPosition3.x, lightPosition3.y, lightPosition3.z);

    int light_position4 = glGetUniformLocation(shader->program, "light_position4");
    glUniform3f(light_position4, lightPosition4.x, lightPosition4.y, lightPosition4.z);

    int light_position5 = glGetUniformLocation(shader->program, "light_position5");
    glUniform3f(light_position5, lightPosition5.x, lightPosition5.y, lightPosition5.z);

    int light_position6 = glGetUniformLocation(shader->program, "light_position6");
    glUniform3f(light_position6, lightPosition6.x, lightPosition6.y, lightPosition6.z);

    int light_position7 = glGetUniformLocation(shader->program, "light_position7");
    glUniform3f(light_position7, lightPosition7.x, lightPosition7.y, lightPosition7.z);

    int light_position8 = glGetUniformLocation(shader->program, "light_position8");
    glUniform3f(light_position8, lightPosition8.x, lightPosition8.y, lightPosition8.z);

    int light_position9 = glGetUniformLocation(shader->program, "light_position9");
    glUniform3f(light_position9, lightPosition9.x, lightPosition9.y, lightPosition9.z);

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

    int object1_color = glGetUniformLocation(shader->program, "object1_color");
    glUniform3f(object1_color, color1.r, color1.g, color1.b);

    int object2_color = glGetUniformLocation(shader->program, "object2_color");
    glUniform3f(object2_color, color2.r, color2.g, color2.b);

    int object3_color = glGetUniformLocation(shader->program, "object3_color");
    glUniform3f(object3_color, color3.r, color3.g, color3.b);

    int object4_color = glGetUniformLocation(shader->program, "object4_color");
    glUniform3f(object4_color, color4.r, color4.g, color4.b);

    int object5_color = glGetUniformLocation(shader->program, "object5_color");
    glUniform3f(object5_color, color5.r, color5.g, color5.b);

    int object6_color = glGetUniformLocation(shader->program, "object6_color");
    glUniform3f(object6_color, color6.r, color6.g, color6.b);

    int object7_color = glGetUniformLocation(shader->program, "object7_color");
    glUniform3f(object7_color, color7.r, color7.g, color7.b);

    int object8_color = glGetUniformLocation(shader->program, "object8_color");
    glUniform3f(object8_color, color8.r, color8.g, color8.b);

    int object9_color = glGetUniformLocation(shader->program, "object9_color");
    glUniform3f(object9_color, color9.r, color9.g, color9.b);
 
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


void Tema3::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture1, Texture2D* texture2)
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

    // TODO(student): Set any other shader uniforms that you need

    glUniform1i(glGetUniformLocation(shader->program, "mix_tex"), mix);
    glUniform1i(glGetUniformLocation(shader->program, "earth"), earth);

    GLint loc_time = glGetUniformLocation(shader->program, "time");
    glUniform1f(loc_time, (GLfloat)Engine::GetElapsedTime());


    if (texture1)
    {
        // TODO(student): Do these:
        // - activate texture location 0
        glActiveTexture(GL_TEXTURE0);
        // - bind the texture1 ID
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        // - send theuniform value
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

    }

    if (texture2)
    {
        // TODO(student): Do these:
        // - activate texture location 1
        glActiveTexture(GL_TEXTURE1);
        // - bind the texture2 ID
        glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
        // - send the uniform value
        glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);

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

    // TODO(student): Generate random texture data
    for (int i = 0; i < size; i++) {
        data[i] = rand() % 256;
    }

    // TODO(student): Generate and bind the new texture ID
    // unsigned int gl_texture_object;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    if (GLEW_EXT_texture_filter_anisotropic) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
    }
    // TODO(student): Set the texture parameters (MIN_FILTER, MAG_FILTER and WRAPPING MODE) using glTexParameteri
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    CheckOpenGLError();

    // Use glTexImage2D to set the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // TODO(student): Generate texture mip-maps
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
