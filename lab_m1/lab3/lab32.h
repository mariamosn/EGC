#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Lab32 : public gfxc::SimpleScene
    {
     public:
        Lab32();
        ~Lab32();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        float cx, cy, cx2, cy2;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep1, angularStep2;

        // TODO(student): If you need any other class variables, define them here.
        float x1, y1, x2,
              y2, x3, y3,
              r1, r2;

    };
}   // namespace m1
