#pragma once

#ifndef STF_define
#define STF_define

#include <Analysis/Spectro.hpp>

namespace STF {

    Spectro::Spectrogram SpectroToStf(const Spectro::Spectrogram &spectro, int chunkSize);
}

#endif // !STF_define