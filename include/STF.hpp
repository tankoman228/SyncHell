#pragma once

#ifndef STF_define
#define STF_define

#include <Spectro.hpp>

namespace STF {

    Spectro::Spectrogram SpectroToStf(const Spectro::Spectrogram &spectro, int chunkSize);
}

#endif // !STF_define