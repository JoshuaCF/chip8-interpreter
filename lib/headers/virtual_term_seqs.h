#pragma once

// A library designed simply to format and write ANSI escape sequences to a buffer
// Functionality intentionally kept simple and scope narrow -- meant to be used to
// build more advanced controls.
//
// Based off of the information here:
// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

#include <stddef.h>

#include "term_color_defs.h"

int bufWriteCursorMoveToOrigin(char* out, size_t bufsz);
int bufWriteCursorMoveTo(char* out, size_t bufsz, unsigned int line, unsigned int col);
int bufWriteCursorMoveUp(char* out, size_t bufsz, unsigned int lines);
int bufWriteCursorMoveDown(char* out, size_t bufsz, unsigned int lines);
int bufWriteCursorMoveRight(char* out, size_t bufsz, unsigned int cols);
int bufWriteCursorMoveLeft(char* out, size_t bufsz, unsigned int cols);
int bufWriteCursorMoveDownToLeft(char* out, size_t bufsz, unsigned int lines);
int bufWriteCursorMoveUpToLeft(char* out, size_t bufsz, unsigned int lines);
int bufWriteCursorMoveToCol(char* out, size_t bufsz, unsigned int col);

int bufWriteCursorGetPosition(char* out, size_t bufsz);
int bufWriteCursorMoveOnceUpWithScroll(char* out, size_t bufsz);
int bufWriteCursorSavePosition(char* out, size_t bufsz);
int bufWriteCursorRestorePosition(char* out, size_t bufsz);

int bufWriteCursorSetVisible(char* out, size_t bufsz);
int bufWriteCursorSetInvisible(char* out, size_t bufsz);

int bufWriteDisplayEraseFromCursor(char* out, size_t bufsz);
int bufWriteDisplayEraseToCursor(char* out, size_t bufsz);
int bufWriteDisplayEraseAll(char* out, size_t bufsz);
int bufWriteDisplayEraseSavedLines(char* out, size_t bufsz); // I don't know what this does?
int bufWriteDisplayEraseLineFromCursor(char* out, size_t bufsz);
int bufWriteDisplayEraseLineToCursor(char* out, size_t bufsz);
int bufWriteDisplayEraseLine(char* out, size_t bufsz);

int bufWriteDisplaySave(char* out, size_t bufsz);
int bufWriteDisplayRestore(char* out, size_t bufsz);

int bufWriteDisplayEnterAltBuffer(char* out, size_t bufsz);
int bufWriteDisplayLeaveAltBuffer(char* out, size_t bufsz);

int bufWriteStyleResetModes(char* out, size_t bufsz);
int bufWriteStyleSetBold(char* out, size_t bufsz);
int bufWriteStyleResetBold(char* out, size_t bufsz);
int bufWriteStyleSetDim(char* out, size_t bufsz);
int bufWriteStyleResetDim(char* out, size_t bufsz);
int bufWriteStyleSetItalics(char* out, size_t bufsz);
int bufWriteStyleResetItalics(char* out, size_t bufsz);
int bufWriteStyleSetUnderline(char* out, size_t bufsz);
int bufWriteStyleResetUnderline(char* out, size_t bufsz);
int bufWriteStyleSetBlinking(char* out, size_t bufsz);;
int bufWriteStyleResetBlinking(char* out, size_t bufsz);
int bufWriteStyleSetInverse(char* out, size_t bufsz);
int bufWriteStyleResetInverse(char* out, size_t bufsz);
int bufWriteStyleSetHidden(char* out, size_t bufsz);
int bufWriteStyleResetHidden(char* out, size_t bufsz);
int bufWriteStyleSetStrikethrough(char* out, size_t bufsz);
int bufWriteStyleResetStrikethrough(char* out, size_t bufsz);
int bufWriteStyleSetColor(char* out, size_t bufsz, enum TermColorStyle style);
int bufWriteStyleSetForegroundRGB(char* out, size_t bufsz, unsigned char r, unsigned char g, unsigned char b);
int bufWriteStyleSetBackgroundRGB(char* out, size_t bufsz, unsigned char r, unsigned char g, unsigned char b);
