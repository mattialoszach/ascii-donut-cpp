#include <iostream>
#include <cmath>
#include <cstring>

const float theta_spacing = 0.07;
const float phi_spacing = 0.02;

// Animation/Rotation (x- and z-axis)
const float A_spacing = 0.0004;
const float B_spacing = 0.0002;

// Parameters
const float R1 = 1;
const float R2 = 2;
const float K2 = 5;

// Screen size
const int screen_width = 80;
const int screen_height = 40;
// Calculate K1 based on screen size: the maximum x-distance occurs
// roughly at the edge of the torus, which is at x=R1+R2, z=0.  we
// want that to be displaced 3/8ths of the width of the screen, which
// is 3/4th of the way from the center to the side of the screen.
// screen_width*3/8 = K1*(R1+R2)/(K2+0)
// screen_width*K2*3/(8*(R1+R2)) = K1
const float K1 = screen_width * K2 * 3 / (8 * (R1 + R2));

// Compute/Render frame
void render_frame(float A, float B) {
    char output[screen_height][screen_width];
    float zbuffer[screen_height][screen_width];
    std::memset(output, ' ', sizeof(output));
    std::memset(zbuffer, 0, sizeof(zbuffer));

    float sinA = sin(A);
    float cosA = cos(A);
    float sinB = sin(B);
    float cosB = cos(B);

    for (float u = 0; u < 2 * M_PI; u += theta_spacing) {
        for (float v = -1; v <= 1; v += phi_spacing * 2) {
            float cos_u = cos(u);
            float sin_u = sin(u);
            float cos_u2 = cos(u / 2);
            float sin_u2 = sin(u / 2);
            
            float scale = 1.5;
            float x = scale * ((1 + (v / 2) * cos_u2) * cos_u);
            float y = scale * ((1 + (v / 2) * cos_u2) * sin_u);
            float z = scale * ((v / 2) * sin_u2);

            // Apply rotation A and B (as before)
            float x_rot = x * cosB + z * sinB;
            float z_rot = -x * sinB + z * cosB;
            float y_rot = y * cosA - z_rot * sinA;
            z_rot = y * sinA + z_rot * cosA;

            float ooz = 1 / (K2 + z_rot);
            int xP = (int)(screen_width / 2 + K1 * ooz * x_rot);
            int yP = (int)(screen_height / 2 - K1 * ooz * y_rot * 0.5);

            float L = sin_u2 * cos_u2 - sinA * sin_u + cosB * (cosA * sin_u2 - cos_u2 * sinA);

            if (xP >= 0 && xP < screen_width && yP >= 0 && yP < screen_height) {
                if (ooz > zbuffer[yP][xP]) {
                    zbuffer[yP][xP] = ooz;
                    int luminance_index = std::max(0, std::min(11, (int)(L * 8)));
                    output[yP][xP] = ".,-~:;=!*#$@"[luminance_index];
                }
            }
        }
    }
    std::cout << "\x1b[H"; // Move cursor to top-left
    for (int i = 0; i < screen_height; i++) {
        for (int j = 0; j < screen_width; j++) {
            std::cout << output[i][j];
        }
        std::cout << "\n";
    }
}

int main() {
    float A = 0;
    float B = 0;

    // Start animation loop
    while (true) {
        render_frame(A, B);
        A += A_spacing;
        B += B_spacing;
    }

    return 0;
}
