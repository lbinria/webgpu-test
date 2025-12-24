struct Vertex { pos: vec4<f32> };
@group(0) @binding(0) var<storage, read_write> vertices: array<Vertex>;

@compute @workgroup_size(1)
fn main(@builtin(global_invocation_id) gid: vec3<u32>) {
    // Hard-coded number of rows & cols
    let nRows = 10u;
    let nCols = 10u;
    let nTri = nRows * nCols;

    // If global invocation id is greater than number of triangles, exit
    if (gid.x >= nTri) { return; }

    // Global invocation id
    let i = gid.x;
    
    // Compute height size of a triangle according to number of rows
    let size : f32 = 1. / f32(nRows);

    // Compute current column / row number
    let col : f32 = f32(i) % f32(nCols);
    let row : f32 = floor(f32(i) / f32(nRows));

    // Compute offsets (x, y) according to i
    let off : f32 = -1. + size;
    let xoff : f32 = off + col * size * 2.;
    let yoff : f32 = off + row * size * 2.;

    // Below, set buffer !
    // 3 vertices per triangle
    // Each vertex is aligned in vector [a,b,c,d,e,f] first tri: (a,b,c); second tri: (d,e,f), etc.
    vertices[i * 3 + 0].pos = vec4<f32>(xoff + -size, yoff + -size, 0.0, 1.0);
    vertices[i * 3 + 1].pos = vec4<f32>(xoff + size, yoff + -size, 0.0, 1.0);
    vertices[i * 3 + 2].pos = vec4<f32>(xoff + 0.0, yoff + size, 0.0, 1.0);


}