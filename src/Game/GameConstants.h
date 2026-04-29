#ifndef _GAME_CONSTANTS_H
#define _GAME_CONSTANTS_H

namespace GameConfig {

    // --- DISPLAY & LIMITS ---
    constexpr float SCREEN_CENTER_X = 512.0f;
    constexpr float GAME_MIN_X = 50.0f;
    constexpr float GAME_MAX_X = 974.0f;
    constexpr float DESPAWN_Y_TOP = 650.0f;   
    constexpr float GAME_DESPAWN_Y = 650.0f; // Alias for DESPAWN_Y_TOP x algas

    constexpr float DESPAWN_Y_BOTTOM = -500.0f;
    constexpr float SPAWN_START_Y = -200.0f;

    // --- ALGAS ---
    constexpr int ALGAE_COST_TO_CALL = 3;
    constexpr float ALGAE_SPAWN_RATE_START = 2.0f; 

    // --- BOIDS (SWARM BACKGROUND) ---
    constexpr float BOIDS_COHESION_WEIGHT = 0.5f;
    constexpr float BOIDS_ALIGNMENT_WEIGHT = 1.0f;
    constexpr float BOIDS_SEPARATION_WEIGHT = 1.7f;
    constexpr float BOIDS_NEIGHBOR_RADIUS = 150.0f;
    constexpr float BOIDS_SEPARATION_RADIUS = 40.0f;
    constexpr float BOIDS_MAX_SPEED = 120.0f;

    //
    constexpr float START_DEPTH = 400.0f;
    constexpr float WIN_DEPTH = 2500.0f;
    constexpr float LOSE_DEPTH = 0.0f;
    constexpr float MUSIC_CHANGE_DEPTH = 1200.0f;

    // (PLAYER) 
    constexpr float PLAYER_START_STAMINA = 100.0f;
    constexpr float PLAYER_START_MASS = 10.0f;
    constexpr float PLAYER_SPEED_X = 300.0f;
    constexpr float PLAYER_MIN_SPEED_X = 80.0f;  // V
    constexpr float PLAYER_START_Y = 200.0f;
    constexpr float PLAYER_DIVE_Y = 220.0f;
    constexpr float PLAYER_TIRED_Y = 180.0f;
    constexpr float PLAYER_DIVE_STAMINA_COST = 15.0f; 
    constexpr float PLAYER_IDLE_STAMINA_REGEN = 10.0f; 
    constexpr float PLAYER_BASE_SWIM_SPEED = 200.0f;
    constexpr float PLAYER_MASS_SWIM_MULTIPLIER = 5.0f;

    // --- FORMACIÓN DE ALIADOS (SWARM) ---
    constexpr float SWARM_FORMATION_RADIUS = 60.0f;
    constexpr float SWARM_ROTATION_SPEED = 0.5f;
    constexpr float SWARM_PULSE_SPEED = 2.0f;
    constexpr float SWARM_PULSE_AMPLITUDE = 10.0f;
    constexpr float ALLY_LERP_FACTOR = 0.1f;
    constexpr int   SWARM_MAX_SIZE = 8;  

    // --- ESTADÍSTICAS OBJETOS ---
    constexpr float ALLY_STAMINA_HEAL = 10.0f;
    constexpr float ALLY_MASS_VALUE = 5.0f;
    constexpr float ALLY_SCALE = 0.08f;
    
    constexpr float PENALTY_PLASTIC_BAG = 20.0f;
    constexpr float PENALTY_NET = 10.0f;
    constexpr float PENALTY_CAN = 30.0f;
    constexpr float PENALTY_BOTTLE = 15.0f;  

    constexpr float OBSTACLE_SCALE_BAG = 0.20f;
    constexpr float OBSTACLE_SCALE_NET = 0.35f;
    constexpr float OBSTACLE_SCALE_CAN = 0.12f;
    constexpr float OBSTACLE_SCALE_BOTTLE = 0.10f;
    constexpr float OBSTACLE_SCALE_DEFAULT = 0.15f;

    constexpr float OBSTACLE_SPEED_BAG = 30.0f;
    constexpr float OBSTACLE_SPEED_NET = 25.0f;
    constexpr float OBSTACLE_SPEED_CAN = 120.0f;
    constexpr float OBSTACLE_SPEED_BOTTLE = 55.0f;  
    constexpr float OBSTACLE_SPEED_DEFAULT = 60.0f;

    // (BOAT) 
    constexpr float BOAT_PULL_BASE_SPEED = 80.0f;
    constexpr float BOAT_PULL_DEPTH_MULTIPLIER = 0.015f;
    constexpr float BOAT_DISPLAY_Y = 300.0f;

    // --- ENTORNO ---
    constexpr float OCEAN_BG_START_Y = -800.0f;

    // --- UI: FLASH DE COLISIÓN ---
    constexpr float FLASH_DURATION = 0.15f;  
    constexpr float FLASH_MAX_ALPHA = 0.45f; 

    // --- UI: BARRA DE PROFUNDIDAD ---
    constexpr float DEPTH_BAR_X = 960.0f;       
    constexpr float DEPTH_BAR_TOP_Y = 700.0f;   
    constexpr float DEPTH_BAR_BOTTOM_Y = 40.0f; // Y del extremo inferior
    constexpr float DEPTH_BAR_WIDTH = 12.0f;    // Grosor visual de la barra
}

#endif
