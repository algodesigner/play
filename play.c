/*  
 * play v0.1
 * A simplified music play command parser implementation.
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Vlad Shurupov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <ctype.h>
#include <math.h>
#include <stdbool.h>

//#define DEBUG 1

#ifdef DEBUG
#include <stdio.h>
#endif

/**
 * Play parser primary state enum.
 */
enum parsing_state {
    COMMAND,
    OCTAVE_NUMBER
};

/**
 * Converts a note to the respective frequency.
 * 
 * @param note a note to be converted; should be in the range ["A", "G"]
 * @param half a half-tone; '-' for flat, '+' or '#' for sharp
 * @param octave an octave (numbered from 1)
 */
static int note_to_freq(char note, char half, int octave)
{
    double freq = 0.0;
    double base_freq = 55.0; // A1 reference frequency
    int half_steps = 0;
    switch (note) {
        case 'C': half_steps = -21; break;
        case 'D': half_steps = -19; break;
        case 'E': half_steps = -17; break;
        case 'F': half_steps = -16; break;
        case 'G': half_steps = -14; break;
        case 'A': half_steps = -12; break;
        case 'B': half_steps = -10; break;
        default: break;
    }
    switch (half) {
        case '#': case '+': half_steps++; break;
        case '-': half_steps--; break;
        default: break;
    }
    freq = pow(2.0, (double)half_steps / 12.0) * base_freq;
    if (octave > 1) {
        freq *= pow(2.0, octave);
    } else if (octave < 1) {
        freq /= pow(2.0, -octave);
    }
    return round(freq);
}

/**
 * Plays a specified string of music notes.
 * 
 * @param a string of music notes.
 * @param tonew a pointer to the tone processor function.
 */
static void play_note(char note, char half_tone, int octave, int duration,
        void (*tonew)(int, int))
{
    note = toupper(note);
#ifdef DEBUG
    printf("Play '%c', half_tone='%c', octave=%d, duration=%d\n", note,
            half_tone, octave, duration);
#endif
    if (duration >= 1)
        tonew(note != 'R' ? note_to_freq(note, half_tone, octave) : 0,
            1000 / duration);
}

/**
 * Plays the specified string of music notes.
 * 
 * @param s the string of music notes.
 * @param tonew a pointer to the tone processor function.
 */
int play(const char *s, void (*tonew)(int, int))
{
    enum parsing_state state;
    int pos = 0;
    int octave = 4;
    char note;
    int duration = 1;
    int half_tone = 0;
    bool pending_note = false;
    bool length_specified = false;

#ifdef DEBUG
    printf("PLAY \"%s\"\n", s);
#endif

    for (state = COMMAND; *s; s++, pos++) {
        switch (state) {
        case COMMAND:
            if ((*s >= 'a' && *s <= 'g') || (*s >= 'A' && *s <= 'G') ||
                    *s == 'r' || *s == 'R')
            {
                if (pending_note)
                    play_note(note, half_tone, octave, duration, tonew);
                note = *s;
                half_tone = 0;
                duration = 1;
                pending_note = true;
                length_specified = false;
                if (!*(s + 1))
                    play_note(note, half_tone, octave, duration, tonew);
            } else if (*s == 'o' || *s == 'O') {
                if (pending_note) {
                    play_note(note, half_tone, octave, duration, tonew);
                    pending_note = false;
                    length_specified = false;
                }
                state = OCTAVE_NUMBER;
            } else if (pending_note) {
                if (!length_specified) {
                    if ((*s == '#' || *s == '+' || *s == '-') && note != 'r'
                            && note != 'R')
                    {
                        half_tone = *s;
                    } else if (isdigit(*s)) {
                        duration = *s - '0';
                        length_specified = true;
                    } else
                        return pos;
                } else if (isdigit(*s)) {
                    duration = duration * 10 + *s - '0';
                    play_note(note, half_tone, octave, duration, tonew);
                    pending_note = false;
                    length_specified = false;
                    state = COMMAND;
                } else 
                    return pos;
            }
            break;
        case OCTAVE_NUMBER:
            if (isdigit(*s)) {
                octave = *s - '0';
                state = COMMAND;            
            } else
                return pos;
            break;
        }
    }
    return -1;
}
