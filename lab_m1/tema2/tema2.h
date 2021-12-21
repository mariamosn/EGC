#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/lab_camera.h"

#define N_MAZE 21
#define M_MAZE 21
#define FREE 1
#define WALL 0
#define ENEMY 2
#define HIT 3

#define HEALTH_COOLDOWN 200
#define HEALTH_MAX 10
#define ENEMY_VANISH_SPEED 7
#define PROJ_COOLDOWN 200
#define EXPLOSION 300

#define ENEMY_SIZE 0.5

#define FIRST_PERSON 1
#define THIRD_PERSON 3

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3


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
        void RenderMeshOrtho(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
        void RenderMeshTest(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
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
        glm::mat4 orthoProjectionMatrix;

        // TODO(student): If you need any other class variables, define them here.
        float fov;
        float left, right;
        float bottom, top;
        float z_near = 0.01;
        float z_far = 200;
        char projectionType;

        // character
        float x_char, y_char, z_char, rad_char;
        int health, health_cooldown;

        // maze
        int maze[N_MAZE][M_MAZE] = { WALL };

        // enemy
        float x_enemy, y_enemy, speed_enemy;
        int numberOfEnemies = 20;
        int explosion_done;

        float random;
        float random_increase;
        int cnt;

        // projectile
        int proj_cooldown;
        float x_proj;
        float y_proj;
        float z_proj;
        bool show_proj;
        float x_dir;
        float y_dir;
        float z_dir;

        int camera_type;
        float third_person_distanceToTarget;
        glm::vec3 third_person_position;
        glm::vec3 third_person_forward;
        glm::vec3 third_person_right;
        glm::vec3 third_person_up;

        int facing;
    };
}   // namespace m1
