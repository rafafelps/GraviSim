#include <raylib.h>
#include <math.h>

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

void update();
void render();
void initEarth();
void initMoon();
int collided();

int main(void) {
    InitWindow(screenWidth, screenHeight, "GraviSim");
    SetTargetFPS(200);

    initEarth();
    initMoon();

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
    if (!collided()) {
        Vector2 vers = {earth.pos.x - moon.pos.x, earth.pos.y - moon.pos.y};
        float sz = sqrtf(powf(vers.x, 2) + powf(vers.y, 2));

        // Normalizing vector
        vers.x /= sz;
        vers.y /= sz;

        sz = 384467e3; // unsigned int EMDistance = 384467;

        Vector2 tg = {-vers.y, vers.x};

        float gravitationalForce = (G * earth.mass * moon.mass * 1e48) / powf(sz, 2);
        float acl = gravitationalForce / (moon.mass * 1e24);

        vers.x *= sqrtf(acl * sz);
        vers.y *= sqrtf(acl * sz);

        float orbitalVelocity = sqrtf(G * earth.mass * 1e24 / sz);
        tg.x *= sz;
        tg.y *= sz;

        moon.vlc.x = tg.x + vers.x;
        moon.vlc.y = tg.y + vers.y;

        sz = sqrtf(powf(moon.vlc.x, 2) + powf(moon.vlc.y, 2));

        moon.pos.x += moon.vlc.x / sz * 2.0;
        moon.pos.y += moon.vlc.y / sz * 2.0;
    }
}

void render() {
    BeginDrawing();
    ClearBackground(BLACK);
    
    Vector2 tmp = earth.pos;
    tmp.x -= earth.texture.width * 0.5;
    tmp.y -= earth.texture.height * 0.5;
    DrawTextureV(earth.texture, tmp, WHITE);

    tmp = moon.pos;
    tmp.x -= moon.texture.width * 0.5;
    tmp.y -= moon.texture.height * 0.5;
    DrawTextureV(moon.texture, tmp, WHITE);

    EndDrawing();
}

void initEarth() {
    earth.mass = 5.9724;
    earth.radius = 6371;
    earth.vlc = (Vector2){0, 0};
    earth.acl = (Vector2){0, 0};

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
    moon.vlc = (Vector2){0, 0};
    moon.acl = (Vector2){0, 0};

    Image img = LoadImage("res/moon.png");
    ImageResize(&img, earth.texture.width * ((float)(moon.radius) / earth.radius), earth.texture.width * ((float)(moon.radius) / earth.radius));
    moon.texture = LoadTextureFromImage(img);
    UnloadImage(img);

    moon.pos.x = screenWidth * 0.5 + earth.texture.width * 0.5 + moon.texture.width * 4.5;
    moon.pos.y = screenHeight * 0.5;
}

int collided() {
    float a = powf(earth.pos.x - moon.pos.x, 2);
    float b = powf(earth.pos.y - moon.pos.y, 2);
    float distance = sqrtf(a + b);

    return distance <= ((earth.texture.width * 0.5) + (moon.texture.width * 0.5));
}
