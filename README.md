# Project CONTRE-COURANT

## 1. Project Description

**CONTRE-COURANT** is a 2D arcade survival game that explores the theme of "The Escape" through an inverted perspective: the player must escape the surface. You control a marine entity struggling against a fishing boat’s winch that constantly pulls you upward. The goal is to reach the ocean floor by balancing swimming force, stamina, and the collective mass of your swarm.

## 2. Gameplay & Mechanics

- **The Struggle (Tug-of-War):** The core of the game is a constant battle against the surface. As you dive deeper, the boat's winch strains and pulls harder, creating a mounting sense of pressure and challenge the closer you get to your goal.
- **Strength in Numbers (The Swarm):** You don't dive alone. Rescuing allies creates a "living shield" around you. These allies physically add their mass to yours, helping you sink faster and turning a solitary struggle into a collective effort.
- **The Rhythm of the Dive:** Success depends on managing your energy. You must balance aggressive bursts of swimming with moments of recovery, creating a tactical rhythm where every move counts.

## 3. Technical Quality

- **Architecture:** Built with a clean, modular structure (OOP) to ensure the code is easy to read, maintain, and expand.

## 4. Stability & Performance

- **Efficient Collisions:** Implementation of AABB (Axis-Aligned Bounding Boxes) ensures optimal performance and precise interaction between the player, allies, and environmental hazards.
- **Status:** Fully functional "Vertical Slice" with a complete game loop (Menu → Play → Win/Loss).

## 5. Setup & Controls

1. Run `generate-windows.bat` to create the build environment. (refer to the (SETUP_INSTRUCTIONS.md) file to install necessary dependencies according to your work environment)
2. Open `build/win64/UbiTorContestAPI.sln` in **Visual Studio 2022**.
3. Build and Run (**F5**) in Debug or Release mode.

### Controls:

- **ARROWS** Navigate and apply force to Dive.
- **SPACE / ENTER:** Start Game.
- **R:** Restart after Win/Loss.

---

\_xcaretCastilloS
