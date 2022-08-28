#include "index_buffer.hpp"

#include "log.hpp"

IndexBuffer::IndexBuffer(const int indices[], const int size) {
    LOG_TRACE("Creating Index Buffer");
    m_Count = size / sizeof(int);
    glCreateBuffers(1, &m_BufferID); // glCreateBuffers is exclusive to GL ^4.5
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer::~IndexBuffer() {
    LOG_TRACE("Deleting Index Buffer");
    glDeleteBuffers(1, &m_BufferID);
}

void IndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
}

void IndexBuffer::Unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}