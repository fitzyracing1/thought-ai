# thought-ai

Copyright (c) 2026 Joshua Almeida

This is a small C program that turns a thought-loop sketch into runnable code. It reads one digit from standard input, pauses briefly, and stops with no further output when the digit is 0 or the input is not a digit. For a positive digit it prints hello, then for that many steps it reproduces a core state, runs a stub Ada handshake, and drives each cap point through a kernel built around that point. The mxv routine rotates a block of points with a 2D matrix-times-vector. Each step is printed live on a thought bar, and a draft of the results is summarized into a conclusion line.

Build with `make`, which runs `gcc -Wall -Wextra -std=c99 -lm`. Run it by typing a single digit and pressing Enter, for example `echo 3 | ./thought-ai`. The Ada connection is only a stub handshake, not a real Ada or network link.

It is licensed under the MIT License.
