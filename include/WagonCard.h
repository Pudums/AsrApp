//
// Created by timofey vasilevskij on 19.02.2021.
//

#ifndef PROJECT_WAGON_CARD_H
#define PROJECT_WAGON_CARD_H

#include <iostream>
#include <string>

struct WagonCard {
    std::string color;
    WagonCard() = default;
    explicit WagonCard(std::string s);
    // drawing
};

#endif  // PROJECT_WAGON_CARD_H
