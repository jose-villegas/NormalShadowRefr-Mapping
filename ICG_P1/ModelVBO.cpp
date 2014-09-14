#include "stdafx.h"
#include "ModelVBO.h"

ModelVBO::ModelVBO(void)
{
}

ModelVBO::~ModelVBO(void)
{
}

void ModelVBO::Load(ModelOBJ obj)
{
    if (obj.hasPositions())
    {
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, obj.getNumberOfVertices() * obj.getVertexSize(), obj.getVertexBuffer(), GL_STATIC_DRAW);
    }

    if (obj.hasTextureCoords())
    {
        glGenBuffers(1, &uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, obj.getNumberOfVertices() * obj.getVertexSize(), obj.getVertexBuffer(), GL_STATIC_DRAW);
    }

    if (obj.hasNormals())
    {
        glGenBuffers(1, &normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, obj.getNumberOfVertices() * obj.getVertexSize(), obj.getVertexBuffer(), GL_STATIC_DRAW);
    }

    if (obj.hasTangents())
    {
        glGenBuffers(1, &tangentbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
        glBufferData(GL_ARRAY_BUFFER, obj.getNumberOfVertices() * obj.getVertexSize(), obj.getVertexBuffer(), GL_STATIC_DRAW);
    }

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.getNumberOfIndices() * obj.getIndexSize(), obj.getIndexBuffer(),
                 GL_STATIC_DRAW);
}

void ModelVBO::BindBuffers(ModelOBJ obj)
{
    if (obj.hasPositions())
    {
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            obj.getVertexSize(),                  // stride
            (void *)0           // array buffer offset
        );
    }

    if (obj.hasTextureCoords())
    {
        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            1,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            obj.getVertexSize(),                                // stride
            (void *)(3 * sizeof(float))                       // array buffer offset
        );
    }

    if (obj.hasNormals())
    {
        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(
            2,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            obj.getVertexSize(),                                // stride
            (void *)(5 * sizeof(float))                      // array buffer offset
        );
    }

    if (obj.hasTangents())
    {
        // 4th attribute buffer : tangents
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
        glVertexAttribPointer(
            3,                                // attribute
            4,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            obj.getVertexSize(),                                // stride
            (void *)(8 * sizeof(float))                     // array buffer offset
        );
    }
}

void ModelVBO::UnbindBuffers()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}