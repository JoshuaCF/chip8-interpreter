#pragma once


// POSIX/Windows agnostic functions for controlling the terminal
// Includes things such as cursor positioning, colors, input options,
// window title, etc..
//
// A small bit of setup still needs to be OS specific. In Linux, a
// FILE* for which terminal to write to is needed. In Windows, a
// HANDLE is required instead. These will be `typedef`'d under `Terminal`
// and this header provides no OS agnostic way of getting the needed
// FILE*/HANDLE, as it could come from nearly anywhere and is out of
// the scope of this lib.
//
// Depending on whether or not WINDOWS is defined, term_ctrl.c will
// use primarily virtual terminal sequences (and some POSIX stdlib
// stuff) or entirely Win32 calls (requires Windows SDK, of course)
// Win32 calls are used in favor of virtual terminal sequences for 
// performance. Windows Terminal parses the sequences quite slowly.
//
// Actions are executed by queuing them into a `TermCtrlQueue`
// structure, then executing the batched actions. If few commands need
// to be executed, this overhead must still be paid. The queue will
// do its best to execute atomically and quickly.

#include <stdint.h>
#include <stdio.h>

#include "term_color_defs.h"
#include "queue.h"

#ifndef WINDOWS
typedef FILE* Terminal;
#else
typedef HANDLE Terminal;
#endif

struct TermCtrlQueue {
	Terminal term;
	struct Queue action_queue;
};

struct TermCtrlQueue TermCtrlQueue_new(Terminal term);
void TermCtrlQueue_exec(struct TermCtrlQueue* queue);
void TermCtrlQueue_free(struct TermCtrlQueue* queue);

// Queue actions
void queueTermCtrlWriteString(struct TermCtrlQueue* queue, char* str);
void queueTermCtrlWriteChar(struct TermCtrlQueue* queue, char c);

void queueTermCtrlCursorMoveToOrigin(struct TermCtrlQueue* queue);
void queueTermCtrlCursorMoveTo(struct TermCtrlQueue* queue, unsigned int line, unsigned int col);
void queueTermCtrlCursorMoveUp(struct TermCtrlQueue* queue, unsigned int lines);
void queueTermCtrlCursorMoveDown(struct TermCtrlQueue* queue, unsigned int lines);
void queueTermCtrlCursorMoveRight(struct TermCtrlQueue* queue, unsigned int cols);
void queueTermCtrlCursorMoveLeft(struct TermCtrlQueue* queue, unsigned int cols);
void queueTermCtrlCursorMoveDownToLeft(struct TermCtrlQueue* queue, unsigned int lines);
void queueTermCtrlCursorMoveUpToLeft(struct TermCtrlQueue* queue, unsigned int lines);
void queueTermCtrlCursorMoveToCol(struct TermCtrlQueue* queue, unsigned int col);

void queueTermCtrlCursorGetPosition(struct TermCtrlQueue* queue);
void queueTermCtrlCursorMoveOnceUpWithScroll(struct TermCtrlQueue* queue);
void queueTermCtrlCursorSavePosition(struct TermCtrlQueue* queue);
void queueTermCtrlCursorRestorePosition(struct TermCtrlQueue* queue);

void queueTermCtrlCursorSetVisible(struct TermCtrlQueue* queue);
void queueTermCtrlCursorSetInvisible(struct TermCtrlQueue* queue);

void queueTermCtrlDisplayEraseFromCursor(struct TermCtrlQueue* queue);
void queueTermCtrlDisplayEraseToCursor(struct TermCtrlQueue* queue);
void queueTermCtrlDisplayEraseAll(struct TermCtrlQueue* queue);
void queueTermCtrlDisplayEraseSavedLines(struct TermCtrlQueue* queue);
void queueTermCtrlDisplayEraseLineFromCursor(struct TermCtrlQueue* queue);
void queueTermCtrlDisplayEraseLineToCursor(struct TermCtrlQueue* queue);
void queueTermCtrlDisplayEraseLine(struct TermCtrlQueue* queue);

void queueTermCtrlDisplaySave(struct TermCtrlQueue* queue);
void queueTermCtrlDisplayRestore(struct TermCtrlQueue* queue);

void queueTermCtrlDisplayEnterAltBuffer(struct TermCtrlQueue* queue);
void queueTermCtrlDisplayLeaveAltBuffer(struct TermCtrlQueue* queue);

void queueTermCtrlStyleResetModes(struct TermCtrlQueue* queue);
void queueTermCtrlStyleSetBold(struct TermCtrlQueue* queue);
void queueTermCtrlStyleResetBold(struct TermCtrlQueue* queue);
void queueTermCtrlStyleSetDim(struct TermCtrlQueue* queue);
void queueTermCtrlStyleResetDim(struct TermCtrlQueue* queue);
void queueTermCtrlStyleSetItalics(struct TermCtrlQueue* queue);
void queueTermCtrlStyleResetItalics(struct TermCtrlQueue* queue);
void queueTermCtrlStyleSetUnderline(struct TermCtrlQueue* queue);
void queueTermCtrlStyleResetUnderline(struct TermCtrlQueue* queue);
void queueTermCtrlStyleSetBlinking(struct TermCtrlQueue* queue);;
void queueTermCtrlStyleResetBlinking(struct TermCtrlQueue* queue);
void queueTermCtrlStyleSetInverse(struct TermCtrlQueue* queue);
void queueTermCtrlStyleResetInverse(struct TermCtrlQueue* queue);
void queueTermCtrlStyleSetHidden(struct TermCtrlQueue* queue);
void queueTermCtrlStyleResetHidden(struct TermCtrlQueue* queue);
void queueTermCtrlStyleSetStrikethrough(struct TermCtrlQueue* queue);
void queueTermCtrlStyleResetStrikethrough(struct TermCtrlQueue* queue);
void queueTermCtrlStyleSetColor(struct TermCtrlQueue* queue, enum TermColorStyle style);
void queueTermCtrlStyleSetForegroundRGB(struct TermCtrlQueue* queue, uint8_t r, uint8_t g, uint8_t b);
void queueTermCtrlStyleSetBackgroundRGB(struct TermCtrlQueue* queue, uint8_t r, uint8_t g, uint8_t b);
