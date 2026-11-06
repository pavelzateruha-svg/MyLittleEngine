#pragma once
#include "vec3.h"

// Матрица 4x4 (хранится как массив из 16 чисел)
typedef struct {
    float m[16];
} Mat4;

// Единичная матрица
static inline Mat4 mat4_identity(void) {
    Mat4 mat = { 0 };
    mat.m[0] = 1.0f;  mat.m[5] = 1.0f;
    mat.m[10] = 1.0f; mat.m[15] = 1.0f;
    return mat;
}

// Умножение матрицы на вектор
static inline Vec3 mat4_mul_vec3(Mat4 mat, Vec3 v) {
    Vec3 result;
    result.x = mat.m[0] * v.x + mat.m[4] * v.y + mat.m[8] * v.z + mat.m[12];
    result.y = mat.m[1] * v.x + mat.m[5] * v.y + mat.m[9] * v.z + mat.m[13];
    result.z = mat.m[2] * v.x + mat.m[6] * v.y + mat.m[10] * v.z + mat.m[14];
    return result;
}

// Матрица переноса (сдвигает объект на x, y, z)
static inline Mat4 mat4_translate(float x, float y, float z) {
    Mat4 mat = mat4_identity();
    mat.m[12] = x;
    mat.m[13] = y;
    mat.m[14] = z;
    return mat;
}

// Матрица масштабирования
static inline Mat4 mat4_scale(float x, float y, float z) {
    Mat4 mat = mat4_identity();
    mat.m[0] = x;
    mat.m[5] = y;
    mat.m[10] = z;
    return mat;
}
// Матрица поворота вокруг оси Y (поворот влево-вправо)
static inline Mat4 mat4_rotate_y(float angle) {
    Mat4 mat = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    mat.m[0] = c;   mat.m[8] = s;
    mat.m[2] = -s;  mat.m[10] = c;
    return mat;
}

// Матрица поворота вокруг оси X (наклон вперёд-назад)
static inline Mat4 mat4_rotate_x(float angle) {
    Mat4 mat = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    mat.m[5] = c;   mat.m[9] = -s;
    mat.m[6] = s;   mat.m[10] = c;
    return mat;
}

// Умножение двух матриц 4x4
static inline Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 result = { 0 };
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                // Матрица хранится по столбцам (column-major)
                sum += a.m[col * 4 + k] * b.m[k * 4 + row];
            }
            result.m[col * 4 + row] = sum;
        }
    }
    return result;
}