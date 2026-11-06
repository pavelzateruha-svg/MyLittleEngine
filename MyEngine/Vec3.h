#pragma once
#include <math.h>

// Вектор в 3D-пространстве
typedef struct {
    float x;
    float y;
    float z;
} Vec3;

// Создание вектора
static inline Vec3 vec3(float x, float y, float z) {
    Vec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

// Сложение двух векторов
static inline Vec3 vec3_add(Vec3 a, Vec3 b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

// Вычитание векторов
static inline Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

// Умножение вектора на число
static inline Vec3 vec3_mul(Vec3 v, float s) {
    return vec3(v.x * s, v.y * s, v.z * s);
}

// Длина вектора
static inline float vec3_length(Vec3 v) {
    // sqrtf — квадратный корень из math.h
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Нормализация вектора (делаем длину = 1)
static inline Vec3 vec3_normalize(Vec3 v) {
    float len = vec3_length(v);
    if (len == 0.0f) return vec3(0, 0, 0);
    return vec3(v.x / len, v.y / len, v.z / len);
}