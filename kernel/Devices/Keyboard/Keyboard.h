#pragma once

#include "Arch/x86/isr.h"
#include "Arch/x86/pic.h"
#include "Devices/DebugPort/DebugPort.h"
#include "Devices/DeviceNode.h"
#include "utils/types.h"

class Keyboard : public DeviceNode
{
  public:
	static void keyboard_driver_handler(ISRContextFrame*);
	Keyboard();
	~Keyboard();

  private:
	static bool pressed_keys[3];
	static const int KBD_DATA_PORT = 0x60;
	static constexpr char asccode[58][2] = {
	    {0, 0},     {0, 0},       {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'},  {'7', '&'},
	    {'8', '*'}, {'9', '('},   {'0', ')'}, {'-', '_'}, {'=', '+'}, {8, 8},     {9, 9},     {'q', 'Q'},  {'w', 'W'},
	    {'e', 'E'}, {'r', 'R'},   {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'},  {'[', '{'},
	    {']', '}'}, {'\n', '\n'}, {0, 0},     {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'},  {'h', 'H'},
	    {'j', 'J'}, {'k', 'K'},   {'l', 'L'}, {';', ':'}, {39, 34},   {'`', '~'}, {0, 0},     {'\\', '|'}, {'z', 'Z'},
	    {'x', 'X'}, {'c', 'C'},   {'v', 'V'}, {'b', 'B'}, {'n', 'N'}, {'m', 'M'}, {',', '<'}, {'.', '>'},  {'/', '?'},
	    {0, 0},     {0, 0},       {0, 0},     {' ', ' '},
	};
};
