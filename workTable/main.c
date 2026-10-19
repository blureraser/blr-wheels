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
} Button;

Button buttons[linesCount];
int buttonCount = 0;
Button* addButton(Vector2 size, char* text, int line) {
    if(lineIsUsed[line]) {
        return NULL;
    }
    lineIsUsed[line] = true;
    Rectangle rc = (Rectangle) {displayW, line * lineH, size.x, size.y};
    buttons[buttonCount++] = (Button) {
        .line = line,
        .rc = rc,
        .text = text,
        .textSize = rc.height * 0.7,
        .textPos = (Vector2){rc.x + rc.width / 2 - strlen(text) * rc.height / 4,
                             rc.y + rc.height * 0.15},
        .Ncol = BLUE,
        .Hcol = SKYBLUE,
        .Ccol = RED,
        .Tcol = BLACK
    };
    return buttons + buttonCount - 1;
}

void updateButtons() {
    Vector2 mouse = GetMousePosition();
    for(int i = 0; i < buttonCount; i++) {
        Button* b = &buttons[i];
        b->hovered = CheckCollisionPointRec(mouse, b->rc);
        b->clicked = b->hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }
}

void drawButtons() {
    for(int i = 0; i < buttonCount; i++) {
        Button* b = &buttons[i];
        Color col = b->hovered ? (b->clicked ? b->Ccol : b->Hcol) : b->Ncol;
        DrawRectangleRec(b->rc, col);
        DrawRectangleLinesEx(b->rc, 3.0f, b->Tcol);
        DrawText(b->text, b->textPos.x, b->textPos.y, b->textSize, b->Tcol);
    }
}

//----- struct textBox -----
char temp[MAX_TEXTBOX];
typedef void (*Formatter)(char* skeleton, void *value);

void formatInt(char* skeleton, void *value) {
    int n = *(int *)value;
    snprintf(temp, MAX_TEXTBOX, skeleton, n);
}
void formatBool(char* skeleton, void* value) {
    bool n = *(bool*)value;
    snprintf(temp, MAX_TEXTBOX, skeleton, n);
}
void formatDouble(char* skeleton, void *value) {
    double n = *(double*)value;
    snprintf(temp, MAX_TEXTBOX, skeleton, n);
}
void formatFloat(char* skeleton, void *value) {
    float n = *(float*)value;
    snprintf(temp, MAX_TEXTBOX, skeleton, n);
}
void formatColor(char* skeleton, void *value) {
    Color col = *(Color*)value;
    snprintf(temp, MAX_TEXTBOX, skeleton, col.r, col.g, col.b, col.a);
}
void formatVector2(char* skeleton, void *value) {
    Vector2 vec = *(Vector2*)value;
    snprintf(temp, MAX_TEXTBOX, skeleton, vec.x, vec.y);
}
void formatRect(char* skeleton, void *value) {
    Rectangle rc = *(Rectangle*)value;
    snprintf(temp, MAX_TEXTBOX, skeleton, rc.x, rc.y, rc.width, rc.height);
}
void formatChar(char* skeleton, void* value) {
    char n = *(char*)value;
    snprintf(temp, MAX_TEXTBOX, skeleton, n);
}
void formatString(char* skeleton, void *value) {
    char* str = *(char**)value;
    snprintf(temp, MAX_TEXTBOX, skeleton, str);
}

#define format(typeV)                                       \
    ({                                                    \
        void formatP(char* skeleton, void* value) {        \
            typeV v = *(typeV*) value;                      \
            snprintf(temp, MAX_TEXTBOX, skeleton, v);  \
        }                                                   \
        formatP;                                            \
    })


typedef struct{
    char text[MAX_TEXTBOX];
    char* skeleton;
    Vector2 position;
    int size;
    Color col;

    void *valuePtr;
    Formatter formatter;
} textBox;

textBox textBoxs[linesCount];
int textBoxCount = 0;
int defaultSize = lineH * 0.7;

textBox* addtextBox(int line, char* skeleton, void* valuePtr, Color col, Formatter fmtr) {
    if(lineIsUsed[line]) {
        return NULL;
    }
  
    lineIsUsed[line] = true;
    textBoxs[textBoxCount++] = (textBox) {
        .position = (Vector2) { displayW, line * lineH},
        .skeleton = skeleton,
        .valuePtr = valuePtr,
        .formatter = fmtr,
        .size = defaultSize,
        .col = col,
    };
    return textBoxs + textBoxCount - 1;
}
void updateTextBoxs() {
    for(int i = 0; i < textBoxCount; i++) {
        textBox* t = &textBoxs[i];
        t->formatter(t->skeleton, t->valuePtr);
        strcpy(t->text, temp);
    }
}
void drawTextBoxs() {
    for(int i = 0; i < textBoxCount; i++) {
        textBox* t = &textBoxs[i];
        DrawText(t->text, t->position.x, t->position.y, t->size, t->col);
    }
}

//----- main -----

int main(void) {
    InitWindow(screenW, screenH, "work table: a simple window");
    SetTargetFPS(FPS);

    int number1 = 0;
  
    Button* someButton = addButton((Vector2){240, 60}, "hello", 0);
    addtextBox(1, "number1: %d", &number1, BLACK, format(int));

    bool shouldgreet = false;

    while(!WindowShouldClose()) {
        updateButtons();
        updateTextBoxs();

        
        number1++;
        if(someButton->clicked) shouldgreet = !shouldgreet;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawLine(displayW, 0, displayW, screenH, BLACK);

        if(shouldgreet) DrawText("world", 20, 20, 30, BLACK);
    
        drawButtons();
        drawTextBoxs();
    
        EndDrawing();
    }

    CloseWindow();
  
    return 0;
}
