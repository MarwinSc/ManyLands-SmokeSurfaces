#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;

out VS_OUT {
    vec4 Color;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mvp;

struct vec5{
    float x;
    float y;
    float z;
    float w;
    float e;
};

struct mat5{
    vec5 x;
    vec5 y;
    vec5 z;
    vec5 w;
    vec5 e;
};

uniform vec5 translate;
uniform vec5 scale;

mat5 get4DProjectionMatrix(float r, float t, float d, float n, float f);
vec5 multiplyVectorByMat5(vec5 vector, mat5 matrix);
mat5 createMat5(mat4 matrix);
mat5 multiplyMat5ByMat5(mat5 matrix1, mat5 matrix2);
vec5 addVec5(vec5 v1, vec5 v2);
vec5 multiplyVec5(vec5 v1, vec5 v2);

void main()
{
    vs_out.Color = aColor;
    
    vec5 vert = vec5(aPos.x, aPos.y, aPos.z, aPos.w, 1.0);

    vert = addVec5(vert, translate);
    vert = multiplyVec5(vert, scale);

    vert = vec5(vert.x, vert.y, vert.z, vert.w - 550.0, vert.e);

    float fov = 30.0 * (3.141592653589793238463/ 180);
    mat5 projection_4D = get4DProjectionMatrix(fov, fov, fov, 1.0, 10.0);
    vert = multiplyVectorByMat5(vert, projection_4D);

    vec4 Position4D = vec4(vert.x / vert.e, vert.y / vert.e, vert.z / vert.e, vert.w / vert.e);
    Position4D = mvp * Position4D;

    gl_Position = Position4D;

    vec3 Position3D = vec3(Position4D.x / Position4D.w, Position4D.y / Position4D.w, Position4D.z / Position4D.w);

    //gl_Position = mvp * vec4(Position3D, 1.0);
    //gl_Position = vec4(Position3D,1.0);
}

vec5 addVec5(vec5 v1, vec5 v2){
    return vec5(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w, v1.e + v2.e);
}

vec5 multiplyVec5(vec5 v1, vec5 v2){
    return vec5(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w, v1.e * v2.e);
}

mat5 getIdentityMatrix(){
    vec5 v1 = vec5(1.0, 0.0, 0.0, 0.0, 0.0);
    vec5 v2 = vec5(0.0, 1.0, 0.0, 0.0, 0.0);
    vec5 v3 = vec5(0.0, 0.0, 1.0, 0.0, 0.0);
    vec5 v4 = vec5(0.0, 0.0, 0.0, 1.0, 0.0);
    vec5 v5 = vec5(0.0, 0.0, 0.0, 0.0, 1.0);
    return mat5(v1,v2,v3,v4,v5);
}

mat5 get4DProjectionMatrix(float r, float t, float d, float n, float f){
    vec5 v1 = vec5(n/r, 0.0, 0.0, 0.0, 0.0);
    vec5 v2 = vec5(0.0, n/t, 0.0, 0.0, 0.0);
    vec5 v3 = vec5(0.0, 0.0, n/d, 0.0, 0.0);
    vec5 v4 = vec5(0.0, 0.0, 0.0, -(f + n) / (f - n), -(2.0 * f * n) / (f - n));
    vec5 v5 = vec5(0.0, 0.0, 0.0, -1.0, 0.0);
    return mat5(v1,v2,v3,v4,v5);
}

vec5 multiplyVectorByMat5(vec5 vector, mat5 matrix){
    float x = matrix.x.x * vector.x + matrix.y.x * vector.y + matrix.z.x * vector.z + matrix.w.x * vector.w + matrix.e.x * vector.e;
    float y = matrix.x.y * vector.x + matrix.y.y * vector.y + matrix.z.y * vector.z + matrix.w.y * vector.w + matrix.e.y * vector.e;
    float z = matrix.x.z * vector.x + matrix.y.z * vector.y + matrix.z.z * vector.z + matrix.w.z * vector.w + matrix.e.z * vector.e;
    float w = matrix.x.w * vector.x + matrix.y.w * vector.y + matrix.z.w * vector.z + matrix.w.w * vector.w + matrix.e.w * vector.e;
    float e = matrix.x.e * vector.x + matrix.y.e * vector.y + matrix.z.e * vector.z + matrix.w.e * vector.w + matrix.e.e * vector.e;
    return vec5(x,y,z,w,e);
}

//creates homogeneous 5x5 matrix from vec4
mat5 createMat5(mat4 matrix){
    vec5 x = vec5(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0], 0.0);
    vec5 y = vec5(matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1], 0.0);
    vec5 z = vec5(matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2], 0.0);
    vec5 w = vec5(matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3], 0.0);
    vec5 e = vec5(0.0, 0.0, 0.0, 0.0, 1.0);
    return mat5(x,y,z,w,e);
}

