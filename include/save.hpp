#ifndef ASTEROID_SAVE_HPP
#define ASTEROID_SAVE_HPP

#include <rayflex.hpp>
#include "settings.hpp"

namespace Asteroid {

    struct PlayerStats
    {
        uint16_t acceleration;      // Acceleration de base du moteur
        uint16_t maxVelEngine;      // Velocité max de base du moteur
        uint16_t bulletPerSec;      // Nombre de bullet par secondes
        uint16_t damage;            // Degat d'une bullet
        uint16_t regen;             // Regeneration de la vie par secondes
        uint16_t life;              // Vie du vaisseau
        bool unlocked;              // Indique si ce vaisseau a été débloqué/acheté
    };

    struct Save
    {
        PlayerStats playerStats[PlayerCount] = {

            {
                .acceleration = 100,
                .maxVelEngine = 200,
                .bulletPerSec = 5,
                .damage = 10,
                .regen = 10,
                .life = 100,

                .unlocked = true
            },

            {
                .acceleration = 120,
                .maxVelEngine = 240,
                .bulletPerSec = 5,
                .damage = 10,
                .regen = 10,
                .life = 100,

                .unlocked = false
            },

            {
                .acceleration = 120,
                .maxVelEngine = 240,
                .bulletPerSec = 6,
                .damage = 12,
                .regen = 10,
                .life = 125,

                .unlocked = false
            },

            {
                .acceleration = 150,
                .maxVelEngine = 300,
                .bulletPerSec = 6,
                .damage = 12,
                .regen = 20,
                .life = 125,

                .unlocked = false
            },

            {
                .acceleration = 150,
                .maxVelEngine = 300,
                .bulletPerSec = 6,
                .damage = 12,
                .regen = 20,
                .life = 135,

                .unlocked = false
            },

            {
                .acceleration = 160,
                .maxVelEngine = 320,
                .bulletPerSec = 8,
                .damage = 16,
                .regen = 20,
                .life = 140,

                .unlocked = false
            },

            {
                .acceleration = 160,
                .maxVelEngine = 320,
                .bulletPerSec = 8,
                .damage = 16,
                .regen = 30,
                .life = 150,

                .unlocked = false
            },

            {
                .acceleration = 180,
                .maxVelEngine = 360,
                .bulletPerSec = 8,
                .damage = 16,
                .regen = 30,
                .life = 175,

                .unlocked = false
            },

            {
                .acceleration = 180,
                .maxVelEngine = 360,
                .bulletPerSec = 10,
                .damage = 20,
                .regen = 30,
                .life = 200,

                .unlocked = false
            },

        };

        uint32_t money = 0, hiscore = 0;
        float volume = DefaultVolume;
        uint8_t selectedPlayer = 0;
        bool firstLaunch = true;
    };

    constexpr Save saveOrigin{};

    inline void ToggleVolume(rf::core::App* app)
    {
        auto save = app->saveManager->Get<Save>();
        save->volume = (save->volume == 0.0f ? DefaultVolume : 0.0f);
        app->audio.SetVolume(save->volume);
    }

}

#endif //ASTEROID_SAVE_HPP
