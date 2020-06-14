#pragma once

#include "glm\vec3.hpp"
#include "glm\vec4.hpp"
#include "glm\vec2.hpp"
#include <glm\gtc\quaternion.hpp>
#include <glm\mat3x3.hpp>
#include <glm\mat4x4.hpp>
// JSON utility library
#include "json.hpp"

typedef glm::vec2 vector2;
typedef glm::vec3 vector3;
typedef glm::vec4 vector4;
typedef glm::mat3x3 matrix3;
typedef glm::mat4x4 matrix4;
typedef glm::quat quaternion;
// Namespace typedef
using json = nlohmann::json;