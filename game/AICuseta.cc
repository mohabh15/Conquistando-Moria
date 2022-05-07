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
    for(unsigned int i=0; i<  wizards(me()).size() and ret==false; ++i)
    {
      if(unit_id ==  wizards(me())[i]) ret=true;
    }
    for(unsigned int i=0; i<  dwarves(me()).size() and ret==false; ++i)
    {
      if(unit_id ==  dwarves(me())[i]) ret=true;
    }
    return ret;
  }
  //Dwarv mio
  bool dwarv_mio(int unit_id)      
  {
    bool ret=false;
    for(unsigned int i=0; i<  dwarves(me()).size() and ret==false; ++i)
    {
      if(unit_id ==  dwarves(me())[i]) ret=true;
    }
    return ret;
  }


  //Entrar dentro
  void entrar_dentro(Unit &u)   //añadir control unidades enemigas
  {
    if(u.pos.i==0 or u.pos.i==1) command(u.id,Bottom);
    else if(u.pos.i==59 or u.pos.i==58) command(u.id,Top);

    else if(u.pos.j== 0 or u.pos.j== 1) command(u.id,Right);
    else command(u.id,Left);
  }

  //Si hay Balrog cerca
  vector<pair<int,int>> dirs_balrog = { {0,2},{1,2},{2,2},{-1,2},{-2,2},{0,-2},{1,-2},{2,-2},{-1,-2},{-2,-2},{2,0},{2,1},{2,-1},{-2,0},{-2,1},{2,-1}};
  bool balrog_cerca(Unit &u, Pos &balrog) 
  {
   bool res=false;
   for (auto& d : dirs_balrog) 
   {
     balrog = u.pos+Pos(d.first,d.second); 
     if(cell(balrog).id == balrog_id()) res=true;
   }
    return res;
  }

  //Si hay troll cerca
  bool troll(Cell &enemigo) 
  {
    bool res=false;
    for(unsigned int i=0; i< trolls().size(); ++i)
    {
      if(enemigo.id == trolls()[i]) res=true;
    }   
    return res;
  }


  //Variables i Estr. Dades
  const int INF = 1e9;
  vector<pair<int,int>> dirs = { {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1} };
  bool bon_vei (Pos celda) {
    return celda.i >= 0 and celda.j >= 0 and celda.i < rows() and celda.j < cols() and cell(celda.i,celda.j).type != Granite and cell(celda.i,celda.j).type != Abyss; 
  }

  Pos djsktra_tresor (vector<vector<Pos>> &previos, Pos origen) 
  {
    vector<vector<int>> dist(rows(),vector<int>(cols(), INF));   //pones todos a distancia infinita
    queue<Pos> Q;
    Q.push(origen);
    dist[origen.i][origen.j] = 0;
    while (not Q.empty()) 
    {
      Pos p = Q.front(); 
      Q.pop();
      for (auto& d : dirs) {
        Pos v=p+Pos(d.first,d.second);  
        if (bon_vei(v)) 
        {
          if(cell(v).type == Cave or cell(v).type == Outside)
          {
            if(dist[v.i][v.j] > dist[p.i][p.j] + 1)
            {
              dist[v.i][v.j] = dist[p.i][p.j] + 1;
              previos[v.i][v.j]=p;
              Q.push(v); 
            }
          }
          else
          {
            if(dist[v.i][v.j] > dist[p.i][p.j] + cell(v).turns)
            {
              dist[v.i][v.j] = dist[p.i][p.j] + cell(v).turns;
              previos[v.i][v.j]=p;
              Q.push(v); 
            }
          }
          if (cell(v.i,v.j).treasure) return Pos(v.i,v.j);   //si encuentra un tesoro acaba == al destino/tesoro mas cerca debe debolver esta posicion
        }
      }
    }
    return Pos(0,0);
  }
  

  void ir_tesoro(Unit &u)
  {
    int n=rows(), m=cols();
    vector<vector<Pos>> previos(n,vector<Pos>(m));

    Pos Destino = djsktra_tresor(previos,u.pos);
    Pos siguiente;
    Pos previo = previos[Destino.i][Destino.j];
    while(Destino.i!=0 and Destino.j!=0 and previo != u.pos)
    {
      siguiente=previo;
      previo = previos[previo.i][previo.j];
    }
    //Mover a siguiente     
    int fila = u.pos.i-siguiente.i;     
    int col = u.pos.j-siguiente.j;
    if(fila==0)
    {
      if(col==1) command(u.id,Left);
      else command(u.id,Right);
    }
    else if(fila==1)      
    {
      if(col==0) command(u.id,Top);
      else if(col==1) command(u.id,TL);  //Arreglar no puede la misma direccion las dos -> ESTA BIEN LA DIRECCION?
      else command(u.id,RT);
    }
    else if(fila==-1)
    {
      if(col==0) command(u.id,Bottom);
      else if(col==1) command(u.id,LB);
      else command(u.id,BR);
    }
  }


  //Buscar el duende mas cerca -> mejora: con menos vida
  vector<pair<int,int>> dirs_wizards = { {0,1},{1,0},{0,-1},{-1,0}};
  bool bon_vei_wizards (Pos celda) {
  return celda.i >= 2 and celda.j >= 2 and celda.i < rows() and celda.j < cols() and cell(celda.i,celda.j).type != Granite and cell(celda.i,celda.j).type != Abyss and cell(celda.i,celda.j).type != Rock; 
  }
  Pos buscar_dwarv (vector<vector<Pos>> &previos, Pos origen) 
  {
    vector<vector<int>> dist(rows(),vector<int>(cols(), INF));   //pones todos a distancia infinita
    queue<Pos> Q;
    Q.push(origen);
    dist[origen.i][origen.j] = 0;
    while (not Q.empty()) 
    {
      Pos p = Q.front(); 
      Q.pop();
      for (auto& d : dirs_wizards) 
      {
        Pos v=p+Pos(d.first,d.second);  
        if (bon_vei_wizards(v) and dist[v.i][v.j] == INF) 
        {
	        dist[v.i][v.j] = dist[p.i][p.j] + 1;
          previos[v.i][v.j]=p;
	        Q.push(v);
          if (dwarv_mio(cell(v.i,v.j).id)) return Pos(v.i,v.j);   //si encuentra un tesoro acaba == al destino/tesoro mas cerca debe debolver esta posicion
        }
      }
    }  
    return Pos(0,0);
  }
  void ir_dwarv(Unit &u)
  {
    int n=rows(), m=cols();
    vector<vector<Pos>> previos(n,vector<Pos>(m));

    Pos Destino = buscar_dwarv(previos,u.pos);
    Pos siguiente;
    Pos previo = previos[Destino.i][Destino.j];
    while(Destino.i!=0 and Destino.j!=0 and previo != u.pos)
    {
      siguiente=previo;
      previo = previos[previo.i][previo.j];
    }
    //Mover a siguiente
    int fila = u.pos.i-siguiente.i;     
    int col = u.pos.j-siguiente.j;
    if(fila==0)
    {
      if(col==1) command(u.id,Left);
      else command(u.id,Right);
    }
    else if(col==0)
    {
      if(fila==1) command(u.id,Top);
      else command(u.id,Bottom);
    }
  }



  void wizard_cueva(Unit &u)
  {
    bool moved=false;
    Pos balrog;
    //Si hay balrog_orco_troll escapar
    if(balrog_cerca(u,balrog))
    {
      //escapar
      if(balrog.j<u.pos.j)
      {
        command(u.id,Bottom);
      }
      else if(balrog.j>u.pos.j)
      {
        command(u.id,Top);
      }
      else
      {
        if(balrog.i<u.pos.i) command(u.id,Right);
        else command(u.id,Left);
      }
      moved=true;
    }
    for (int d = 0; d < 7 and not moved; ++d) 
    {
      Pos p1 = u.pos + Dir(d);
      Cell c1 = cell(p1);
      //Si hay enemigo escapar
      if(c1.id!= -1 and not unidad_mia(c1.id))   //Arreglar para si hay muro esquivar
      {
        if(d%2==0)
        {
          if(d>3) command(u.id,Dir(d-4)); 
          else command(u.id,Dir(d+4));
          moved=true;
        }
        else 
        {
          if(d==3 or d==1) command(u.id,Left); 
          else command(u.id,Right);
          moved=true; 
        }
      }
    }
    if(not moved)
    {
      ir_dwarv(u);   
    }
  }

  //dwarv en cueva 
  void dwarv_cueva(Unit &u)
  {
    bool moved=false;
    Pos balrog;
    //Si hay balrog_orco_troll escapar
    if(balrog_cerca(u,balrog))
    {
      //escapar
      if(balrog.j<u.pos.j)
      {
        command(u.id,Bottom);
      }
      else if(balrog.j>u.pos.j)
      {
        command(u.id,Top);
      }
      else
      {
        if(balrog.i<u.pos.i) command(u.id,Right);
        else command(u.id,Left);
      }
      moved=true;
    }
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
      //Si hay enemigo con poca vida
      else if(c1.id!= -1 and not unidad_mia(c1.id))
      {
        if(unit(c1.id).health < 101 and not troll(c1)) //mejora: si es una unidad de otro clan y la diferencia de vida es 20 0 mayor
        {
          command(u.id,Dir(d));
          moved=true;
        }
        else    //añadir que escapi por donde no hay cueva y no si hay roca
        {
          if(d>3) command(u.id,Dir(d-4)); 
          else command(u.id,Dir(d+4));
          moved=true;
        }
      }
    }
    if(not moved)
    {
      ir_tesoro(u);
    }
  }
   

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () 
  {
    //mover dwarves
    for(unsigned int i=0; i< dwarves(me()).size(); ++i)
    {
      int id=dwarves(me())[i];
      Unit u= unit(id);

      //si esta en el exterior
      if(cell(u.pos).type==Outside)
      {
        entrar_dentro(u);
      }
      //durante las primera rondas juntar
      /*else if(round()<11)
      {
        //juntarse

      }*/
      else
      {
        dwarv_cueva(u);      
      }
    }
    //mover wizards
    for(unsigned int i=0; i<wizards(me()).size(); ++i)
    {
      int id=wizards(me())[i];
      Unit u= unit(id);

      //si esta en el exterior
      if(cell(u.pos).type==Outside)
      {
        ir_dwarv(u);
      }
      else
      {
        wizard_cueva(u);
      }
    }
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);




