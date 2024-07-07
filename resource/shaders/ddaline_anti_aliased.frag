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

    // Calculate the differences
    vec2 delta_pixel = u_point2 - u_point1;
    float len_pixel = max(abs(delta_pixel.x), abs(delta_pixel.y));
    vec2 delta = p2 - p1;
    float len = max(abs(delta_pixel.x), abs(delta_pixel.y));
    float eps = 1.0 / len;
    vec2 increment = eps * delta;
    int steps = int(len_pixel);

    // Initialize the starting point
    vec2 currentPos = p1;// + vec2(0.5) / u_resolution;
    
    // Bresenham's algorithm variables
    float Err = 0.0;
    float deltaErr = delta.y / delta.x;
    bool onLine = false;


    // Calculate the distance of the fragment to the line
    float minDistance = 1.0;
    for (int i = 0; i <= steps; ++i) {
        float distanceToLine = distance(fragPos, currentPos);
        minDistance = min(minDistance, distanceToLine);

        currentPos += increment;

        if (Err > 0) {
            currentPos.x += increment.x;
            Err -= delta.y;
        } else {
            currentPos.y += increment.y;
            Err += delta.x;
        }
    }

    // Calculate the alpha value based on the distance to the line
    float alpha = smoothstep(u_lineWidth / u_resolution.x, 0.0, minDistance);

    // Set the fragment color based on whether it's on the line
    if(distance(p1, fragPos) < 0.01){
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);    
    } 
    else if(distance(p2, fragPos) < 0.01){
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);    
    }
    else {
        FragColor =  mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), alpha);
    }
}
