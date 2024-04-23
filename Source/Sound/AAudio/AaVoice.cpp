#include "AaVoice.h"
#include "AaSubmix.h"

AaVoice::AaVoice(AaSubmix*  submix,std::uint16_t inChannel, std::uint16_t outChannel, std::uint32_t sample) :
    sample_{sample}, submix_{submix}
{

    SetChannel(inChannel,outChannel);
    if (submix_ != nullptr)
    {
        submix_->AddVoice(this);
    }
}

AaVoice::~AaVoice()
{
    if (submix_ != nullptr)
    {
        submix_->RemoveVoice(this);
    }
}

void AaVoice::SetOut(AaSubmix *submix,std::uint16_t inChannel, std::uint16_t outChannel)
{
    if (outMatrix_.size() < (inChannel * outChannel))
    {
        outMatrix_.resize(inChannel * outChannel);
    }

    if (submix_ != nullptr)
    {
        submix_->RemoveVoice(this);
    }
    submix_ = submix;

    if (submix_ != nullptr)
    {
        submix_->AddVoice(this);
    }
}

void AaVoice::SetChannel(std::uint16_t inChannel, std::uint16_t outChannel) {
    outMatrix_.resize(outChannel * inChannel);
    std::fill(outMatrix_.begin(),outMatrix_.end(), 0.0f);
    for (std::uint16_t y = 0; y < outChannel; y++)
    {
        for (std::uint16_t x = 0; x < inChannel; x++)
        {
            if (x == y)
            {
                outMatrix_[x + (y * outChannel)] = 1.0f;
            }
        }
    }
}
