#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "play.h"

#define MAX_NOTES 100

typedef struct {
    int freq;
    int duration;
} tone_t;

tone_t recorded_tones[MAX_NOTES];
int recorded_count = 0;

void mock_tonew(int freq, int duration)
{
    if (recorded_count < MAX_NOTES) {
        recorded_tones[recorded_count].freq = freq;
        recorded_tones[recorded_count].duration = duration;
        recorded_count++;
    }
}

void reset_recorder()
{
    recorded_count = 0;
    memset(recorded_tones, 0, sizeof(recorded_tones));
}

bool check_tone(int index, int expected_freq, int expected_duration)
{
    if (index >= recorded_count) {
        printf("  FAIL: Expected tone at index %d, but only %d tones "
               "recorded\n",
                index, recorded_count);
        return false;
    }
    if (recorded_tones[index].freq != expected_freq ||
            recorded_tones[index].duration != expected_duration) {
        printf("  FAIL: Tone %d mismatch. Expected freq=%d, dur=%d; Got "
               "freq=%d, dur=%d\n",
                index, expected_freq, expected_duration,
                recorded_tones[index].freq, recorded_tones[index].duration);
        return false;
    }
    return true;
}

int main()
{
    int failed = 0;

    // Test Case 1: Basic Scale
    printf("Test 1: Basic Scale (CDE)\n");
    reset_recorder();
    play("CDE", mock_tonew);
    if (!check_tone(0, 262, 1000))
        failed++;
    if (!check_tone(1, 294, 1000))
        failed++;
    if (!check_tone(2, 330, 1000))
        failed++;
    if (recorded_count != 3) {
        printf("  FAIL: Expected 3 tones, got %d\n", recorded_count);
        failed++;
    }

    // Test Case 2: Octaves
    printf("Test 2: Octave Changes (O3C O5C)\n");
    reset_recorder();
    play("O3CO5C", mock_tonew);
    if (!check_tone(0, 131, 1000))
        failed++;
    if (!check_tone(1, 523, 1000))
        failed++;

    // Test Case 3: Durations
    printf("Test 3: Durations (C4 D8)\n");
    reset_recorder();
    play("C4D8", mock_tonew);
    if (!check_tone(0, 262, 250))
        failed++;
    if (!check_tone(1, 294, 125))
        failed++;

    // Test Case 4: Accidentals (C# F+ G-)
    printf("Test 4: Accidentals (C# F+ G-)\n");
    reset_recorder();
    // 'E' followed by 'b' is not supported as 'b' is not '#', '+', or '-'
    // 'F+' and 'G-' are supported
    play("C#F+G-", mock_tonew);
    if (!check_tone(0, 277, 1000))
        failed++;  // C#4
    if (!check_tone(1, 370, 1000))
        failed++;  // F#4
    if (!check_tone(2, 370, 1000))
        failed++;  // Gb4 (F#4)

    // Test Case 5: Rests
    printf("Test 5: Rests (CRD)\n");
    reset_recorder();
    play("CRD", mock_tonew);
    if (!check_tone(0, 262, 1000))
        failed++;
    if (!check_tone(1, 0, 1000))
        failed++;
    if (!check_tone(2, 294, 1000))
        failed++;

    // Test Case 6: Error Handling (Invalid Char)
    printf("Test 6: Error Handling (Invalid Char)\n");
    // "CD X E"
    // C (0), D (1), ' ' (2) -> Error at 2
    int pos = play("CD X E", mock_tonew);
    if (pos != 2) {
        printf("  FAIL: Expected error at pos 2, got %d\n", pos);
        failed++;
    }

    if (failed == 0) {
        printf("\nALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("\n%d TESTS FAILED!\n", failed);
        return 1;
    }
}
