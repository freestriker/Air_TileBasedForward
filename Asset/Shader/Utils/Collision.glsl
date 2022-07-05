#ifndef _COLLISION_GLSL_
#define _COLLISION_GLSL_

vec4 BuildPlane(vec3 pos0, vec3 pos1, vec3 pos2)
{
    vec3 d0 = pos1 - pos0;
    vec3 d1 = pos2 - pos1;
    vec3 normal = normalize(cross(d0, d1));
    float p = -dot(normal, pos0);
    return vec4(normal, p);
}

bool ObbPlaneIntersection(vec3[8] vertexes, vec4[6] planes)
{
    for(int i = 0; i < 6; i++)
    {
        int j = 0;
        for(; j < 8; j++)
        {
            if(dot(vec4(vertexes[j], 1), planes[i]) > 0)
            {
                break;
            }
        }
        if(j == 8) return false;
    }
    return true;
}

bool ObbPlaneMatrixIntersection(vec3[8] vertexes, vec4[6] planes, mat4 matrix)
{
    for(int i = 0; i < 6; i++)
    {
        int j = 0;
        for(; j < 8; j++)
        {
            if(dot(matrix * vec4(vertexes[j], 1), planes[i]) > 0)
            {
                break;
            }
        }
        if(j == 8) return false;
    }
    return true;
}

#endif