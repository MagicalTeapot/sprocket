#pragma once
#include "Maths.h"
#include "Resources.h"
#include "BufferLayout.h"

#include <vector>

namespace Sprocket {

class StreamBuffer 
// A class to be used with the intention of streaming data to VBOs.
// By default, the VBO has 3 attribute pointers enabled, but nothing
// is defined; that is up to the user.
{
    std::shared_ptr<VAO> d_vao;
    std::shared_ptr<VBO> d_vertexBuffer;
    std::shared_ptr<VBO> d_indexBuffer;


public:
    StreamBuffer();
    ~StreamBuffer();

    void Bind() const;
    void Unbind() const;

    void SetBufferLayout(const BufferLayout& layout) const;
        // Sets the buffer layout of the vertex buffer.

    void SetVertexData(std::size_t size, const void* data);
    void SetIndexData(std::size_t size, const void* data);
        // Sets the data inside the StreamBuffer object. The
        // StreamBuffer object MUST be bound before calling these
        // functions, otherwise the behaviour is undefined.

    void Draw(int count) const;
        // Draws the specified number of indices in the buffer.

    // Templatised wrapper functions for an nicer interface.
    template <class Vertex>
    void SetVertexData(const std::vector<Vertex>& data);

    template <class Index>
    void SetIndexData(const std::vector<Index>& data);

    template <class Vertex, class Index>
    void Draw(const std::vector<Vertex>& vertices,
              const std::vector<Index>& indices);
};

template <class Vertex>
void StreamBuffer::SetVertexData(const std::vector<Vertex>& data)
{
    SetVertexData(sizeof(Vertex) * data.size(), data.data());
}

template <class Index>
void StreamBuffer::SetIndexData(const std::vector<Index>& data)
{
    SetIndexData(sizeof(Index) * data.size(), data.data());
}

template <class Vertex, class Index>
void StreamBuffer::Draw(const std::vector<Vertex>& vertices,
                        const std::vector<Index>& indices)
{
    SetVertexData(vertices);
    SetIndexData(indices);
    Draw(indices.size());
}

}