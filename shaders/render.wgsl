struct Vertex { @location(0) pos: vec4<f32>; @location(1) uv: vec2<f32>; };

@vertex
fn vs_main(@location(0) pos: vec4<f32>, @location(1) uv: vec2<f32>) -> @builtin(position) vec4<f32> {
    return pos;
}

@fragment
fn fs_main() -> @location(0) vec4<f32> {
    return vec4<f32>(0.9, 0.6, 0.2, 1.0);
}