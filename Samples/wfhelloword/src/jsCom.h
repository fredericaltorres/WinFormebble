/*
 * WinFormebble Library
 * A C# and Winform friendly library for the developing for the Pebble watch
 *
 * (C) Torres Frederic 2014
 *
 * Licence: MIT
  */
#pragma once

void jsCom_Initialize();
void jsCom_SendMessage(uint8_t key);
void jsCom_SendIntMessage(uint8_t key, uint8_t cmd);
bool jsCom_SendStringMessage(int key, char *text);