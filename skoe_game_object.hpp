#pragma once

#include "skoe_model.hpp"

#include <memory>

namespace skoe
{

    struct Transform2dComponent
    {
        glm::vec2 translation{};
        glm::vec2 scale{1.0f,1.0f};
        float rotation;

        glm::mat2 mat2() 
        {
            const float s = glm::sin(rotation);
            const float c = glm:: cos(rotation);
            glm::mat2 rotMatrix{{c, s}, {-s, c}};

            glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};
            return rotMatrix * scaleMat;
        }
    };

    class SkoeGameObject
    {
        public:
            using id_t = unsigned int;

            static SkoeGameObject createGameObject()
            {
                static id_t currentId = 0;
                return SkoeGameObject{currentId++};
            }

            SkoeGameObject(const SkoeGameObject &) = delete;
            SkoeGameObject &operator=(const SkoeGameObject &) = delete;
            SkoeGameObject(SkoeGameObject&&) = default;
            SkoeGameObject &operator=(SkoeGameObject &&) = default;

            id_t getId() {return id;}

            std::shared_ptr<SkoeModel> model{};
            glm::vec3 color{};
            Transform2dComponent transform2d{};

        private:
            SkoeGameObject(id_t objId) : id{objId} {}

            id_t id;
    
    };
    
}
