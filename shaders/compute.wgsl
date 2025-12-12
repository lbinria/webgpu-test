struct Vertex { pos: vec4<f32> };
@group(0) @binding(0) var<storage, read_write> vertices: array<Vertex>;

@compute @workgroup_size(1)
fn main(@builtin(global_invocation_id) gid: vec3<u32>) {
    if (gid.x >= 1) { return; }

    let i = gid.x;
    vertices[0].pos = vec4<f32>(-0.5, -0.5, 0.0, 1.0);
    vertices[1].pos = vec4<f32>(0.5, -0.5, 0.0, 1.0);
    vertices[2].pos = vec4<f32>(0.0,  0.5, 0.0, 1.0);
}