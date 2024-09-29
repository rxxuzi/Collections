/*
 * color.h - A simple color manipulation library for C
 *
 * This header-only library provides functions to manipulate colors, convert between
 * different color spaces (RGB, HSL, Hex), and output colored text to the console.
 * It supports both Windows (MinGW) and Linux (GCC) environments.
 * 
 * Example:
 *      printf(RED("This is red text\n"));
 *      c_256f(10, "This is green text\n");
 *      c_hexf("#0000ff","This is blue text\n");
 *      c_rgbf(newRGB(255,255,0), "This is yellow text\n");
 *  
 * Support: Windows (MinGW), Linux (GCC)
 * Author: github.com/rxxuzi
 * License: CC0 Public Domain
 */

#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RGB {
    int r;
    int g;
    int b;
} RGB;

typedef struct HSL {
    float h;  // Hue 0-360
    float s;  // Saturation 0-1
    float l;  // Lightness 0-1
} HSL;

static void evtp(void);
#ifdef _WIN32
static void evtp(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#else
static void evtp(void) {
    // Linux doesn't need any special setup for ANSI escape codes
}
#endif

__attribute__((constructor))
static void auto_init(void) {
    static int initialized = 0;
    if (!initialized) {
        evtp();
        initialized = 1;
    }
}

// Color output functions
void c_256f(int color, const char *format, ...);
void c_rgbf(RGB color, const char *format, ...);
void c_hexf(const char *hex, const char *format, ...);
void c_hslf(HSL color, const char *format, ...); 

// Color manipulation functions
RGB newRGB(int r, int g, int b);
HSL rgb_to_hsl(RGB rgb);
RGB hsl_to_rgb(HSL hsl);
RGB blend_colors(RGB c1, RGB c2, float ratio);
RGB complementary_color(RGB c);
RGB lighten_color(RGB c, float amount);
RGB darken_color(RGB c, float amount);

static void _hex_to_rgb(const char *hex, RGB *color);
static int _clamp(int value, int min, int max);
static float _clamp_float(float value, float min, float max);

static float _fmax(float a, float b) {
    return (a > b) ? a : b;
}

static float _fmin(float a, float b) {
    return (a < b) ? a : b;
}

static float _fmod(float x, float y) {
    return x - y * (int)(x / y);
}

static float _fabs(float x) {
    return (x > 0) ? x : -x;
}

static void _hex_to_rgb(const char *hex, RGB *color) {
    if (hex[0] == '#') hex++;
    unsigned int hex_color = (unsigned int)strtol(hex, NULL, 16);
    color->r = (hex_color >> 16) & 0xFF;
    color->g = (hex_color >> 8) & 0xFF;
    color->b = hex_color & 0xFF;
}

static int _clamp(int value, int min, int max) {
    return (value < min) ? min : (value > max) ? max : value;
}

static float _clamp_float(float value, float min, float max) {
    return (value < min) ? min : (value > max) ? max : value;
}

RGB newRGB(int r, int g, int b) {
    return (RGB){_clamp(r, 0, 255), _clamp(g, 0, 255), _clamp(b, 0, 255)};
}

RGB hex_to_rgb(const char *hex) {
    RGB color;
    _hex_to_rgb(hex, &color);
    return color;
}

HSL rgb_to_hsl(RGB rgb) {
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;
    float max = _fmax(_fmax(r, g), b);
    float min = _fmin(_fmin(r, g), b);
    float h, s, l = (max + min) / 2.0f;

    if (max == min) {
        h = s = 0;  // achromatic
    } else {
        float d = max - min;
        s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
        if (max == r)
            h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        else if (max == g)
            h = (b - r) / d + 2.0f;
        else
            h = (r - g) / d + 4.0f;
        h /= 6.0f;
    }

    HSL hsl = {h * 360.0f, s, l};
    return hsl;
}

RGB hsl_to_rgb(HSL hsl) {
    float h = hsl.h / 60.0f;
    float s = hsl.s;
    float l = hsl.l;
    float c = (1 - _fabs(2 * l - 1)) * s;
    float x = c * (1 - _fabs(_fmod(h, 2.0f) - 1));
    float m = l - c / 2;
    float r, g, b;

    if (h < 1) {
        r = c; g = x; b = 0;
    } else if (h < 2) {
        r = x; g = c; b = 0;
    } else if (h < 3) {
        r = 0; g = c; b = x;
    } else if (h < 4) {
        r = 0; g = x; b = c;
    } else if (h < 5) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }

    RGB rgb = {
        _clamp((r + m) * 255, 0, 255),
        _clamp((g + m) * 255, 0, 255),
        _clamp((b + m) * 255, 0, 255)
    };
    return rgb;
}

