#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define G 6.6743e-11

struct Body {
    float mass; // In 10^24 Kg
    float radius; // In Km
    Texture texture;
    Vector2 pos;
    Vector2 vlc;
    Vector2 acl;
};

typedef struct Body Body;

const float screenRatio = 16.0 / 9.0;
const int screenHeight = 720;
const int screenWidth = screenRatio * screenHeight;
Body earth;
Body moon;
char stopEvents = 0;
float distance = 384467e3;
float ratio = 0;
float t = 0;

void update();
void render();
void initEarth();
void initMoon();
int collided();

int main() {
    InitWindow(screenWidth, screenHeight, "GraviSim");
    SetTargetFPS(60);

    initEarth();
    initMoon();

    while (!WindowShouldClose()) {
        if (IsKeyReleased(KEY_A)) {
            distance *= 0.9; 
        } else if (IsKeyReleased(KEY_D)) {
            distance *= 1.1;
        } else if (IsKeyReleased(KEY_R)) {
            distance = 384467e3;
        }

        if (!stopEvents) {
            for (int i = 0; i < 3; i++)
                update();
        }
        render();
    }

    UnloadTexture(earth.texture);
    UnloadTexture(moon.texture);
    CloseWindow();

    return 0;
}

void update() {
    if (!collided()) {
        moon.pos.x = screenWidth * 0.5 + (distance * ratio) * cosf(t * (PI / 180.0));
        moon.pos.y = screenHeight * 0.5 + (distance * ratio) * -sinf(t * (PI / 180.0));

        t += (0.2 * 384467e3) / distance;
        if (t >= 360) { t = 0; }
    } else {
        distance *= 1.1;

        moon.pos.x = screenWidth * 0.5 + (distance * ratio) * cosf(t * (PI / 180.0));
        moon.pos.y = screenHeight * 0.5 + (distance * ratio) * -sinf(t * (PI / 180.0));
    }
}

void render() {
    BeginDrawing();
    ClearBackground(BLACK);

    DrawCircleLines(earth.pos.x, earth.pos.y, distance * ratio, WHITE);
    
    Vector2 tmp = earth.pos;
    tmp.x -= earth.texture.width * 0.5;
    tmp.y -= earth.texture.height * 0.5;
    DrawTextureV(earth.texture, tmp, WHITE);

    tmp = moon.pos;
    tmp.x -= moon.texture.width * 0.5;
    tmp.y -= moon.texture.height * 0.5;
    DrawTextureV(moon.texture, tmp, WHITE);

    DrawText("Orbital Velocity:", 0.8 * screenWidth, 0.8 * screenHeight, screenHeight * (25.0 / 720), WHITE);
    float oVl = sqrtf(G * earth.mass * 1e24 / distance);
    int len = snprintf(NULL, 0, "%f", oVl);
    char *info = malloc(len + 1);
    snprintf(info, len + 1, "%f", oVl);
    DrawText(info, 0.85 * screenWidth, 0.85 * screenHeight, screenHeight * (25.0 / 720), WHITE);
    free(info);

    DrawText("Distance: ", 0.8 * screenWidth, 0.7 * screenHeight, screenHeight * (25.0 / 720), WHITE);
    len = snprintf(NULL, 0, "%f", distance);
    info = malloc(len + 1);
    snprintf(info, len + 1, "%f", distance);
    for (int i = 0; info[i] != '\0'; i++) {
        if (info[i] == '.') { info[i] = '\0'; break; }
    }
    DrawText(info, 0.85 * screenWidth, 0.75 * screenHeight, screenHeight * (25.0 / 720), WHITE);
    free(info);


    EndDrawing();
}

void initEarth() {
    earth.mass = 5.9724;
    earth.radius = 6371;

    Image img = LoadImage("res/earth.png");
    ImageResize(&img, screenWidth / 7.0, screenWidth / 7.0);
    earth.texture = LoadTextureFromImage(img);
    UnloadImage(img);

    earth.pos.x = screenWidth * 0.5;
    earth.pos.y = screenHeight * 0.5;
}

void initMoon() {
    moon.mass = 0.07346;
    moon.radius = 1737.4;

    Image img = LoadImage("res/moon.png");
    ImageResize(&img, earth.texture.width * ((float)(moon.radius) / earth.radius), earth.texture.width * ((float)(moon.radius) / earth.radius));
    moon.texture = LoadTextureFromImage(img);
    UnloadImage(img);

    moon.pos.x = screenWidth * 0.5 + earth.texture.width * 0.5 + moon.texture.width * 4.5;
    moon.pos.y = screenHeight * 0.5;

    ratio = (earth.texture.width * 0.5 + moon.texture.width * 4.5) / distance;
}

int collided() {
    float a = powf(earth.pos.x - moon.pos.x, 2);
    float b = powf(earth.pos.y - moon.pos.y, 2);
    float distance = sqrtf(a + b);

    return distance <= ((earth.texture.width * 0.5) + (moon.texture.width * 0.5));
}
