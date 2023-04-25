#include "../../Include/Graphics/IndexView.h"

Eugene::IndexView::~IndexView()
{
}

const std::uint32_t Eugene::IndexView::GetIndexNum(void) const
{
    return indexNum_;
}


Eugene::IndexView::IndexView(std::uint32_t indexNum) :
    indexNum_{indexNum}
{
}