RGB blend_colors(RGB c1, RGB c2, float ratio) {
    if (ratio < 0.0 || ratio > 1.0) return (RGB){0, 0, 0};

    float m = 1.0 - ratio;
    RGB result = {
        (int)(c1.r * m + c2.r * ratio + 0.5),
        (int)(c1.g * m + c2.g * ratio + 0.5),
        (int)(c1.b * m + c2.b * ratio + 0.5)
    };
    return result;
}

RGB complementary_color(RGB c) {
    HSL hsl = rgb_to_hsl(c);
    hsl.h = _fmod(hsl.h + 180.0f, 360.0f);
    return hsl_to_rgb(hsl);
}

RGB lighten_color(RGB c, float amount) {
    HSL hsl = rgb_to_hsl(c);
    hsl.l = _clamp_float(hsl.l + amount, 0.0f, 1.0f);
    return hsl_to_rgb(hsl);
}

RGB darken_color(RGB c, float amount) {
    return lighten_color(c, -amount);
}


/* outputs */
void c_256f(int color, const char *format, ...) {
    printf("\x1b[38;5;%dm", color);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\x1b[0m");
}

void c_rgbf(RGB color, const char *format, ...) {
    printf("\x1b[38;2;%d;%d;%dm", color.r, color.g, color.b);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\x1b[0m");
}

void c_hexf(const char *hex, const char *format, ...) {
    RGB color;
    _hex_to_rgb(hex, &color);

    printf("\x1b[38;2;%d;%d;%dm", color.r, color.g, color.b);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\x1b[0m");
}

void c_hslf(HSL color, const char *format, ...) {
    RGB rgb = hsl_to_rgb(color);
    printf("\x1b[38;2;%d;%d;%dm", rgb.r, rgb.g, rgb.b);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\x1b[0m");
}


void c_gradf(RGB *colors, int color_count, const char *format, ...) {
    if (color_count < 2) {
        fprintf(stderr, "Error: At least two colors are required for a gradient.\n");
        return;
    }

    va_list args;
    va_start(args, format);
    int len = vsnprintf(NULL, 0, format, args);
    va_end(args);

    char *text = malloc(len + 1);
    va_start(args, format);
    vsnprintf(text, len + 1, format, args);
    va_end(args);

    for (int i = 0; i < len; i++) {
        float t = (float)i / (len - 1);
        int seg = (int)(t * (color_count - 1));
        float f = (t * (color_count - 1)) - seg;

        RGB s = colors[seg];
        RGB e = colors[seg + 1];

        RGB color = {
            (int)((1 - f) * s.r + f * e.r),
            (int)((1 - f) * s.g + f * e.g),
            (int)((1 - f) * s.b + f * e.b)
        };

        printf("\x1b[38;2;%d;%d;%dm%c", color.r, color.g, color.b, text[i]);
    }

    printf("\x1b[0m");
    free(text);
}

// Color macros
#define BLACK(text) "\x1b[30m" text "\x1b[0m"
#define RED(text) "\x1b[31m" text "\x1b[0m"
#define GREEN(text) "\x1b[32m" text "\x1b[0m"
#define YELLOW(text) "\x1b[33m" text "\x1b[0m"
#define BLUE(text) "\x1b[34m" text "\x1b[0m"
#define MAGENTA(text) "\x1b[35m" text "\x1b[0m"
#define CYAN(text) "\x1b[36m" text "\x1b[0m"
#define WHITE(text) "\x1b[37m" text "\x1b[0m"

#ifdef __cplusplus
}
#endif
#endif // COLOR_H