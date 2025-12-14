#include <iostream>
#include <cmath>
#include <cstdint>
#include "src/ale_interface.hpp"
#include <SDL/SDL.h>
#include <ctime>
#include <fstream>


// Constants
constexpr uint32_t maxSteps = 7500;

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
    static int32_t lives { alei.lives() }; // Guarda el número de vidas
    static int fireHold = 0; // Para el disparo

    if (alei.lives() < lives) { // Comprueba si he muerto
        lives = alei.lives();
        alei.act(PLAYER_A_FIRE);
    }

    // Actualiza el estado del teclado (proesa eventos del sistema para poder leer las teclas pulsadas)
    SDL_PumpEvents();
    int nkeys = 0;
    Uint8* kb = SDL_GetKeyState(&nkeys);

    // Le asignamos nombre a las teclas (por legibilidad)
    bool left  = kb && kb[SDLK_LEFT];
    bool right = kb && kb[SDLK_RIGHT];
    bool up    = kb && kb[SDLK_UP];
    bool fire  = kb && kb[SDLK_SPACE];

    //Si no se pulsa nada, no hacemos nada
    Action a = PLAYER_A_NOOP;

    // Para mantener durante 3 frames la acción de disparo por si no se registra en 1 solo frame
    // Aunque "dispare" durante 3 frames seguidos, solo sale 1 bala debido al cooldown del propio juego
    if (fire) fireHold = 3;
    if (fireHold > 0) --fireHold;

    bool wantFire = fire || (fireHold > 0);

    //Dependiendo de la tecla pulsada, ejecuto una acción u otra
    if (wantFire) {
        if (left)       a = PLAYER_A_LEFTFIRE;
        else if (right) a = PLAYER_A_RIGHTFIRE;
        else            a = PLAYER_A_UPFIRE;
    } else {
        if (left)       a = PLAYER_A_LEFT;
        else if (right) a = PLAYER_A_RIGHT;
        else if (up)    a = PLAYER_A_UP;
        else            a = PLAYER_A_NOOP;
    }

    chosenAction = a;
    return alei.act(a); // recompensa que devuelve ALE tras ejecutar la acción (puntos / penalización) (Normalmente 0)
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
