#pragma once

#include <string>
#include <unordered_map>

#include "components/simple_scene.h"
#include "components/transform.h"

#define FLOOR_SIZE 8
#define DANCERS 5

namespace m1
{
    class Tema3 : public gfxc::SimpleScene
    {
     public:
        Tema3();
        ~Tema3();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D *texture1 = NULL, Texture2D *texture2 = NULL);
        void RenderSimpleMesh_1Light(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, const glm::vec3& lightPosition);
        void RenderSimpleMesh_Floor(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
        void RenderSimpleMesh_9Lights(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix,
            const glm::vec3& color1, const glm::vec3& lightPosition1,
            const glm::vec3& color2, const glm::vec3& lightPosition2,
            const glm::vec3& color3, const glm::vec3& lightPosition3,
            const glm::vec3& color4, const glm::vec3& lightPosition4,
            const glm::vec3& color5, const glm::vec3& lightPosition5,
            const glm::vec3& color6, const glm::vec3& lightPosition6,
            const glm::vec3& color7, const glm::vec3& lightPosition7,
            const glm::vec3& color8, const glm::vec3& lightPosition8,
            const glm::vec3& color9, const glm::vec3& lightPosition9
        );
        void RenderMeshCentered(int a, int b, const glm::mat4& modelMatrix);
        Texture2D *CreateRandomTexture(unsigned int width, unsigned int height);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void Generate_Floor();
        void Generate_Dancers();
        void MoveDancer(int i);

        std::unordered_map<std::string, Texture2D *> mapTextures;

        bool mix;
        bool earth;

        // glm::vec3 lightPosition;
        glm::vec3 lightDirection;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;

        // TODO(student): If you need any other class variables, define them here.
        GLint isSpotlight;
        GLfloat cutOff;
        GLfloat angleOX, angleOY;

        glm::vec3 floor[FLOOR_SIZE + 2][FLOOR_SIZE + 2];

        float x_dancers[DANCERS];
        float x_dir_dancers[DANCERS];
        float z_dancers[DANCERS];
        float z_dir_dancers[DANCERS];
        float dancers_speed = 0.05;
    };
}   // namespace m1
