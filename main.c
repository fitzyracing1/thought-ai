#define _POSIX_C_SOURCE 200809L

/*
 * thought-ai: read one digit, pause, then run a small thought loop.
 * Copyright (c) 2026 Joshua Almeida
 */

#include <stdio.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_STEPS 9
#define N_CAPS 3
#define BLOCK_SIZE 2
#define PAUSE_NS 250000000L

typedef struct {
    double x;
    double y;
} Vec2;

typedef struct {
    Vec2 origin;
    double angle;
} Kernel;

typedef struct {
    int generation;
    int step;
    Vec2 centroid;
} Core;

typedef struct {
    int step;
    int cap;
    double angle_deg;
    Vec2 result;
} DraftEntry;

/* Pause briefly after reading the digit. */
static void brief_pause(void)
{
    struct timespec req;

    req.tv_sec = 0;
    req.tv_nsec = PAUSE_NS;
    nanosleep(&req, NULL);
}

/*
 * Stub Ada connection: a simple handshake only.
 * This is not a real Ada language or network link.
 */
static int ada_connection_stub(int step)
{
    int token = (step * 17 + 3) % 97;
    int ack = (token * 5 + 1) % 97;

    return ack;
}

/* mxv: matrix-times-vector rotation of a 2D point by angle radians. */
static Vec2 mxv(Vec2 v, double angle)
{
    double c = cos(angle);
    double s = sin(angle);
    Vec2 out;

    out.x = c * v.x - s * v.y;
    out.y = s * v.x + c * v.y;
    return out;
}

/* Rebuild (reproduce) the core state for this step from the current points. */
static Core reproduce_core(int step, const Vec2 *points, int count)
{
    Core core;
    int i;

    core.generation = step;
    core.step = step;
    core.centroid.x = 0.0;
    core.centroid.y = 0.0;
    for (i = 0; i < count; i++) {
        core.centroid.x += points[i].x;
        core.centroid.y += points[i].y;
    }
    if (count > 0) {
        core.centroid.x /= (double)count;
        core.centroid.y /= (double)count;
    }
    return core;
}

/* Build a kernel around a cap point for this step. */
static Kernel make_kernel(Vec2 cap, int step, int cap_index)
{
    Kernel k;

    k.origin = cap;
    k.angle = ((double)(step + cap_index) * 30.0) * (M_PI / 180.0);
    return k;
}

/*
 * Drive a cap point: rotate a block of points around the kernel origin
 * with mxv, then return the block centroid in world space.
 */
static Vec2 drive_cap_point(const Kernel *k, const Vec2 *block, int count)
{
    Vec2 sum;
    int i;

    sum.x = 0.0;
    sum.y = 0.0;
    for (i = 0; i < count; i++) {
        Vec2 rel;
        Vec2 rot;
        Vec2 world;

        rel.x = block[i].x - k->origin.x;
        rel.y = block[i].y - k->origin.y;
        rot = mxv(rel, k->angle);
        world.x = rot.x + k->origin.x;
        world.y = rot.y + k->origin.y;
        sum.x += world.x;
        sum.y += world.y;
    }
    if (count > 0) {
        sum.x /= (double)count;
        sum.y /= (double)count;
    }
    return sum;
}

/* Print one live thought-bar line and flush so it appears immediately. */
static void thought_bar(int step, int steps, int cap, double angle_deg, Vec2 result)
{
    printf("[thought] step %d/%d | cap %d/%d | rotate %.1f deg | result (%.3f, %.3f)\n",
           step, steps, cap, N_CAPS, angle_deg, result.x, result.y);
    fflush(stdout);
}

static int read_one_digit(void)
{
    int c = getchar();

    if (c < '0' || c > '9') {
        return 0;
    }
    return c - '0';
}

int main(void)
{
    static const Vec2 caps[N_CAPS] = {
        {1.0, 0.0},
        {0.0, 1.0},
        {-1.0, 0.0}
    };
    static const Vec2 block[BLOCK_SIZE] = {
        {2.0, 0.5},
        {1.5, 1.0}
    };

    int n;
    int step;
    int cap;
    int ada_ack;
    Core core;
    DraftEntry draft[MAX_STEPS * N_CAPS];
    int draft_count = 0;
    double sum_x = 0.0;
    double sum_y = 0.0;
    Vec2 conclusion;

    n = read_one_digit();
    brief_pause();
    if (n <= 0) {
        return 0;
    }

    printf("hello\n");
    fflush(stdout);

    for (step = 1; step <= n; step++) {
        core = reproduce_core(step, caps, N_CAPS);
        ada_ack = ada_connection_stub(step);
        printf("[thought] step %d/%d | core reproduced gen %d centroid (%.3f, %.3f) | ada handshake stub ack %d\n",
               step, n, core.generation, core.centroid.x, core.centroid.y, ada_ack);
        fflush(stdout);

        for (cap = 0; cap < N_CAPS; cap++) {
            Kernel k = make_kernel(caps[cap], step, cap);
            Vec2 result = drive_cap_point(&k, block, BLOCK_SIZE);
            double angle_deg = k.angle * (180.0 / M_PI);
            DraftEntry entry;

            thought_bar(step, n, cap + 1, angle_deg, result);

            entry.step = step;
            entry.cap = cap + 1;
            entry.angle_deg = angle_deg;
            entry.result = result;
            draft[draft_count++] = entry;
        }
    }

    printf("--- draft ---\n");
    for (step = 0; step < draft_count; step++) {
        printf("step %d cap %d angle %.1f deg result (%.3f, %.3f)\n",
               draft[step].step, draft[step].cap,
               draft[step].angle_deg, draft[step].result.x, draft[step].result.y);
        sum_x += draft[step].result.x;
        sum_y += draft[step].result.y;
    }

    conclusion.x = (draft_count > 0) ? (sum_x / (double)draft_count) : 0.0;
    conclusion.y = (draft_count > 0) ? (sum_y / (double)draft_count) : 0.0;
    printf("--- conclusion ---\n");
    printf("The thought is working in the draft: %d step(s), %d cap result(s), mean (%.3f, %.3f).\n",
           n, draft_count, conclusion.x, conclusion.y);

    return 0;
}
