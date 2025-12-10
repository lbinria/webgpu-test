struct Vertex { pos: vec4<f32>; uv: vec2<f32>; };
@group(0) @binding(0) var<storage, read_write> vertices: array<Vertex>;

@compute @workgroup_size(1)
fn main(@builtin(global_invocation_id) gid: vec3<u32>) {
    // we will emit 3 vertices (triangle) at indices 0..2
    if (gid.x >= 3u) { return; }
    let i = gid.x;
    if (i == 0u) {
        vertices[0].pos = vec4<f32>(-0.5, -0.5, 0.0, 1.0);
        vertices[0].uv  = vec2<f32>(0.0, 0.0);
    } else if (i == 1u) {
        vertices[1].pos = vec4<f32>( 0.5, -0.5, 0.0, 1.0);
        vertices[1].uv  = vec2<f32>(1.0, 0.0);
    } else {
        vertices[2].pos = vec4<f32>( 0.0,  0.5, 0.0, 1.0);
        vertices[2].uv  = vec2<f32>(0.5, 1.0);
    }
}