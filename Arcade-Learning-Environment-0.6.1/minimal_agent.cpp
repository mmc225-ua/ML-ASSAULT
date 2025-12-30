#include <iostream>
#include <cmath>
#include <cstdint>
#include "src/ale_interface.hpp"
#include <SDL/SDL.h>
#include <ctime>
#include <fstream>

#include "weights_assault.hpp"


// Constants
constexpr uint32_t maxSteps = 7500;

static const bool BOT_MODE = true;   // true = bot, false = teclado humano


///////////////////////////////////////////////////////////////////////////////
/// Get info from RAM
///////////////////////////////////////////////////////////////////////////////
int32_t getPlayerX(ALEInterface& alei) {
   return alei.getRAM().get(72) + ((rand() % 3) - 1);
}

int32_t getBallX(ALEInterface& alei) {
   return alei.getRAM().get(99) + ((rand() % 3) - 1);
}

///////////////////////////////////////////////////////////////////////////////
/// Calcular el score (s = w*x + b)
///////////////////////////////////////////////////////////////////////////////
static inline float score128(const float* w, ALEInterface& alei) {
    float s = w[128];                  // bias
    auto &ram = alei.getRAM();
    for (int j = 0; j < 128; ++j) {
        float x = float(ram.get(j)) / 255.0f;   // MISMA normalización que en Python
        s += w[j] * x;
    }
    return s;
}

static inline int predict128(const float* w,  ALEInterface& alei) {
    return (score128(w, alei) >= 0.0f) ? 1 : 0;
}


///////////////////////////////////////////////////////////////////////////////
/// Do Next Agent Step
///////////////////////////////////////////////////////////////////////////////
// reward_t agentStep(ALEInterface& alei) {
//    static constexpr int32_t wide { 9 };
//    static int32_t lives { alei.lives() };
//    reward_t reward{0};

//    // When we loose a live, we need to press FIRE to start again
//    if (alei.lives() < lives) {
//       lives = alei.lives();
//       alei.act(PLAYER_A_FIRE);
//    }

//    // Apply rules.
//    auto playerX { getPlayerX(alei) };
//    auto ballX   { getBallX(alei)   };
   
//    if       (ballX < playerX + wide) { reward = alei.act(PLAYER_A_LEFT);   }
//    else if  (ballX > playerX + wide) { reward = alei.act(PLAYER_A_RIGHT);  }
   
//    return reward + alei.act(PLAYER_A_NOOP);
// }

//alei -> emulador de Atari ( Leemos RAM, consultamos vidas, ejecutamos acciones ...)
//chosenAction -> Salida de la función. (Devolvemos qué acción se ha decidido para luego registrarla en el CSV)
reward_t agentStep(ALEInterface& alei, Action& chosenAction) {
    static int32_t lives { alei.lives() };
    static int fireHold = 0;

    // Si perdemos una vida, pulsamos FIRE para reanudar
    if (alei.lives() < lives) {
        lives = alei.lives();
        alei.act(PLAYER_A_FIRE);
    }

    Action a = PLAYER_A_NOOP;

    if (BOT_MODE) {
        // --- MODO BOT: 3 perceptrones (LEFT / RIGHT / FIRE) ---
        float sL = score128(wLEFT,  alei);
        float sR = score128(wRIGHT, alei);
        float sF = score128(wFIRE,  alei);

        bool left  = (sL >= 0.0f);
        bool right = (sR >= 0.0f);
        bool fire  = (sF >= 0.0f);

        // Resolver conflicto LEFT y RIGHT si ambos “1”
        if (left && right) {
            if (sL >= sR) right = false;
            else          left  = false;
        }

        if (fire) {
            if (left)       a = PLAYER_A_LEFTFIRE;
            else if (right) a = PLAYER_A_RIGHTFIRE;
            else            a = PLAYER_A_UPFIRE;  // disparo vertical quieto
        } else {
            if (left)       a = PLAYER_A_LEFT;
            else if (right) a = PLAYER_A_RIGHT;
            else            a = PLAYER_A_NOOP;
        }

        chosenAction = a;
        return alei.act(a);
    }

    // --- MODO HUMANO  ---
    SDL_PumpEvents();
    int nkeys = 0;
    Uint8* kb = SDL_GetKeyState(&nkeys);

    bool left  = kb && kb[SDLK_LEFT];
    bool right = kb && kb[SDLK_RIGHT];
    bool fire  = kb && kb[SDLK_SPACE];

    if (fire) fireHold = 3;
    if (fireHold > 0) --fireHold;
    bool wantFire = fire || (fireHold > 0);

    if (wantFire) {
        if (left)       a = PLAYER_A_LEFTFIRE;
        else if (right) a = PLAYER_A_RIGHTFIRE;
        else            a = PLAYER_A_UPFIRE;
    } else {
        if (left)       a = PLAYER_A_LEFT;
        else if (right) a = PLAYER_A_RIGHT;
        else            a = PLAYER_A_NOOP;
    }

    chosenAction = a;
    return alei.act(a);
}



///////////////////////////////////////////////////////////////////////////////
/// Print usage and exit
///////////////////////////////////////////////////////////////////////////////
void usage(char const* pname) {
   std::cerr
      << "\nUSAGE:\n" 
      << "   " << pname << " <romfile>\n";
   exit(-1);
}




///////////////////////////////////////////////////////////////////////////////
/// MAIN PROGRAM
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
   reward_t totalReward{};
   ALEInterface alei{};

   // Check input parameter
   if (argc != 2 && argc != 3)
      usage(argv[0]);

   // Configure alei object.
   alei.setInt  ("random_seed", 0);
   alei.setFloat("repeat_action_probability", 0);
   alei.setBool ("display_screen", true);
   alei.setBool ("sound", false);
   alei.loadROM (argv[1]);

   std::ofstream csv;
   bool recording = (argc == 3);
   if (recording) {
      csv.open(argv[2]);
      if (!csv) {
         std::cerr << "Cannot open output file: " << argv[2] << std::endl;
         return -1;
      }
      // Cabecera
      csv << "step,action,reward";
      for (int i = 0; i < 128; ++i) csv << ",ram" << i;
      csv << "\n";
   }


   // Init
   std::srand(static_cast<uint32_t>(std::time(0)));

   // Main loop
   {
      alei.act(PLAYER_A_FIRE);
      uint32_t step{};
      while (!alei.game_over() && step < maxSteps) {
         Action chosen = PLAYER_A_NOOP;
         reward_t r = agentStep(alei, chosen);
         totalReward += r;

         if (recording) {
            csv << step << "," << int(chosen) << "," << r;
            auto &ram = alei.getRAM();
            for (int i = 0; i < 128; ++i) csv << "," << int(ram.get(i));
            csv << "\n";
         }

         ++step;
      }

      std::cout << "Steps: " << step << std::endl;
      std::cout << "Reward: " << totalReward << std::endl;
   }

   if (recording) csv.close();
   return 0;
}
