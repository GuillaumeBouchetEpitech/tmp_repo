
#pragma once

struct t_vertex3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    t_vertex3();
    t_vertex3(float x, float y, float z);
    t_vertex3(const t_vertex3& other);
    t_vertex3(t_vertex3&& other);
    ~t_vertex3();

    t_vertex3& operator=(const t_vertex3& other);
    t_vertex3& operator=(t_vertex3&& other);

};

