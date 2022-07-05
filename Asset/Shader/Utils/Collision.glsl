#ifndef _COLLISION_GLSL_
#define _COLLISION_GLSL_

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