#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"

#define linesCount 13
#define MAX_TEXTBOX 64

const int screenW = 1920;
const int screenH = 1080;
const int displayW = screenW * 0.75;
const int FPS = 60;
const int lineH = screenH / linesCount;

bool lineIsUsed[linesCount];

//----- struct Button -----

typedef struct{
    Rectangle rc;
    const char* text;

    Vector2 textPos;
    int textSize;
    int line;
  
    bool hovered;
    bool clicked;
    Color Ncol;
    Color Hcol;
    Color Ccol;
    Color Tcol;

    bool enable;
} Button;

Button buttons[linesCount];
int buttonCount = 0;
Button* addButtonAtPosition(Vector2 position, Vector2 size, char* text) {
    if(buttonCount >= linesCount) {
        return NULL;
    }
    Rectangle rc = (Rectangle) {position.x, position.y, size.x, size.y};
    buttons[buttonCount++] = (Button) {
        .rc = rc,
        .text = text,
        .textSize = rc.height * 0.7,
        .textPos = (Vector2){rc.x + rc.width / 2 - strlen(text) * rc.height / 4,
                             rc.y + rc.height * 0.15},
        .Ncol = BLUE,
        .Hcol = SKYBLUE,
        .Ccol = RED,
        .Tcol = BLACK,
        .enable = true,
    };
    return buttons + buttonCount - 1;
}

Button* addButton(Vector2 size, char* text, int line) {
    if(lineIsUsed[line]) {
        return NULL;
    }
    lineIsUsed[line] = true;
    Button* b = addButtonAtPosition((Vector2) {displayW, line * lineH}, size, text);
    if(b == NULL) {
        lineIsUsed[line] = false;
        return NULL;
    }
    b->line = line;
    return b;
}

void updateButtons() {
    Vector2 mouse = GetMousePosition();
    for(int i = 0; i < buttonCount; i++) {
        Button* b = &buttons[i];
        if(!b->enable) continue;
        b->hovered = CheckCollisionPointRec(mouse, b->rc);
        b->clicked = b->hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }
}

void drawButtons() {
    for(int i = 0; i < buttonCount; i++) {
        Button* b = &buttons[i];
        Color col = b->hovered ? (b->clicked ? b->Ccol : b->Hcol) : b->Ncol;
        DrawRectangleRec(b->rc, col);
        DrawRectangleLinesEx(b->rc, lineH * 0.04f, b->Tcol);
        DrawText(b->text, b->textPos.x, b->textPos.y, b->textSize, b->Tcol);
    }
}

//----- struct textBox -----
char temp[MAX_TEXTBOX];
typedef void (*Formatter)();

#define format(skeleton, ...)                                   \
    ({                                                          \
        void formatP() {                                        \
            snprintf(temp, MAX_TEXTBOX, skeleton, __VA_ARGS__); \
        }                                                       \
        formatP;                                                \
    })


typedef struct{
    char text[MAX_TEXTBOX];
    Vector2 position;
    int size;
    Color col;

    void *valuePtr;
    Formatter formatter;
} textBox;

textBox textBoxs[linesCount];
int textBoxCount = 0;
int defaultSize = lineH * 0.7;

textBox* addtextBox(int line, Color col, Formatter fmtr) {
    if(lineIsUsed[line]) {
        return NULL;
    }
  
    lineIsUsed[line] = true;
    textBoxs[textBoxCount++] = (textBox) {
        .position = (Vector2) { displayW, line * lineH},
        .formatter = fmtr,
        .size = defaultSize,
        .col = col,
    };
    return textBoxs + textBoxCount - 1;
}
void updateTextBoxs() {
    for(int i = 0; i < textBoxCount; i++) {
        textBox* t = &textBoxs[i];
        t->formatter();
        strcpy(t->text, temp);
    }
}
void drawTextBoxs() {
    for(int i = 0; i < textBoxCount; i++) {
        textBox* t = &textBoxs[i];
        DrawText(t->text, t->position.x, t->position.y, t->size, t->col);
    }
}

