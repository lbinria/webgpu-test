struct Vertex { 
    pos: vec4<f32> 
};
@group(0) @binding(0) var<storage, read_write> vertices: array<Vertex>;

// Mesh is represented with two read-only arrays
@group(0) @binding(1) var<storage, read> points: array<vec4<f32>>;
@group(0) @binding(2) var<storage, read> offsets: array<i32>;
@group(0) @binding(3) var<storage, read> facets: array<i32>;
@group(0) @binding(4) var<uniform> nFacets: i32;

@compute @workgroup_size(1)
fn main(@builtin(global_invocation_id) gid: vec3<u32>) {

    // If global invocation id is greater than number of facets, exit
    if (gid.x >= cast<u32>(nFacets)) { return; }

    // Global invocation id
    let f = gid.x;
    
    // Below, set buffer !
    // 3 vertices per triangle
    // Each vertex is aligned in vector [a,b,c,d,e,f] first tri: (a,b,c); second tri: (d,e,f), etc.
    vertices[i * 3 + 0].pos = vec4<f32>(xoff + -size, yoff + -size, 0.0, 1.0);
    vertices[i * 3 + 1].pos = vec4<f32>(xoff + size, yoff + -size, 0.0, 1.0);
    vertices[i * 3 + 2].pos = vec4<f32>(xoff + 0.0, yoff + size, 0.0, 1.0);


}