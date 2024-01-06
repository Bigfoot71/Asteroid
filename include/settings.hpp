#ifndef ASTEROID_GLOBAL_HPP
#define ASTEROID_GLOBAL_HPP

namespace Asteroid {

    constexpr int TargetFPS             = 60;
    constexpr float DefaultVolume       = 0.25f;            ///< Default master volume level
    constexpr float DefaultMusicVolume  = 0.75f;            ///< Default music volume level

    constexpr int BaseWidth             = 800;
    constexpr int BaseHeight            = 450;

    constexpr unsigned PlayerCount      = 9;                ///< Number of playable ship types

    constexpr unsigned short MaxBullets = 8*24 + 128;       ///< EnemiesCount * MaxEnemyBullets + MaxPlayerBullets

}

#endif //ASTEROID_GLOBAL_HPP