//----- struct slider -----
char tempS[MAX_TEXTBOX];

typedef struct{
    Vector2 position;
    int length;
    int width;
    int rangeMin;
    int rangeMax;

    int size;
    Color textCol;
    char* text;
    bool hovered;
    bool adjusting;
    bool enabled;

    Rectangle buttonRc;
    Vector2 valuePos;
    int value;

    int leftEnd;
    int offset;
} Slider;
Slider sliders[linesCount];
int sliderCount = 0;
int defaultWidth = lineH / 8;
int defaultButtonLength = lineH / 4;
int padding = lineH / 4;

Slider* addSliderAtPosition(Vector2 position, int length, int rangeMin, int rangeMax, int defaultValue, char* text) {
    if(sliderCount >= linesCount) {
        return NULL;
    }

    int leftEnd = position.x + defaultSize * 0.5 * strlen(text) + padding;
    sliders[sliderCount++] = (Slider) {
        .position = position,
        .size = defaultSize,
        .textCol = BLACK,
        .text = text,
        .rangeMin = rangeMin,
        .rangeMax = rangeMax,
        .length = length,
        .leftEnd = leftEnd,
        .buttonRc = (Rectangle) { leftEnd + (defaultValue - rangeMin) * length / (rangeMax - rangeMin),
                                  position.y + defaultSize/2 - defaultWidth/2, defaultButtonLength, defaultWidth},
        .value = defaultValue,
        .width = defaultWidth,
        .enabled = true,
    };

    return sliders + sliderCount - 1;
}

Slider* addSlider(int line, int length, int rangeMin, int rangeMax, int defaultValue, char* text) {
    if(lineIsUsed[line]) {
        return NULL;
    }
  
    lineIsUsed[line] = true;
    Slider* s = addSliderAtPosition((Vector2) { displayW, line * lineH}, length, rangeMin, rangeMax, defaultValue, text);
    if(s == NULL) {
        lineIsUsed[line] = false;
        return NULL;
    }
    return s;
}

void updateSilders() {
    Vector2 mouse = GetMousePosition();
    for(int i = 0; i < sliderCount; i++) {
        Slider* s = &sliders[i];
        if(!s->enabled) {
            continue;
        }
        s->hovered = CheckCollisionPointRec(mouse, s->buttonRc);
        if(s->hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            s->adjusting = true;
            s->offset = mouse.x - s->buttonRc.x;
        }
        if(s->adjusting && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) s->adjusting = false;
        if(s->adjusting) {
            int crr_x = mouse.x - s->offset;
            if(crr_x < s->leftEnd) crr_x = s->leftEnd;
            if(crr_x > s->leftEnd + s->length) crr_x = s->leftEnd + s->length;
            s->buttonRc.x = crr_x;
            s->value = (s->buttonRc.x - s->leftEnd) * (s->rangeMax - s->rangeMin) / s->length + s->rangeMin;
        }
    }
}

void drawSliders() {
    for(int i = 0; i < sliderCount; i++) {
        Slider* s = &sliders[i];
        if(!s->enabled) {
            continue;
        }
        DrawText(s->text, s->position.x, s->position.y, s->size, s->textCol);
        DrawRectangleRec((Rectangle) {s->leftEnd, s->buttonRc.y, s->length + s->buttonRc.width, s->width}, GRAY);
        DrawRectangleRec(s->buttonRc, BLACK);
        snprintf(tempS, MAX_TEXTBOX, "%d", s->value);
        DrawText(tempS, s->leftEnd + s->length + padding * 2, s->position.y, s->size, s->textCol);
    }
}

//----- struct tileMap -----

typedef struct {
    Vector2 position;
    int xMax;
    int yMax;
    int xSpan;
    int ySpan;

    float xStep;
    float yStep;

    Color** colorMap;
    bool showNet;
    Color netCol;
    int netThick;

    bool enablePainter;
} TileMap;

TileMap tm;

