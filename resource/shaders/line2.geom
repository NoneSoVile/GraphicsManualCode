#version 330 core
precision highp float;
layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 4) out;
uniform uint usingGeoShader = 0u;
uniform vec2 u_viewport_size;
uniform vec2 u_aa_radius;
in vec4 v_col[];
in float v_line_width[];
out vec4 g_col;
out  float g_line_width;
out  float g_line_length;
out  float g_u;
out  float g_v;
void main()
{
    if(usingGeoShader == 0u)
    {
        for(int i = 0; i < 3; i++) { // You used triangles, so it's always 3
            gl_Position = gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
        return;
    }
    float u_width        = u_viewport_size[0];
    float u_height       = u_viewport_size[1];
    float u_aspect_ratio = u_height / u_width;
    vec2 ndc_a = gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;
    vec2 ndc_b = gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;
    vec2 ndc_c = gl_in[2].gl_Position.xy / gl_in[2].gl_Position.w;

    vec2 line_vector = ndc_b - ndc_a;
    vec2 line_vector2 = ndc_c - ndc_b;

    vec2 viewport_line_vector = line_vector * u_viewport_size;
    vec2 viewport_line_vector2 = line_vector2 * u_viewport_size;

    vec2 dir = normalize(vec2( line_vector.x, line_vector.y * u_aspect_ratio ));
    vec2 dir2 = normalize(vec2( line_vector2.x, line_vector2.y * u_aspect_ratio ));

    float line_width_a     = max( 1.0, v_line_width[0] ) + u_aa_radius[0];
    float line_width_b     = max( 1.0, v_line_width[1] ) + u_aa_radius[0];
    float line_width_c     = max( 1.0, v_line_width[2] ) + u_aa_radius[0];

    float extension_length = 0.0;

    float line_length      = length( viewport_line_vector ) + 2.0 * extension_length;
    float line_length2      = length( viewport_line_vector2 ) + 2.0 * extension_length;

    vec2 normal    = vec2( -dir.y, dir.x );
    vec2 normal_a  = vec2( line_width_a/u_width, line_width_a/u_height ) * normal;
    vec2 normal_b  = vec2( line_width_b/u_width, line_width_b/u_height ) * normal;

    vec2 normal2    = vec2( -dir2.y, dir2.x );
    vec2 normal_b2  = vec2( line_width_b/u_width, line_width_b/u_height ) * normal2;
    vec2 normal_c2  = vec2( line_width_c/u_width, line_width_c/u_height ) * normal2;

    vec2 extension = vec2( extension_length / u_width, extension_length / u_height ) * dir;
    g_col = vec4( v_col[0].rgb, v_col[0].a * min( v_line_width[0], 1.0f ) );

    g_u = line_width_b;
    g_v = -line_length * 0.5;
    g_line_width = line_width_b;
    g_line_length = line_length * 0.5;
    gl_Position = vec4( (ndc_b + normal_b + extension) * gl_in[1].gl_Position.w, gl_in[1].gl_Position.zw );
    EmitVertex();
    g_u = -line_width_b;
    g_v = -line_length * 0.5;
    g_line_width = line_width_b;
    g_line_length = line_length * 0.5;
    gl_Position = vec4( (ndc_b - normal_b + extension) * gl_in[1].gl_Position.w, gl_in[1].gl_Position.zw );
    EmitVertex();

    g_col = vec4( v_col[0].rgb, v_col[0].a * min( v_line_width[0], 1.0f ) );
    
    g_u = line_width_c;
    g_v = -line_length2 * 0.5;
    g_line_width = line_width_c;
    g_line_length = line_length2 * 0.5;
    gl_Position = vec4( (ndc_c + normal_c2 + extension) * gl_in[2].gl_Position.w, gl_in[2].gl_Position.zw );
    EmitVertex();
    g_u = -line_width_c;
    g_v = -line_length2 * 0.5;
    g_line_width = line_width_c;
    g_line_length = line_length2 * 0.5;
    gl_Position = vec4( (ndc_c - normal_c2 + extension) * gl_in[2].gl_Position.w, gl_in[2].gl_Position.zw );
    EmitVertex();
    EndPrimitive();
};