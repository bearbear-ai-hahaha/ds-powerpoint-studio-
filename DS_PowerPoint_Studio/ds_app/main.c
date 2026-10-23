#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAX_SLIDES 12
#define TITLE_LEN 48
#define BODY_LEN 180
#define SAVE_DIR "/data/ds-powerpoint"
#define SAVE_FILE "/data/ds-powerpoint/presentation.dspres"

typedef struct {
    char title[TITLE_LEN];
    char body[BODY_LEN];
    int bg;
} Slide;

static PrintConsole topConsole, glConsole;
static Slide slides[MAX_SLIDES];
static int slide_count = 3;
static int current = 0;
static int view_mode = 0;
static int edit_mode = 0; /* 0 none, 1 title, 2 body */
static int cursor = 0;
static const char *bg_names[] = {"navy","black","white","blue","green","red","purple","yellow"};
static const int bg_colors[] = {RGB15(2,5,15), RGB15(0,0,0), RGB15(31,31,31), RGB15(3,12,29), RGB15(2,18,7), RGB15(25,3,3), RGB15(16,3,22), RGB15(31,27,0)};
static const char charset[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,!?'-:/";

static void set_defaults(void) {
    memset(slides, 0, sizeof(slides));
    strcpy(slides[0].title, "DS PowerPoint Studio");
    strcpy(slides[0].body, "Make a presentation on your Nintendo DS!");
    slides[0].bg = 0;
    strcpy(slides[1].title, "How to use it");
    strcpy(slides[1].body, "D-pad: change slide\nA: edit title\nB: edit body\nY: save project\nX: presentation mode");
    slides[1].bg = 3;
    strcpy(slides[2].title, "Export to PowerPoint");
    strcpy(slides[2].body, "Save the .dspres project, copy it to a computer or phone, then run export_pptx.py to create a real .pptx file.");
    slides[2].bg = 6;
}

static char *edit_buffer(void) {
    return edit_mode == 1 ? slides[current].title : slides[current].body;
}
static int edit_capacity(void) {
    return edit_mode == 1 ? TITLE_LEN : BODY_LEN;
}
static void redraw(void) {
    int bg = bg_colors[slides[current].bg & 7];
    consoleClear();
    iprintf("DS POWERPOINT STUDIO      %02d/%02d\n", current+1, slide_count);
    iprintf("--------------------------------\n");
    iprintf("%s\n\n", slides[current].title);
    iprintf("%s\n\n", slides[current].body);
    iprintf("--------------------------------\n");
    iprintf("%s  |  %s\n", view_mode ? "PRESENTING" : "EDIT MODE", edit_mode ? (edit_mode==1 ? "EDIT TITLE" : "EDIT BODY") : "READY");
    iprintf("\nD-PAD: slides   A: title\nB: body      X: view/edit\nY: save      L/R: cursor\n");
    iprintf("START: help    SELECT: new slide\n");
    iprintf("\nTouch: tap title/body to edit\n");
    iprintf("Tap bottom: prev / next / save\n");
    /* Draw a simple lower-screen preview. */
    consoleSelect(&glConsole);
    consoleClear();
    iprintf("SLIDE PREVIEW\n\n");
    iprintf("[%s]\n\n", bg_names[slides[current].bg & 7]);
    iprintf("%s\n\n", slides[current].title);
    iprintf("%s\n", slides[current].body);
    iprintf("\n\n");
    iprintf("< PREV      NEXT >\n");
    iprintf("EDIT TITLE  EDIT BODY\n");
    iprintf("SAVE        BG COLOR\n");
    consoleSelect(&topConsole);
    (void)bg;
}

static void save_project(void) {
    if (!fatInitDefault()) {
        iprintf("\nFAT not available: check PiCO firmware/DLDI.\n");
        return;
    }
    mkdir("/data", 0777);
    mkdir(SAVE_DIR, 0777);
    FILE *f = fopen(SAVE_FILE, "w");
    if (!f) { iprintf("\nCannot write project file.\n"); return; }
    /* Minimal JSON escaping: replace line breaks and quotes safely. */
    fprintf(f, "{\n\"title\":\"DS PowerPoint Studio\",\n\"slides\":[\n");
    for (int i=0; i<slide_count; i++) {
        if (i) fprintf(f, ",\n");
        fprintf(f, "{\"title\":\"");
        for (const char *p=slides[i].title; *p; p++) {
            if (*p=='"' || *p=='\\') fputc('\\',f);
            if (*p=='\n' || *p=='\r') { fputc(' ',f); continue; }
            fputc(*p,f);
        }
        fprintf(f, "\",\"body\":\"");
        for (const char *p=slides[i].body; *p; p++) {
            if (*p=='"' || *p=='\\') fputc('\\',f);
            if (*p=='\n' || *p=='\r') { fputc('\\',f); fputc('n',f); continue; }
            fputc(*p,f);
        }
        fprintf(f, "\",\"bg\":\"%s\"}", bg_names[slides[i].bg & 7]);
    }
    fprintf(f, "]\n}\n");
    fclose(f);
    iprintf("\nSaved: %s\n", SAVE_FILE);
}

static void add_slide(void) {
    if (slide_count >= MAX_SLIDES) return;
    current = slide_count++;
    memset(&slides[current], 0, sizeof(Slide));
    strcpy(slides[current].title, "New slide");
    strcpy(slides[current].body, "Press A or B to edit text.");
    slides[current].bg = 0;
}

static void text_edit_step(int delta) {
    char *buf = edit_buffer();
    int len = strlen(buf), cap = edit_capacity();
    if (len >= cap-1) len = cap-2;
    if (cursor < 0) cursor = 0;
    if (cursor > len) cursor = len;
    if (delta > 0) {
        const char *p = strchr(charset, buf[cursor]);
        int idx = p ? (int)(p-charset) : 0;
        buf[cursor] = charset[(idx + 1) % ((int)sizeof(charset)-1)];
    } else {
        const char *p = strchr(charset, buf[cursor]);
        int idx = p ? (int)(p-charset) : 0;
        idx = (idx + (int)sizeof(charset)-2) % ((int)sizeof(charset)-1);
        buf[cursor] = charset[idx];
    }
}

int main(void) {
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleDemoInit();
    consoleInit(&topConsole, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&glConsole, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
    consoleSelect(&topConsole);
    set_defaults();
    redraw();

    while (1) {
        scanKeys();
        int down = keysDown();
        int held = keysHeld();
        touchPosition touch;
        if (down & KEY_LEFT) { if (current > 0) current--; edit_mode=0; redraw(); }
        if (down & KEY_RIGHT) { if (current < slide_count-1) current++; edit_mode=0; redraw(); }
        if (down & KEY_A) { edit_mode=1; cursor=strlen(slides[current].title); redraw(); }
        if (down & KEY_B) { edit_mode=2; cursor=strlen(slides[current].body); redraw(); }
        if (down & KEY_X) { view_mode=!view_mode; edit_mode=0; redraw(); }
        if (down & KEY_Y) { save_project(); redraw(); }
        if (down & KEY_SELECT) { add_slide(); redraw(); }
        if (down & KEY_START) { iprintf("\nControls: A/B edit; UP/DOWN change character; L/R move cursor; Y saves; SELECT adds slide.\n"); }
        if (edit_mode && (down & KEY_UP)) { text_edit_step(1); redraw(); }
        if (edit_mode && (down & KEY_DOWN)) { text_edit_step(-1); redraw(); }
        if (edit_mode && (down & KEY_L)) { if (cursor>0) cursor--; redraw(); }
        if (edit_mode && (down & KEY_R)) { if (cursor < (int)strlen(edit_buffer())) cursor++; redraw(); }
        if (down & KEY_TOUCH) {
            touchRead(&touch);
            if (touch.py > 170) {
                if (touch.px < 80) { if (current>0) current--; edit_mode=0; }
                else if (touch.px > 175) { if (current<slide_count-1) current++; edit_mode=0; }
                else if (touch.py > 205 && touch.px < 125) { edit_mode=1; cursor=strlen(slides[current].title); }
                else if (touch.py > 205 && touch.px < 205) { edit_mode=2; cursor=strlen(slides[current].body); }
                else if (touch.px > 200) { slides[current].bg=(slides[current].bg+1)&7; }
                redraw();
            } else if (touch.py < 100) { edit_mode=1; cursor=strlen(slides[current].title); redraw(); }
            else { edit_mode=2; cursor=strlen(slides[current].body); redraw(); }
        }
        swiWaitForVBlank();
        (void)held;
    }
    return 0;
}
