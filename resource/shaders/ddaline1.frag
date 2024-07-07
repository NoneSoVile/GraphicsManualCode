#version 330 core

out vec4 FragColor;

uniform vec2 u_resolution; // The resolution of the screen
uniform vec2 u_point1;     // The first point of the line (x1, y1)
uniform vec2 u_point2;     // The second point of the line (x2, y2)
uniform float u_lineWidth; // The width of the line

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    
    // Convert fragment coordinates to the range [0, 1]
    vec2 p1 = u_point1 / u_resolution;
    vec2 p2 = u_point2 / u_resolution;
    vec2 fragPos = fragCoord / u_resolution;
    
    // Calculate the distance from the fragment to the line
    float a = distance(p1, p2);
    float b = distance(fragPos, p1);
    float c = distance(fragPos, p2);
    
    float s = (a + b + c) / 2.0;
    float area = sqrt(s * (s - a) * (s - b) * (s - c));
    float distanceToLine = 2.0 * area / a;
    
    // Set the fragment color based on the distance to the line
    if(distance(p1, fragPos) < 0.01){
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);    
    } 
    else if(distance(p2, fragPos) < 0.01){
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);    
    }
    else if (distanceToLine < u_lineWidth / u_resolution.x) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color for the line
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black color for the background
    }
}
