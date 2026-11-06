#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include "Vec3.h"
#include "mat4.h"
#include "cube.h"

static float focal_length = 300.0f;

static float camera_distance = 5.0f;

static bool running = true;
static uint32_t* pixel_buffer = NULL;

static int buffer_width = 800;
static int buffer_height = 600;

static void put_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < buffer_width && y >= 0 && y < buffer_height) {
        pixel_buffer[y * buffer_width + x] = color;
    }
}

static uint32_t make_color(uint8_t r, uint8_t g, uint8_t b) {
    return (uint32_t)r | ((uint32_t)g << 8) | ((uint32_t)b << 16);
}

static void draw_rect(int x, int y, int w, int h, uint32_t color) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            put_pixel(x + i, y + j, color);
        }
    }
}

static void safe_project(Vec3 point3d, int* out_x, int* out_y) {
    float z = point3d.z + camera_distance;

    if (z < 0.1f) {
        z = 0;
    }

    float x_proj = (point3d.x / z) * focal_length;
    float y_proj = (point3d.y / z) * focal_length;

    *out_x = (int)(x_proj + buffer_width / 2);
    *out_y = (int)(y_proj + buffer_height / 2);
}

static void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    int sx = (dx > 0) ? 1 : -1;
    int sy = (dy > 0) ? 1 : -1;

    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    int err = dx - dy;
    int x = x0;
    int y = y0;

    while (1) {
        put_pixel(x, y, color);

        if (x == x1 && y == y1) break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }

        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

int in = 0;
static int left = 1;
static int right = 2;
static int up = 4;
static int down = 8;

static int getcode(x, y) {
    int code = in;
    if (x < 0) { code = code | left; }
    if (x > buffer_width) { code = code | right; }
    if (y < 0)  { code = code | up; }
    if (y > buffer_height) { code = code | down; }
    return code;
}

static void draw_cutable_line(float x1, float y1, float x2, float y2, uint32_t color) {
    int code1 = getcode(x1, y1);
    int code2 = getcode(x2, y2);

    while (true) {
        if (code1 == 0 || code2 == 0) {

        }
    }
}

static void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x3, y3, color);
    draw_line(x3, y3, x1, y1, color);
}

static void swap_int(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

static void draw_filled_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    if (y1 > y2) { swap_int(&y1, &y2); swap_int(&x1, &x2); }
    if (y1 > y3) { swap_int(&y1, &y3); swap_int(&x1, &x3); }
    if (y2 > y3) { swap_int(&y2, &y3); swap_int(&x2, &x3); }

    for (int y = y1; y <= y3; y++) {
        int x_left, x_right;

        if (y <= y2) {
            if (y2 - y1 == 0) {
                x_left = x1;
            }
            else {
                x_left = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            }

            if (y3 - y1 == 0) {
                x_right = x1;
            }
            else {
                x_right = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
            }
        }
        else {
            if (y3 - y2 == 0) {
                x_left = x2;
            }
            else {
                x_left = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
            }

            if (y3 - y1 == 0) {
                x_right = x1;
            }
            else {
                x_right = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
            }
        }
        if (x_left > x_right) {
            swap_int(&x_left, &x_right);
        }
        for (int x = x_left; x <= x_right; x++) {
            put_pixel(x, y, color);
        }
    }
}

static float rotation_angle = 0.0f;

static void render(void) {
    for (int i = 0; i < buffer_width * buffer_height; i++) {
        pixel_buffer[i] = 0;
    }

    rotation_angle += 0.001f;

    Mat4 rot_x = mat4_rotate_x(rotation_angle);
    Mat4 rot_y = mat4_rotate_y(rotation_angle*2.0);
    Mat4 rotation = mat4_mul(rot_y, rot_x);

    int screen_x[8];
    int screen_y[8];

    for (int i = 0; i < 8; i++) {
        Vec3 rotated = mat4_mul_vec3(rotation, cube_vertices[i]);
        safe_project(rotated, &screen_x[i], &screen_y[i]);
    }

    for (int i = 0; i < 12; i++) {
        int a = cube_edges[i][0];
        int b = cube_edges[i][1];

        if (screen_x[a] < 0 || screen_y[a] < 0) continue;
        if (screen_x[b] < 0 || screen_y[b] < 0) continue;

        draw_line(screen_x[a], screen_y[a], screen_x[b], screen_y[b],
            make_color(0, 255, 255));
    }

    for (int i = 0; i < 8; i++) {
        if (screen_x[i] < 0 || screen_y[i] < 0) continue;
        draw_rect(screen_x[i] - 2, screen_y[i] - 2, 5, 5,
            make_color(255, 255, 0));
    }
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        running = false;
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            running = false;
        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = buffer_width;
        bmi.bmiHeader.biHeight = -buffer_height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        StretchDIBits(hdc,
            0, 0, buffer_width, buffer_height,
            0, 0, buffer_width, buffer_height,
            pixel_buffer, &bmi, DIB_RGB_COLORS, SRCCOPY);

        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int main(void) {
    AllocConsole();
    printf(">>> Engine starting...\n");

    HINSTANCE hInstance = GetModuleHandle(NULL);
    int nCmdShow = SW_SHOW;

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyEngineClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    if (!RegisterClassW(&wc)) {
        printf("ERROR: RegisterClass failed!\n");
        return 1;
    }

    HWND hwnd = CreateWindowExW(0, L"MyEngineClass", L"My 3D Engine", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, buffer_width, buffer_height, NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        printf("ERROR: CreateWindow failed!\n");
        return 1;
    }

    pixel_buffer = (uint32_t*)calloc(buffer_width * buffer_height, sizeof(uint32_t));
    if (!pixel_buffer) {
        printf("ERROR: Memory allocation failed!\n");
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    SetForegroundWindow(hwnd);
    printf(">>> Window created and shown!\n");

    MSG msg;
    int frame_count = 0;

    while (running) {
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        render();
        InvalidateRect(hwnd, NULL, FALSE);

        frame_count++;
        if (frame_count % 60 == 0) {
            printf(">>> Rendering... (Press ESC to exit)\n");
        }
    }

    printf(">>> Engine shutting down.\n");
    free(pixel_buffer);
    return 0;
}