TileMap* enableTileMap(Vector2 position, int xMax, int yMax, int xSpan, int ySpan, Color defaultColor) {
    Color** colorMap = malloc(sizeof(Color*) * xMax);
    for(int i = 0; i < xMax; i++) {
        colorMap[i] = malloc(sizeof(Color) * yMax);
        for(int j = 0; j < yMax; j++) {
            colorMap[i][j] = defaultColor;
        }
    }
    tm.colorMap = colorMap;
    tm.position = position;
    tm.xMax = xMax;
    tm.yMax = yMax;
    tm.xSpan = xSpan;
    tm.ySpan = ySpan;
    tm.xStep = (float) xSpan / xMax;
    tm.yStep = (float) ySpan / yMax;
    tm.showNet = false;
    tm.enablePainter = false;
    return &tm;
}

void drawTileMap() {
    for(int i = 0; i < tm.xMax; i++) {
        for(int j = 0; j < tm.yMax; j++) {
            DrawRectangleRec((Rectangle){tm.position.x + i * tm.xStep, tm.position.y + j * tm.yStep, tm.xStep, tm.yStep}, tm.colorMap[i][j]);
        }
    }

    if(tm.showNet) {
        for(int i = 0; i <= tm.xMax; i++) {
            DrawLineEx((Vector2){tm.position.x + i * tm.xStep, tm.position.y},
                       (Vector2){tm.position.x + i * tm.xStep, tm.position.y + tm.ySpan}, tm.netThick, BLACK);
        }
        for(int i = 0; i <= tm.yMax; i++) {
            DrawLineEx((Vector2){tm.position.x, tm.position.y + i * tm.yStep},
                       (Vector2){tm.position.x + tm.xSpan, tm.position.y + i * tm.yStep}, tm.netThick, BLACK);
        }
    }
}

void updateTileMap(Color color) {
    if(!tm.enablePainter) {
        return;
    }
    if(!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        return;
    }

    Vector2 mouse = GetMousePosition();
    float fx = (mouse.x - tm.position.x) / tm.xStep;
    float fy = (mouse.y - tm.position.y) / tm.yStep;
    if(fx < 0 || fx >= tm.xMax || fy < 0 || fy >= tm.yMax) {
        return;
    }
    tm.colorMap[(int)fx][(int)fy] = color;
}

void freeTileMap() {
    for(int i = 0; i < tm.xMax; i++) {
        free(tm.colorMap[i]);
    }
    free(tm.colorMap);
}

//----- struct colorPicker -----

typedef struct {
    Color chosen;
    int line;

    Button* body;
    Slider* SliderR;
    Slider* SliderG;
    Slider* SliderB;

    bool choosing;
    bool listAbove;
    Rectangle backgroundRec;
    Rectangle previewRec;
} ColorPicker;

ColorPicker colorPickers[linesCount];
int colorPickerCount = 0;

ColorPicker* addColorPicker(int line, Color defaultColor) {
    if(lineIsUsed[line]) {
        return NULL;
    }
    if(buttonCount >= linesCount || sliderCount + 3 > linesCount) {
        return NULL;
    }

    lineIsUsed[line] = true;

    int panelW = (screenW - displayW) - padding * 2;
    int panelX = displayW + padding;
    int rowH = defaultSize;
    int trackLength = panelW - defaultSize * 3 - padding * 3;
    int listHeight = rowH * 3;
    bool listAbove = line * lineH + lineH + listHeight > screenH;

    Rectangle backgroundRec = listAbove
        ? (Rectangle) { panelX, line * lineH - listHeight, panelW, listHeight}
        : (Rectangle) { panelX, line * lineH + lineH, panelW, listHeight};

    Button* body = addButtonAtPosition((Vector2) { panelX, line * lineH},
                                       (Vector2) { lineH * 0.7, lineH * 0.7}, "");
    Slider* SliderR = addSliderAtPosition((Vector2) { backgroundRec.x, backgroundRec.y + rowH * 0},
                                          trackLength, 0, 255, defaultColor.r, "R:");
    Slider* SliderG = addSliderAtPosition((Vector2) { backgroundRec.x, backgroundRec.y + rowH},
                                          trackLength, 0, 255, defaultColor.g, "G:");
    Slider* SliderB = addSliderAtPosition((Vector2) { backgroundRec.x, backgroundRec.y + rowH * 2},
                                          trackLength, 0, 255, defaultColor.b, "B:");

    colorPickers[colorPickerCount++] = (ColorPicker) {
        .chosen = defaultColor,
        .line = line,
        .body = body,
        .SliderR = SliderR,
        .SliderG = SliderG,
        .SliderB = SliderB,
        .choosing = false,
        .listAbove = listAbove,
        .backgroundRec = backgroundRec,
        .previewRec = body->rc,
    };

    ColorPicker* p = colorPickers + colorPickerCount - 1;
    p->SliderR->enabled = false;
    p->SliderG->enabled = false;
    p->SliderB->enabled = false;

    return p;
}

