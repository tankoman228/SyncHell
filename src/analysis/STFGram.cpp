#include <STFGram.hpp>
#include <iostream>

namespace STF {

    // Нормализация и создание текстуры для визуализации STF фич
    sf::Texture *GetSTFVisualized(Spectro::Spectrogram rawSTF) {

        sf::Image image;
        image.create(rawSTF.timeLength, 256);

        float featureMax[256] = {-999999};
        float featureMin[256] = {999999};

        for (int feature = 0; feature < 256; feature++) {
            for (int t = 0; t < rawSTF.timeLength; t++) {

                float& val = rawSTF.spectro[t][feature];
                if (val > featureMax[feature]) {
                    featureMax[feature] = val;
                }      
                else if (val < featureMin[feature]) {
                    featureMin[feature] = val;
                }                    
            }
        }

        for (int feature = 0; feature < 256; feature++) {

            std::cout << feature << " is between " << featureMax[feature] << " " << featureMin[feature] <<  "\n";

            for (int t = 0; t < rawSTF.timeLength; t++) {

                float val = rawSTF.spectro[t][feature] / featureMax[feature];
                image.setPixel(t, feature, sf::Color(val * 254.f, val * 254.f, val * 254.f));         
            }
        }

        sf::Texture* texture = new sf::Texture();
        texture->loadFromImage(image);
        return texture;
    }
}