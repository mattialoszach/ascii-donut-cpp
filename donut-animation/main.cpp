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

    for (float theta = 0; theta < 2 * M_PI; theta += theta_spacing) {
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        for (float phi = 0; phi < 2 * M_PI; phi += phi_spacing) {
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            float circleX = R2 + R1 * cosTheta;
            float circleY = R1 * sinTheta;

            // Torus 3D parameterization
            float x = circleX * (cosB * cosPhi + sinA * sinB * sinPhi) - circleY * cosA * sinB;
            float y = circleX * (sinB * cosPhi - sinA * cosB * sinPhi) + circleY * cosA * cosB;
            float z = K2 + cosA * circleX * sinPhi + circleY * sinA;
            float ooz = 1 / z;
            
            // Computing 2d projection
            // x and y projection.  note that y is negated here, because y
            // goes up in 3D space but down on 2D displays.
            int xP = (int)(screen_width / 2 + K1 * ooz * x);
            int yP = (int)(screen_height / 2 - K1 * ooz * y * 0.5); // Adjust y scaling to fix aspect ratio
            
            // Compute Illumination
            float L = cosPhi * cosTheta * sinB - cosA * cosTheta * sinPhi - sinA * sinTheta + cosB * (cosA * sinTheta - cosTheta * sinA * sinPhi);
            
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
