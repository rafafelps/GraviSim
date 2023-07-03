#include <raylib.h>
#include <math.h>

struct Body {
    float mass; // In 10^20 Kg
    float radius; // In Km
    Texture texture;
    Vector2 pos;
};

typedef struct Body Body;

const float screenRatio = 1.0 / 1.0;
const int screenHeight = 1000;
const int screenWidth = screenRatio * screenHeight;
Body earth;
Body moon;
float counter = 0;

void update();
void render();
void initEarth();
void initMoon();

int main(void) {
    InitWindow(screenWidth, screenHeight, "GraviSim");
    SetTargetFPS(60);

    initEarth();
    initMoon();

    unsigned int EMDistance = 384467;

    while (!WindowShouldClose()) {
        update();

        render();
    }

    UnloadTexture(earth.texture);
    UnloadTexture(moon.texture);
    CloseWindow();

    return 0;
}

void update() {
    moon.pos.x = screenWidth * 0.5 + (moon.texture.width * 107.5 + earth.texture.width * 0.5) * cosf(counter * (PI / 180.0)) - (moon.texture.width * 0.5);
    moon.pos.y = screenHeight * 0.5 + (moon.texture.width * 107.5 + earth.texture.width * 0.5) * -sinf(counter * (PI / 180.0)) - (moon.texture.width * 0.5);

    counter += 0.5;
    if (counter >= 360) { counter = 0; }
}

void render() {
    BeginDrawing();
    ClearBackground(BLACK);
    
    DrawTextureV(earth.texture, earth.pos, WHITE);
    DrawTextureV(moon.texture, moon.pos, WHITE);

    EndDrawing();
}

void initEarth() {
    earth.mass = 5.792 * 10e4;
    earth.radius = 6371;

    Image img = LoadImage("res/earth.png");
    ImageResize(&img, screenWidth / 53.0, screenWidth / 53.0);
    earth.texture = LoadTextureFromImage(img);
    UnloadImage(img);

    earth.pos.x = screenWidth * 0.5 - (earth.texture.width * 0.5);
    earth.pos.y = screenHeight * 0.5 - (earth.texture.height * 0.5);
}

void initMoon() {
    moon.mass = 7.342 * 10e2;
    moon.radius = 1737.4;

    Image img = LoadImage("res/moon.png");
    ImageResize(&img, earth.texture.width * ((float)(moon.radius) / earth.radius), earth.texture.width * ((float)(moon.radius) / earth.radius));
    moon.texture = LoadTextureFromImage(img);
    UnloadImage(img);

    moon.pos.x = (screenWidth * 0.5 + moon.texture.width * 107.5 + earth.texture.width * 0.5) - (moon.texture.width * 0.5);
    moon.pos.y = screenHeight * 0.5 - (moon.texture.height * 0.5);
}
