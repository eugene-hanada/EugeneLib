#include "../../Include/Graphics/VertexView.h"

Eugene::VertexView::~VertexView()
{
}

const std::uint32_t Eugene::VertexView::GetVertexNum(void) const
{
    return vertexNum_;
}

Eugene::VertexView::VertexView(std::uint32_t vertexNum):
    vertexNum_{vertexNum}
{
}
