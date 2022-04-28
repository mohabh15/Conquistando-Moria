#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Cuseta


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

  //Buscar al mago mas cerca si duende con poca vida 

  //Buscar al duende mas cerca y con mas daño

  //Ir a buscar tesoros -> solo hacerlo una vez y apuntarse donde estan los tesoros 
  //Cada vez que se vaya a un tesoro mirar antes si aun tiene el tesoro o no

   

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () 
  {
    

  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

/*
//Duende
1. si hay enemigo cerca atacar
2. orcos:
2. trolls:
4. balrog: mirar si esta a 2 casillas 



*/