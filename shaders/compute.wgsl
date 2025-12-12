struct Vertex { pos: vec4<f32> };
@group(0) @binding(0) var<storage, read_write> vertices: array<Vertex>;

@compute @workgroup_size(1)
fn main(@builtin(global_invocation_id) gid: vec3<u32>) {
    let nRows = 10u;
    let nCols = 10u;
    let nTri = nRows * nCols;

    if (gid.x >= nTri) { return; }

    let i = gid.x;
    
    let size : f32 = 1. / f32(nRows);

    // Compute current column / row number
    let col : f32 = f32(i) % f32(nCols);
    let row : f32 = floor(f32(i) / f32(nRows));

    // Compute offsets (x, y) according to i
    let off : f32 = -1. + size;
    let xoff : f32 = off + col * size * 2.;
    let yoff : f32 = off + row * size * 2.;

    // Set buffer
    vertices[i * 3 + 0].pos = vec4<f32>(xoff + -size, yoff + -size, 0.0, 1.0);
    vertices[i * 3 + 1].pos = vec4<f32>(xoff + size, yoff + -size, 0.0, 1.0);
    vertices[i * 3 + 2].pos = vec4<f32>(xoff + 0.0, yoff + size, 0.0, 1.0);


}