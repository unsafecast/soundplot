// SPDX-License-Identifier: MIT

// This is a simple program that visualizes your sound files. It accepts a raw sound file
// and it creates a window, the plots the hilbert curve for every sample, with the color 
// value being the frequency of it.
//
// This project was inspired by this video by 3Blue1Brown:
//   https://www.youtube.com/watch?v=3s7h2MHQtxc&t=147s
//
// Building: The build system doesn't require anything other than having a C compiler.
//           To bootstrap the build system, run `cc -o nobuild nobuild.c`. This will
//           create the build system executable, which you can run to build the project
//           itself.
//
//           This currently only works on Linux with raylib installed globally on the 
//           system. Windows/MacOS support is trivial to add, it's just a matter of adapting
//           the build system.
//
// To-Do List:
// - [BUG] For some reason when plotting the files, you always get a black region.
//         It seems to be related to window size. Happens when using any file of any
//         size every time.

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

int Dimension;
#define NumberOfCells (Dimension * Dimension)

typedef struct
{
    int X, Y;
} vec2i;


// The Rotate and Hilbert functions - I barely know how they work. If anyone
// can offer any help on making them more readable and documenting them,
// as I couldn't figure it out by myself (see the variable names...)
//
// I took these from https://people.math.sc.edu/Burkardt/c_src/hilbert_curve/hilbert_curve.c
// They are barely documented there, but from the given documentation I could
// rename some badly named variables. If anyone with a better understanding
// of hilbert curves reads this, please submit a pull request to fix them. Thanks.

// Rotates a quad to face the right direction in higher-order curves
void Rotate(int Length, vec2i *Quad, vec2i Idfk)
{
    if (Idfk.Y == 0)
    {
        if (Idfk.X == 1)
        {
            Quad->X = Length - 1 - Quad->X;
            Quad->Y = Length - 1 - Quad->Y;
        }

        int Temp = Quad->X;
        Quad->X = Quad->Y;
        Quad->Y = Temp;
    }
}

// Converts a 1D point into a 2D point using the Hilbert curve
vec2i Hilbert(int Coord)
{
    vec2i Idfk;
    vec2i Ret = {0};

    int T = Coord;  // TODO: What is T??
    for (int Index = 1; Index < NumberOfCells; Index *= 2)
    {
        Idfk.X = 1 & (T / 2);
        Idfk.Y = 1 & (T ^ Idfk.X);

        Rotate(Index, &Ret, Idfk);
        Ret.X = Ret.X + Index * Idfk.X;
        Ret.Y = Ret.Y + Index * Idfk.Y;
        T /= 4;
    }

    return Ret;
}

float Lerp(float A, float B, float Val)
{
    return (1.0f - Val) * A + B * Val;
}

float InverseLerp(float A, float B, float Val)
{
    return (Val - A) / (B - A);
}

// This remaps a float from a range to another range.
// For example, given a float 0.0f, the input range [-1.0f; 1.0f]
// and the output range [0.0f; 1.0f], it will remap to 0.5f, aka 
// halfway through in both ranges
float Remap(float Min1, float Max1, float Min2, float Max2, float Val)
{
    return Lerp(Min2, Max2, InverseLerp(Min1, Max1, Val));
}

void Usage()
{
    fprintf(stderr, "Usage: soundplot <path-to-raw-audio-file> <square-window-dimension-in-px>\n");
    fprintf(stderr, "Note: The number of bytes read from the given file will be equal to the window dimension squared. If your dimensions are too big for the audio file, the program won't run\n");
    exit(1);
}

int main(int ArgCount, const char **Args)
{
    Args++;
    ArgCount--;

    if (ArgCount < 2)
    {
        Usage();
    }

    const char *Path = Args[0];
    Dimension = atoi(Args[1]);
    
    if (Dimension <= 0)
    {
        Usage();
    }

    SetTraceLogLevel(LOG_NONE);
    InitWindow(Dimension, Dimension, "soundplot");

    FILE *AudioFile = fopen(Path, "rb");

    // This is the buffer which will store the audio file
    float *Buffer = malloc(sizeof(float) * NumberOfCells);
    
    // Read the first part of the file; only as much as fits on screen
    if (fread(Buffer, sizeof(float), NumberOfCells, AudioFile) != NumberOfCells)
    {
        fprintf(stderr, "Error: Image file smaller than given dimensions\n");
        exit(1);
    }
    
    fclose(AudioFile);
    
    // We create an array representing the audio file, and pre-calculate
    // the 2D position of every sample.
    vec2i *Map = malloc(sizeof(vec2i) * NumberOfCells);
    for (int Index = 0; Index < NumberOfCells; Index += 1)
    {
        Map[Index] = Hilbert(Index);
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        for (int Index = 0; Index < NumberOfCells; Index += 1)
        {
            // Remapping the frequency from [-1.0f; 1.0f] to [0.0f; 1.0f] so we can use it
            // as the color values.
            int Components = (int)(Remap(-1.0f, 1.0f, 0.0f, 1.0f, Buffer[Index]) * 255);
            DrawPixel(Map[Index].X, Map[Index].Y, (Color) {Components, Components, Components, Components});
        }
        EndDrawing();
    }

    return 0;
}
