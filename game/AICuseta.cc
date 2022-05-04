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


  //Para mirar si la unidad es mia o no
  bool unidad_mia(int unit_id)      
  {
    bool ret=false;
    for(unsigned int i=0; i<  dwarves(me()).size() and ret==false; ++i)
    {
      if(unit_id ==  my_units(me())[i]) ret=true;
    }
    return ret;
  }















  vector<Pos> pos_tesoros;
  Pos tesoro_mas_cerca(Unit &u)
  {
    Pos mas_cerca;
    for(int i=2;i<59;++i)
    {
      for(int j=2;j<59;++j)
      {
        Pos posicion = Pos(i,j);
        Cell celda = cell(posicion);
        if(celda.treasure) pos_tesoros.push_back(posicion);
      }
    } 

    unsigned int res_filas=0;
    unsigned int res_columnas=0;
    int sum_resta_anterior = 999;
    //Analizamos las posiciones y comparamos la mas cerca
    for(unsigned int i=0; i<pos_tesoros.size(); ++i)  
    {
      res_filas = abs(u.pos.i - pos_tesoros[i].i);
      res_columnas = abs(u.pos.j - pos_tesoros[i].j);
      int sum_resta_actual = res_filas + res_columnas;
      if( sum_resta_actual < sum_resta_anterior)                           
      { 
        sum_resta_anterior = sum_resta_actual;
        mas_cerca.i=pos_tesoros[i].i;
        mas_cerca.j=pos_tesoros[i].j;
      }
    }
    return mas_cerca;
  }

  //Entrar dentro
  void entrar_dentro(Unit &u)
  {
    if(u.pos.i== 0 or 2) command(u.id,Bottom);
    else if(u.pos.i== 58 or 59) command(u.id,Top);

    else if(u.pos.j== 0 or 1) command(u.id,Right);
    else if(u.pos.j== 58 or 59) command(u.id,Left);
  }

  //Si hay Balrog cerca
  bool balrog_cerca()
  {

  }




  //Buscar al mago mas cerca si duende con poca vida 

  //Buscar al duende mas cerca y con mas daño



  map<int,pair<bool,pair<Dir,Dir>>> en_muro;
  map<int,bool> esquivando_muro;
  //Nuevo metodo esquivar
  void esquivar_muro(Unit &u)
  {
    Dir dir=en_muro[u.id].second.first;
    Dir siguiente_dir=en_muro[u.id].second.second;

    if(cell(u.pos+dir).type != Granite and not esquivando_muro[u.id]) 
    {
      command(u.id,dir);
      en_muro[u.id].first=false;
    }
    else if(cell(u.pos+siguiente_dir).type != Granite) 
    {
      command(u.id,siguiente_dir);
      esquivando_muro[u.id]=false;
    }
    else
    {
      if(dir==Top) command(u.id,Bottom);
      else if(dir==Bottom) command(u.id,Top);
      else if(dir==Right) command(u.id,Left);
      else command(u.id,Right);
      esquivando_muro[u.id]=true;
    }
  }
  //Ir a buscar tesoros -> solo hacerlo una vez y apuntarse donde estan los tesoros 
  //Cada vez que se vaya a un tesoro mirar antes si aun tiene el tesoro o no
  void ir_tesoro(Unit &u)
  {
    Cell arriba = cell(u.pos + Top);
    Cell abajo = cell(u.pos + Bottom);
    Cell derecha = cell(u.pos + Right);
    Cell izquierda = cell(u.pos + Left);

    //Miramos Tesoro mas cerca
    Pos camino_mas_cerca=tesoro_mas_cerca(u);   
    
    unsigned int res_filas; 
    unsigned int res_columnas;
    Pos siguiente_dir;

    res_filas = abs(u.pos.i - camino_mas_cerca.i);    
    res_columnas = abs(u.pos.j - camino_mas_cerca.j); 
    siguiente_dir=camino_mas_cerca;


    if(en_muro[u.id].first)
    {
      //Una vez empieza a esquivar muro ignora todo lo demas y solo esquiva el muro y va hacia el objetivo
      //inicial siga sin ser mio o ya lo sea 
      //Salir del modo esquivar muro cuando pueda mover a la direccion inicial(la que iria si no hubiera muro)
      esquivar_muro(u);
    }


    if(res_columnas==0)     //La unidad esta en la misma columna del tesoro
    {
      if(u.pos.i < camino_mas_cerca.i)  //mi unidad esta a arriba del destino mas cerca
      {
        //Nuevo modelo esquivando muro
        if(abajo.type!=Granite and not en_muro[u.id].first)
        {
          command(u.id,Bottom);
        } 
        else    
        {
          en_muro[u.id].first=true;
          en_muro[u.id].second.first=Bottom;
          if(u.pos.j < siguiente_dir.j) en_muro[u.id].second.second=Right;   
          else en_muro[u.id].second.second=Left;
          esquivar_muro(u); 
        }
      }
      else
      {
        if(arriba.type!=Granite and not en_muro[u.id].first)
        {
          command(u.id,Top);
        } 
        else
        {
          en_muro[u.id].first=true;
          en_muro[u.id].second.first=Top;
          if(u.pos.j < siguiente_dir.j) en_muro[u.id].second.second=Right;   
          else en_muro[u.id].second.second=Left;
          esquivar_muro(u);
        }
      }
    }
    else if(res_filas==0)     //La unidad esta en la misma fila del tesoro
    {
      if(u.pos.j < camino_mas_cerca.j)  //mi unidad esta a la izquierda del tesoro mas cerca
      {   
        if(derecha.type!=Granite and not en_muro[u.id].first)
        {
          command(u.id,Right);
        } 
        else
        {
          en_muro[u.id].first=true;
          en_muro[u.id].second.first=Right;
          if(u.pos.i < siguiente_dir.i) en_muro[u.id].second.second=Bottom;   
          else en_muro[u.id].second.second=Top;
          esquivar_muro(u); //llamar a esquivar muro
        } 
      }
      else    //Mi unidad esta a la derecha del tesoro mas cerca
      {
        if(izquierda.type!=Granite and not en_muro[u.id].first)
        {
          command(u.id,Left);
        } 
        else
        {
          en_muro[u.id].first=true;
          en_muro[u.id].second.first=Left;
          if(u.pos.i < siguiente_dir.i) en_muro[u.id].second.second=Bottom;   
          else en_muro[u.id].second.second=Top;
          esquivar_muro(u); //llamar a esquivar muro
        }
      }
    }
    //Movemientos en diagonal
    else if( res_filas < res_columnas )   //La unidad esta en diferente fila i columna 
    {
      if(u.pos.i < camino_mas_cerca.i)  //mi unidad esta a arriba del destino mas cerca
      {

      }
      else
      {

      }
    }
    else
    {
      if(u.pos.j < camino_mas_cerca.j)  //mi unidad esta a arriba del destino mas cerca
      {

      }
      else
      {

      }

    }
  }






  //En cueva
  void en_cueva(Unit &u)
  {
    bool moved=false;
    for (int d = 0; d < 8 and not moved; ++d) 
    {
      Pos p1 = u.pos + Dir(d);
      Cell c1 = cell(p1);
      //Si hay tesoro
      if(c1.treasure)
      {
        command(u.id,Dir(d));
        moved=true;
      }
      //Si hay cueva que no es mia
      if(c1.type==Cave and c1.owner != me())
      {
        command(u.id,Dir(d));
        moved=true;
      }
      //Si hay enemigo con poca vida
      if(c1.id)
      {
        if

      }
    }
  }

   

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () 
  {
    //mover dwarves
    for(uint i=0; i<dwarves(me()).size(); ++i)
    {
      int id=dwarves(me())[i];
      Unit u= unit(id);

      //si esta en el exterior
      if(cell(u.pos).type==Outside)
      {
        entrar_dentro(u);
      }
      else
      {
        en_cueva(u);
      }


    }

    //mover wizards
    for(uint i=0; i<wizards(me()).size(); ++i)
    {
      int id=wizards(me())[i];
      Unit u= unit(id);

      //si esta en el exterior
      if(cell(u.pos).type==Outside)
      {
        entrar_dentro(u);
      }

    }
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