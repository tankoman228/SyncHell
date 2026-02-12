#include <STFGram.hpp>
#include <iostream>

namespace STF {

    // создание текстуры для визуализации STF фич
    sf::Texture *GetSTFVisualized(Spectro::Spectrogram rawSTF) {

        sf::Image image;
        image.create(rawSTF.timeLength, 256);

        for (int feature = 0; feature < 256; feature++) {

            for (int t = 0; t < rawSTF.timeLength; t++) {

                float val = rawSTF.spectro[t][feature];
                image.setPixel(t, feature, sf::Color(val * 254.f, val * 254.f, val * 254.f));         
            }
        }

        sf::Texture* texture = new sf::Texture();
        texture->loadFromImage(image);
        return texture;
    }
}