mat5 multiplyMat5ByMat5(mat5 matrix1, mat5 matrix2){
    float x = matrix1.x.x * matrix2.x.x + matrix1.x.y * matrix2.y.x + matrix1.x.z * matrix2.z.x + matrix1.x.w * matrix2.w.x + matrix1.x.e * matrix2.e.x;
    float y = matrix1.x.x * matrix2.x.y + matrix1.x.y * matrix2.y.y + matrix1.x.z * matrix2.z.y + matrix1.x.w * matrix2.w.y + matrix1.x.e * matrix2.e.y;
    float z = matrix1.x.x * matrix2.x.z + matrix1.x.y * matrix2.y.z + matrix1.x.z * matrix2.z.z + matrix1.x.w * matrix2.w.z + matrix1.x.e * matrix2.e.z;
    float w = matrix1.x.x * matrix2.x.w + matrix1.x.y * matrix2.y.w + matrix1.x.z * matrix2.z.w + matrix1.x.w * matrix2.w.w + matrix1.x.e * matrix2.e.w;
    float e = matrix1.x.x * matrix2.x.e + matrix1.x.y * matrix2.y.e + matrix1.x.z * matrix2.z.e + matrix1.x.w * matrix2.w.e + matrix1.x.e * matrix2.e.e;
    vec5 v1 = vec5(x,y,z,w,e);

    x = matrix1.y.x * matrix2.x.x + matrix1.y.y * matrix2.y.x + matrix1.y.z * matrix2.z.x + matrix1.y.w * matrix2.w.x + matrix1.y.e * matrix2.e.x;
    y = matrix1.y.x * matrix2.x.y + matrix1.y.y * matrix2.y.y + matrix1.y.z * matrix2.z.y + matrix1.y.w * matrix2.w.y + matrix1.y.e * matrix2.e.y;
    z = matrix1.y.x * matrix2.x.z + matrix1.y.y * matrix2.y.z + matrix1.y.z * matrix2.z.z + matrix1.y.w * matrix2.w.z + matrix1.y.e * matrix2.e.z;
    w = matrix1.y.x * matrix2.x.w + matrix1.y.y * matrix2.y.w + matrix1.y.z * matrix2.z.w + matrix1.y.w * matrix2.w.w + matrix1.y.e * matrix2.e.w;
    e = matrix1.y.x * matrix2.x.e + matrix1.y.y * matrix2.y.e + matrix1.y.z * matrix2.z.e + matrix1.y.w * matrix2.w.e + matrix1.y.e * matrix2.e.e;
    vec5 v2 = vec5(x,y,z,w,e);

    x = matrix1.z.x * matrix2.x.x + matrix1.z.y * matrix2.y.x + matrix1.z.z * matrix2.z.x + matrix1.z.w * matrix2.w.x + matrix1.z.e * matrix2.e.x;
    y = matrix1.z.x * matrix2.x.y + matrix1.z.y * matrix2.y.y + matrix1.z.z * matrix2.z.y + matrix1.z.w * matrix2.w.y + matrix1.z.e * matrix2.e.y;
    z = matrix1.z.x * matrix2.x.z + matrix1.z.y * matrix2.y.z + matrix1.z.z * matrix2.z.z + matrix1.z.w * matrix2.w.z + matrix1.z.e * matrix2.e.z;
    w = matrix1.z.x * matrix2.x.w + matrix1.z.y * matrix2.y.w + matrix1.z.z * matrix2.z.w + matrix1.z.w * matrix2.w.w + matrix1.z.e * matrix2.e.w;
    e = matrix1.z.x * matrix2.x.e + matrix1.z.y * matrix2.y.e + matrix1.z.z * matrix2.z.e + matrix1.z.w * matrix2.w.e + matrix1.z.e * matrix2.e.e;
    vec5 v3 = vec5(x,y,z,w,e);

    x = matrix1.w.x * matrix2.x.x + matrix1.w.y * matrix2.y.x + matrix1.w.z * matrix2.z.x + matrix1.w.w * matrix2.w.x + matrix1.w.e * matrix2.e.x;
    y = matrix1.w.x * matrix2.x.y + matrix1.w.y * matrix2.y.y + matrix1.w.z * matrix2.z.y + matrix1.w.w * matrix2.w.y + matrix1.w.e * matrix2.e.y;
    z = matrix1.w.x * matrix2.x.z + matrix1.w.y * matrix2.y.z + matrix1.w.z * matrix2.z.z + matrix1.w.w * matrix2.w.z + matrix1.w.e * matrix2.e.z;
    w = matrix1.w.x * matrix2.x.w + matrix1.w.y * matrix2.y.w + matrix1.w.z * matrix2.z.w + matrix1.w.w * matrix2.w.w + matrix1.w.e * matrix2.e.w;
    e = matrix1.w.x * matrix2.x.e + matrix1.w.y * matrix2.y.e + matrix1.w.z * matrix2.z.e + matrix1.w.w * matrix2.w.e + matrix1.w.e * matrix2.e.e;
    vec5 v4 = vec5(x,y,z,w,e);

    x = matrix1.e.x * matrix2.x.x + matrix1.e.y * matrix2.y.x + matrix1.e.z * matrix2.z.x + matrix1.e.w * matrix2.w.x + matrix1.e.e * matrix2.e.x;
    y = matrix1.e.x * matrix2.x.y + matrix1.e.y * matrix2.y.y + matrix1.e.z * matrix2.z.y + matrix1.e.w * matrix2.w.y + matrix1.e.e * matrix2.e.y;
    z = matrix1.e.x * matrix2.x.z + matrix1.e.y * matrix2.y.z + matrix1.e.z * matrix2.z.z + matrix1.e.w * matrix2.w.z + matrix1.e.e * matrix2.e.z;
    w = matrix1.e.x * matrix2.x.w + matrix1.e.y * matrix2.y.w + matrix1.e.z * matrix2.z.w + matrix1.e.w * matrix2.w.w + matrix1.e.e * matrix2.e.w;
    e = matrix1.e.x * matrix2.x.e + matrix1.e.y * matrix2.y.e + matrix1.e.z * matrix2.z.e + matrix1.e.w * matrix2.w.e + matrix1.e.e * matrix2.e.e;
    vec5 v5 = vec5(x,y,z,w,e);

    return mat5(v1,v2,v3,v4,v5);
}