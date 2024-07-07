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


    float length = length(delta);
    vec2 direction = delta / length;
    // Calculate the perpendicular distance from the fragment to the line
    vec2 perpDirection = vec2(-direction.y, direction.x);
    float distanceToLine = abs(dot(fragPos - p1, perpDirection));

    // Calculate the parallel distance along the line
    float parallelDistance = dot(fragPos - p1, direction);

    // Determine if the fragment is within the line segment
    bool withinLineSegment = parallelDistance >= 0.0 && parallelDistance <= length;

    // Calculate the alpha value for anti-aliasing
    float halfLineWidth = u_lineWidth / u_resolution.x * 0.5;
    float alpha = smoothstep(halfLineWidth, 0.0, distanceToLine);

    vec4 u_lineColor = vec4(1.0, 1.0, 1.0, 1.0); ;  // The color of the line
    vec4 u_bgColor =  vec4(0.0, 0.0, 0.0, 1.0);;    // The background color

    // Mix the line color with the background color based on the distance
    vec4 color = mix(u_bgColor, u_lineColor, alpha);
    // Set the fragment color
    if (distance(p1, fragPos) < 0.01) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color for the first point
    } 
    else if (distance(p2, fragPos) < 0.01) {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Green color for the second point
    }
    // Set the fragment color
    else if (withinLineSegment && distanceToLine <= halfLineWidth)
    {
        FragColor = color; // Line color with anti-aliasing
    }
    else 
    {
        FragColor = u_bgColor; // Background color
    }
}