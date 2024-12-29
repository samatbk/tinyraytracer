#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define WIDTH 512
#define HEIGHT 512

#define PI 3.141592

typedef uint8_t u8;
typedef int32_t i32;
typedef double f64;

typedef struct {
  f64 x;
  f64 y;
  f64 z;
} v3;

typedef struct {
  i32 x;
  i32 y;
  i32 z;
} v3i;

typedef u8 Buffer[WIDTH * HEIGHT][3];

typedef struct {
  v3 center;
  f64 radius;
} Sphere;

typedef struct {
  v3 position;
  f64 intensity;
} Light;

f64 dot(v3 a, v3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
v3 diff(v3 a, v3 b) {
  return (v3){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
}
v3 add(v3 a, v3 b) {
  return (v3){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}
f64 mag(v3 a) { return sqrt(a.x * a.x + a.y * a.y + a.z * a.z); }
v3 norm(v3 a) {
  f64 m = mag(a);
  return (v3){a.x / m, a.y / m, a.z / m};
}
v3 scale(v3 a, f64 s) { return (v3){a.x * s, a.y * s, a.z * s}; }

bool ray_intersect(Sphere sphere, v3 origin, v3 dir, v3 *point) {
  v3 co = diff(sphere.center, origin);
  f64 t = dot(co, dir);

  v3 proj = scale(dir, t);

  v3 closest = add(origin, proj);

  f64 dist = mag(diff(closest, sphere.center));

  if (dist <= sphere.radius) {

    f64 x = sqrt(sphere.radius * sphere.radius - dist * dist);

    *point = scale(dir, t - x);
    return true;
  }

  return false;
}

int main(void) {
  FILE *file = fopen("output.ppm", "w");
  fprintf(file, "P3\n%d %d\n255\n", WIDTH, HEIGHT);

  Buffer buffer;

  f64 fov = PI / 2;

  v3 origin = (v3){0, 0, 0};

  Sphere sphere = (Sphere){(v3){0, 2, -5}, 2};
  Sphere sphere_2 = (Sphere){(v3){-1, 1, -3}, 3};

  for (size_t j = 0; j < HEIGHT; j++) {
    for (size_t i = 0; i < WIDTH; i++) {
      f64 x = (2 * (i + 0.5) / (f64)WIDTH - 1) * tan(fov / 2.0) * WIDTH /
              (f64)HEIGHT;

      f64 y = -(2 * (j + 0.5) / (float)HEIGHT - 1) * tan(fov / 2.0);

      v3 dir = (v3){x, y, -1};
      dir = norm(dir);

      v3 p;

      if (ray_intersect(sphere_2, origin, dir, &p)) {

        v3 light_dir = (v3){3, 3, -4.5};
        light_dir = diff(light_dir, p);
        light_dir = norm(light_dir);

        f64 intensity = dot(norm(light_dir), norm(p));

        if (intensity <= 0) {
          intensity = 0;
        }

        v3 diffuse_color = (v3){100, 150, 100};

        buffer[i + j * WIDTH][0] = diffuse_color.x * intensity;
        buffer[i + j * WIDTH][1] = diffuse_color.y * intensity;
        buffer[i + j * WIDTH][2] = diffuse_color.z * intensity;
      }

      if (ray_intersect(sphere, origin, dir, &p)) {

        v3 light_dir = (v3){3, 3, -4.5};
        light_dir = diff(light_dir, p);
        light_dir = norm(light_dir);

        f64 intensity = dot(norm(light_dir), norm(p));

        if (intensity <= 0) {
          intensity = 0;
        }

        v3 diffuse_color = (v3){100, 150, 100};

        buffer[i + j * WIDTH][0] = diffuse_color.x * intensity;
        buffer[i + j * WIDTH][1] = diffuse_color.y * intensity;
        buffer[i + j * WIDTH][2] = diffuse_color.z * intensity;
      }
    }
  }

  for (size_t i = 0; i < WIDTH * HEIGHT; i += 1) {
    int x = i % WIDTH;
    int y = i / WIDTH;

    fprintf(file, "%d %d %d ", buffer[i][0], buffer[i][1], buffer[i][2]);

    if (x == WIDTH - 1) {
      fprintf(file, "\n");
    }
  }

  fclose(file);

  return 0;
}