void updateColorPickers() {
    Vector2 mouse = GetMousePosition();
    for(int i = 0; i < colorPickerCount; i++) {
        ColorPicker* p = &colorPickers[i];
        p->chosen = (Color) { p->SliderR->value, p->SliderG->value, p->SliderB->value, 255 };
        p->body->Ncol = p->chosen;
        p->body->Hcol = ColorBrightness(p->chosen, 0.25f);
        p->body->Ccol = ColorBrightness(p->chosen, -0.25f);
        p->body->Tcol = (p->chosen.r * 299 + p->chosen.g * 587 + p->chosen.b * 114) / 1000 > 128 ? BLACK : WHITE;

        if(!p->choosing) {
            if(p->body->clicked) {
                p->choosing = true;
                p->SliderR->enabled = true;
                p->SliderG->enabled = true;
                p->SliderB->enabled = true;
            }
        } else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(mouse, p->backgroundRec)) {
            p->choosing = false;
            p->SliderR->enabled = false;
            p->SliderG->enabled = false;
            p->SliderB->enabled = false;
        }
    }
}

void drawColorPickers() {
    for(int i = 0; i < colorPickerCount; i++) {
        ColorPicker* p = &colorPickers[i];
        if(!p->choosing) {
            continue;
        }
        DrawRectangleRec(p->backgroundRec, RAYWHITE);
        DrawRectangleLinesEx(p->backgroundRec, lineH * 0.025f, BLACK);
    }
}

//----- main -----

int main(void) {
    InitWindow(screenW, screenH, "work table: a simple window");
    SetTargetFPS(FPS);

    int number1 = 0;
    

    int tileSize = lineH / 4;
    TileMap* tileMap = enableTileMap((Vector2){0, 0}, displayW / tileSize, screenH / tileSize, displayW, screenH, WHITE);
    tileMap->showNet = true;
    tileMap->netCol = BLACK;
    tileMap->netThick = 1;
    tileMap->enablePainter = true;
    Button* someButton = addButton((Vector2){lineH * 3, lineH * 0.7}, "hello", 0);
    addtextBox(1, BLACK, format("number1: %d", number1));
    Slider* someSlider = addSlider(2, (screenW - displayW) / 4, 1, 120, 60, "FPS:");
    ColorPicker* someColor = addColorPicker(10, (Color){120, 160, 200, 255});

    bool shouldgreet = false;

    while(!WindowShouldClose()) {
        updateButtons();
        updateColorPickers();
        updateTextBoxs();
        updateSilders();
        updateTileMap(someColor->chosen);

        
        number1++;
        if(someButton->clicked) shouldgreet = !shouldgreet;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        drawTileMap();
        
        DrawLine(displayW, 0, displayW, screenH, BLACK);

        if(shouldgreet) DrawText("world", padding, padding, (int)(defaultSize * 0.5), BLACK);

        SetTargetFPS(someSlider->value);

        drawColorPickers();
        drawButtons();
        drawTextBoxs();
        drawSliders();
    
        EndDrawing();
    }

    CloseWindow();
  
    return 0;
}
