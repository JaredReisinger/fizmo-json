#ifndef FIZMO_JSON_SCREEN_H
#define FIZMO_JSON_SCREEN_H

// The standard Z-machine, Glk, and fizmo models of the screen all assume a
// fixed-size, rectangular buffer... but while that's useful for the status
// window, it actually complicates the regular, buffered story window.  To that
// end, we track the buffered and unbuffered variations completely separately.
//
// The buffered text is treated much like HTML, with block (line), and span
// flow.  Its content is *never* placed into the unbuffered grid, under the
// assumption that no game would explicitly set the cursor into the midst of a
// buffered run in order to update the text.
//
// A full-screen-sized unbuffered window is tracked purely for the purposes of
// capturing the status line(s), and/or any explicitly layed-out text (like the
// quote in the beginning of Graham Nelson's "Curses").  We use a screen size
// of 100x255: 100 characters wide allows us to easily infer left/center/right
// alignment, and 255 lines is, as per the Z-machine spec section 8.4.1,
// "infinte height".

#define SCREEN_WIDTH  100
#define SCREEN_HEIGHT 255


extern struct z_screen_interface bot_screen;

#endif // FIZMO_JSON_SCREEN_H
