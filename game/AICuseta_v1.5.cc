#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Cuseta_v1.5


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
  //Ir a por enemigos
  void enemigos(Unit &u)
  {
    vector<vector<bool>> visitados(60,vector<bool>(60,false));
    priority_queue<pair<int,pair<Pos,Dir>>,vector<pair<int,pair<Pos,Dir>>>,greater<pair<int,pair<Pos,Dir>>>> q;
    balrog_trolls(visitados);
    q.push({0,{u.pos,None}});
    bool vist=true,moved=false;

    while (not q.empty() and not moved)
    {
      Pos v=q.top().second.first;
      Dir dir=q.top().second.second;
      int dist=q.top().first;
      q.pop();

      if (cell(v).id!=-1 and unit(cell(v).id).player!=me() )
      {
        command(u.id, dir);
        moved=true;
      }
      else 
      {
        for (int d=0; d<8 and not moved; ++d)
        {
          Pos pos=v+Dir(d);
          int distancia=dist;
          if (bon_vei(pos) and not visitados[pos.i][pos.j])
          {
            if (cell(pos).type==Rock) distancia+=cell(pos).turns;
            else if (cell(pos).treasure) distancia=0;
            else ++distancia;
            if (vist) dir=Dir(d);
            visitados[pos.i][pos.j]=true;
            q.push({distancia,{pos,dir}});
          }
        }
        vist=false;
      }
    }
  }



  //buscar el tesoro mas cerca i ir
  const int INF = 1e9;
  void tesoro(Unit &u)
  {
    vector<vector<bool>> visitados(60,vector<bool>(60,false));
    priority_queue<pair<int,pair<Pos,Dir>>,vector<pair<int,pair<Pos,Dir>>>,greater<pair<int,pair<Pos,Dir>>>> q;
    balrog_trolls(visitados);
    q.push({0,{u.pos,None}});
    bool vist=true,moved=false;

    while (not q.empty() and not moved)
    {
      Pos v=q.top().second.first;
      Dir dir=q.top().second.second;
      int dist=q.top().first;
      q.pop();

      if (cell(v).treasure)
      {
        command(u.id, dir);
        moved=true;
      }
      else 
      {
        for (int d=0; d<8 and not moved; ++d)
        {
          Pos pos=v+Dir(d);
          int distancia=dist;
          if (bon_vei(pos) and not visitados[pos.i][pos.j])
          {
            if (cell(pos).type==Rock) distancia+=cell(pos).turns;
            else if (cell(pos).treasure) distancia=0;
            else ++distancia;
            if (vist) dir=Dir(d);
            visitados[pos.i][pos.j]=true;
            q.push({distancia,{pos,dir}});
          }
        }
        vist=false;
      }
    }
  }


  //pos Bon vei per anar
  bool bon_vei(Pos& pos)
  {
    return pos_ok(pos) and cell(pos).type!=Granite and cell(pos).type!=Abyss;
  }
  //No hi ha balrog ni trolls en pos
  void balrog_trolls(vector<vector<bool>> &enemigos)  
  {
    Pos balrog_pos=unit(balrog_id()).pos;
    for (int i=-3; i<=3;++i)
    {
      for (int j=-3; j<=3; ++j)
      {
        if (pos_ok(balrog_pos.i+i,balrog_pos.j+j)) enemigos[balrog_pos.i+i][balrog_pos.j+j]=true;
      }
    }
    
    vector<int> id_trolls=trolls();
    for (uint k=0; k<id_trolls.size(); ++k)
    {
      Pos troll_pos=unit(id_trolls[k]).pos;
      for (int i=-2; i<=2;++i)
      {
        for (int j=-2; j<=2; ++j)
        {
          if (pos_ok(troll_pos.i+i,troll_pos.j+j)) enemigos[troll_pos.i+i][troll_pos.j+j]=true;
        }
      }
    }
  }
  //Comprobar que la unidad es mia o no
  bool unidad_mia(int unit_id)    
  {
    bool ret=false;
    if(unit_id!=-1)
    {
      if(unit(unit_id).player==me()) ret=true;
    }
    else ret=true;
    return ret;
  }



  //metodo dwarvs
  void dwarvs(Unit &u)
  {
    vector<vector<bool>> visitados(60,vector<bool>(60,false));
    priority_queue<pair<int,pair<Pos,Dir>>,vector<pair<int,pair<Pos,Dir>>>,greater<pair<int,pair<Pos,Dir>>>> q;
    balrog_trolls(visitados);
    q.push({0,{u.pos,None}});
    bool vist=true,moved=false;

    //Djsktra para encontrar celda valida 
    while (not q.empty() and not moved)
    {
      Pos v=q.top().second.first;
      Dir dir=q.top().second.second;
      int dist=q.top().first;
      q.pop();
      Cell c=cell(v);

      if ( c.id==-1 and c.owner!=me() and c.type==Cave )
      {
        command(u.id, dir);
        moved=true;
      }
      else 
      {
        for (int d=0; d<8 and not moved; ++d)
        {
          Pos pos=v+Dir(d);
          int distancia=dist;
          if (bon_vei(pos) and not visitados[pos.i][pos.j])
          {
            if (cell(pos).type==Rock) distancia+=cell(pos).turns;
            else ++distancia;
            visitados[pos.i][pos.j]=true;
            if (vist) dir=Dir(d);
            q.push({distancia,{pos,dir}});
          }
        }
        vist=false;
      }
    }
  }


  
  bool bon_vei_wizards (Pos& celda) 
  {
    return pos_ok(celda) and cell(celda.i,celda.j).type != Granite and cell(celda.i,celda.j).type != Abyss and cell(celda.i,celda.j).type != Rock; 
  }
  //Dwarv mio
  bool dwarv_mio(int unit_id)   
  {
    bool ret=false;
    if(unit_id!=-1 and unit(unit_id).type==Dwarf and unit(unit_id).player==me()) ret=true;
    return ret;
  }

  //Metodo wizard
  void wizard(Unit &u)
  {
    vector<vector<bool>> visitados(60,vector<bool>(60,false));
    balrog_trolls(visitados);
    queue<pair<Pos,Dir>> q;
    q.push({u.pos,None});
    bool vist=true,moved=false;
    
    //BFS para encontrar el dwarf mas cerca
    while (not q.empty() and not moved)
    {
      Pos v=q.front().first;
      Dir dir=q.front().second;
      q.pop();

      //Mover wizard si encuentra un dwarv mio 
      if (dwarv_mio(cell(v).id) ) 
      {
        command(u.id,dir);
        moved=true;
      }
      else
      { 
        for (int d=0; d<8 and not moved; d+=2)  //mirar vecinos
        {
          Pos pos=v+Dir(d);
          if (bon_vei_wizards(pos) and not visitados[pos.i][pos.j]) //solo si buen vecino i no visitados
          {
            if (vist) dir=Dir(d);  //Guardar la direccion a la que ir 
            visitados[pos.i][pos.j]=true;
            q.push({pos,dir});
          }
        }
        vist=false;
      }
    }  
  }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () 
  {
    //Dwarfs
    for(unsigned int i=0; i< dwarves(me()).size(); ++i)
    {
      int id=dwarves(me())[i];
      Unit u= unit(id);
      
      bool moved=false;
      for (int d=0; d<8 and not moved;++d)
      {
        if(pos_ok(u.pos+Dir(d)) and cell(u.pos+Dir(d)).treasure ) //si hay tesoro mover y evitar llamar a tesoro()
        {
          command(u.id,Dir(d));
          moved=true;
        }
        else if (pos_ok(u.pos+Dir(d))  and not unidad_mia( cell(u.pos+Dir(d)).id) )  
        {
          command(id,Dir(d));
          moved=true;
        }
      }
      if(round()<100 and not moved)
      {
        enemigos(u);
      }
      else if(not moved) 
      {
        dwarvs(u);  
      }
    }
    //Wizards
    for(unsigned int i=0; i<wizards(me()).size(); ++i)
    {
      int id=wizards(me())[i];
      Unit u= unit(id);

      wizard(u);
    }
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
