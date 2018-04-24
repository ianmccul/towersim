// implementation of kbhit function

#ifndef KBHIT_H
#define KBHIT_H

// initialize keyboard for raw more with no echo
void init_keyboard();

// put the keyboard state back where it was
void close_keyboard();

// returns true if a key has been pressed, false otherwise.
// call readch() to reset this to false.
bool kbhit();

// read the key, or block if no key has been pressed yet
int readch();

#endif
