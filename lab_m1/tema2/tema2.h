#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/lab_camera.h"

#define N_MAZE 21
#define M_MAZE 21
#define FREE 1
#define WALL 0
#define ENEMY 2


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
        Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
        void BuildMaze();
        bool WallHit(float x, float y);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        implemented::Camera2 *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // TODO(student): If you need any other class variables, define them here.
        float fov;
        float left, right;
        float bottom, top;
        float z_near = 0.01;
        float z_far = 200;
        char projectionType;

        // character
        float x_char, y_char, z_char, rad_char;

        // maze
        int maze[N_MAZE][M_MAZE] = { WALL };

        // enemy
        float x_enemy, y_enemy, speed_enemy;
    };
}   // namespace m1
