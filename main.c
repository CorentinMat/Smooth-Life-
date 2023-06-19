#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#define HEIGHT 100
#define WIDTH 100
#define level_count (sizeof(level) / sizeof(level[0]) - 1)

float ra = 11;

// float alpha = 0.147;
char level[] = " .-=coaA@#";
float grid[WIDTH][HEIGHT] = {0};
float grid_diff[WIDTH][HEIGHT] = {0};

float b1 = 0.278;
float b2 = 0.365;
float d1 = 0.267;
float d2 = 0.445;
// delta time
float dt = 0.05f;
float alpha_m = 0.147;
float alpha_n = 0.028;
float rand_float(void)
{
    return (float)rand() / (float)RAND_MAX;
}
float sigma1(float x, float a, float alpha)
{
    return 1.0f / (1.0f + expf(-(x - a) * 4 / alpha));
}
float sigma2(float x, float a, float b)
{
    return (sigma1(x, a, alpha_n) * (1 - sigma1(x, b, alpha_n)));
}

float sigmam(float x, float y, float m)
{
    return (x * (1 - sigma1(m, 0.5f, alpha_m)) + y * sigma1(m, 0.5f, alpha_m));
}

float s(float n, float m)
{
    return sigma2(n, sigmam(b1, d1, m), sigmam(b2, d2, m));
}
void random_grid(void)
{
    float cx = WIDTH / 4;
    float cy = HEIGHT / 4;

    int radius = WIDTH / 4;
    for (size_t i = 0; i < WIDTH / 2; i++)
    {
        for (size_t j = 0; j < HEIGHT / 2; j++)
        {
            int x = i + WIDTH / 2 - cx;
            int y = j + HEIGHT / 2 - cy;
            grid[x][y] = rand_float();
        }
    }
}
int emod(int a, int b)
{
    return (a % b + b) % b;
}
void display_grid(float grid[WIDTH][HEIGHT])
{
    for (size_t i = 0; i < WIDTH; i++)
    {
        for (size_t j = 0; j < HEIGHT; j++)
        {
            char x = level[(int)(grid[i][j] * (level_count - 1))];
            fputc(x, stdout);
            fputc(x, stdout);
        }
        fputc('\n', stdout);
    }
}
void compute_grid_diff(void)
{
    for (int cy = 0; cy < HEIGHT; ++cy)
    {
        for (int cx = 0; cx < WIDTH; ++cx)
        {
            float m = 0, M = 0;
            float n = 0, N = 0;
            float ri = ra / 3;

            for (int dy = -(ra - 1); dy <= (ra - 1); ++dy)
            {
                for (int dx = -(ra - 1); dx <= (ra - 1); ++dx)
                {
                    int x = emod(cx + dx, WIDTH);
                    int y = emod(cy + dy, HEIGHT);
                    if (dx * dx + dy * dy <= ri * ri)
                    {
                        m += grid[y][x];
                        M += 1;
                    }
                    else if (dx * dx + dy * dy <= ra * ra)
                    {
                        n += grid[y][x];
                        N += 1;
                    }
                }
            }
            m /= M;
            n /= N;
            float q = s(n, m);
            grid_diff[cy][cx] = 2 * q - 1;
        }
    }
}
void clamp(float *x, float l, float h)
{
    if (*x < l)
        *x = l;
    if (*x > h)
        *x = h;
}
void apply_grid_diff(void)
{
    for (size_t i = 0; i < WIDTH; i++)
    {
        for (size_t j = 0; j < HEIGHT; j++)
        {
            grid[i][j] += dt * grid_diff[i][j];
            clamp(&grid[i][j], 0, 1);
        }
    }
}
int main(void)
{

    srand(time(0));
    random_grid();

    for (;;)
    {
        compute_grid_diff();
        apply_grid_diff();
        display_grid(grid);
    }

    return 0;
}