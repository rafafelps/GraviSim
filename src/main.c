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
char stopEvents = 0;
float scale = 0;
float norm = 0;

void update();
void render();
void initEarth();
void initMoon();
int collided();

int main(void) {
    InitWindow(screenWidth, screenHeight, "GraviSim");
    SetTargetFPS(60);

    initEarth();
    initMoon();

    while (!WindowShouldClose()) {
        if (IsKeyReleased(KEY_A)) {
            moon.vlc.x *= 0.95;
            moon.vlc.y *= 0.95;
        } else if (IsKeyReleased(KEY_D)) {
            moon.vlc.x *= 1.05;
            moon.vlc.y *= 1.05;
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
        Vector2 vers = {earth.pos.x - moon.pos.x, earth.pos.y - moon.pos.y};
        float sz = sqrtf(powf(vers.x, 2) + powf(vers.y, 2));

        // Normalizing vector
        vers.x /= sz;
        vers.y /= sz;

        sz *= scale; // unsigned int EMDistance = 384467e3;

        float gravitationalForce = (G * earth.mass * moon.mass * 1e48) / powf(sz, 2);
        float acl = gravitationalForce / (moon.mass * 1e24);

        moon.vlc.x += acl * vers.x;
        moon.vlc.y += acl * vers.y;

        moon.pos.x += moon.vlc.x;
        moon.pos.y += moon.vlc.y;
    } else { stopEvents = 1; SetTargetFPS(1); }
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
    moon.vlc = (Vector2){0, -0.92};
    moon.acl = (Vector2){0, 0};

    Image img = LoadImage("res/moon.png");
    ImageResize(&img, earth.texture.width * ((float)(moon.radius) / earth.radius), earth.texture.width * ((float)(moon.radius) / earth.radius));
    moon.texture = LoadTextureFromImage(img);
    UnloadImage(img);

    moon.pos.x = screenWidth * 0.5 + earth.texture.width * 0.5 + moon.texture.width * 4.5;
    moon.pos.y = screenHeight * 0.5;

    float sz = moon.pos.x - earth.pos.x;
    scale = 384467e3 / sz;
    norm = - sqrtf(G * (earth.mass * 1e24 + moon.mass * 1e24) / (sz * scale));
}

int collided() {
    float a = powf(earth.pos.x - moon.pos.x, 2);
    float b = powf(earth.pos.y - moon.pos.y, 2);
    float distance = sqrtf(a + b);

    return distance <= ((earth.texture.width * 0.5) + (moon.texture.width * 0.5));
}
