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

  //buscar el tesoro mas cerca i ir
  const int INF = 1e9;
  void ir_tesoro (Unit u) 
  {
    int n=rows(), m=cols();
    vector<vector<int>> dist(rows(),vector<int>(cols(), INF));   //pones todos a distancia infinita
    vector<vector<Pos>> previos(n,vector<Pos>(m));
    queue<Pos> Q;
    Q.push(u.pos);
    dist[u.pos.i][u.pos.j] = 0;
    bool trobat=false;
    Pos Destino;
    while (not Q.empty() and not trobat) 
    {
      Pos p = Q.front(); 
      Q.pop();

      if (cell(p).treasure) 
      {
        Destino=p;
        trobat=true;
      }
      for (int d=0; d<8 and not trobat; ++d) 
      {
        Pos v=p+Dir(d);  
        if (bon_vei(v)) 
        {
          Cell c = cell(v);
          if(c.type == Cave or c.type == Outside)
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
            if(dist[v.i][v.j] > dist[p.i][p.j] + c.turns)
            {
              dist[v.i][v.j] = dist[p.i][p.j] + c.turns;
              previos[v.i][v.j]=p;
              Q.push(v); 
            }
          }
        }
      }
    }
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
      else if(col==1) command(u.id,TL);
      else command(u.id,RT);
    }
    else if(fila==-1)
    {
      if(col==0) command(u.id,Bottom);
      else if(col==1) command(u.id,LB);
      else command(u.id,BR);
    }
  }


  //pos Bon vei per anar
  bool bon_vei(Pos& pos)
  {
    return pos_ok(pos) and not balrog_troll(pos) and cell(pos).type!=Granite and cell(pos).type!=Abyss;
  }

  //No hi ha balrog ni trolls en pos
  bool balrog_troll(Pos &pos)
  {
    bool res=false;
    Pos balrog_pos=unit(balrog_id()).pos;
    for (int i=-2; i<=2;++i)
    {
      for (int j=-2; j<=2; ++j)
      {
        if (Pos(balrog_pos.i+i,balrog_pos.j+j) == pos ) res=true;
      }
    }
    
    vector<int> trolls_id=trolls();
    int n=trolls_id.size();
    for (int k=0; k<n and not res; ++k)
    {
      for (int i=-2; i<=2;++i)
      {
        for (int j=-2; j<=2; ++j)
        {
          Pos troll_pos=unit(trolls_id[k]).pos;
          if (Pos(troll_pos.i+i,troll_pos.j+j) == pos) res=true;
        }
      }
    }
    return res;
  }

  bool unidad_mia(int unit_id)      
  {
    bool ret=false;
    if(unit_id!=-1)
    {
      for(unsigned int i=0; i<  wizards(me()).size() and ret==false; ++i)
      {
        if(unit_id ==  wizards(me())[i]) ret=true;
      }
      for(unsigned int i=0; i<  dwarves(me()).size() and ret==false; ++i)
      {
        if(unit_id ==  dwarves(me())[i]) ret=true;
      }
    }
    else ret=true;
    return ret;
  }


  //metodo dwarvs
  void dwarvs(Unit &u)
  {
    vector<vector<bool>> visitados(60,vector<bool>(60,false));
    priority_queue<pair<int,pair<Pos,Dir>>,vector<pair<int,pair<Pos,Dir>>>,greater<pair<int,pair<Pos,Dir>>>> q;
    q.push({0,{u.pos,None}});
    bool vist=true,moved=false;

    while (not q.empty() and not moved)
    {
      Pos v=q.top().second.first;
      Dir dir=q.top().second.second;
      int dist=q.top().first;
      q.pop();
      Cell c=cell(v);

      if ( c.id==-1 and (c.treasure or (c.owner!=me() and c.type==Cave)) )
      {
        command(u.id, dir);
        moved=true;
      }

      else 
      {
        for (int d=0; d<8 and not moved; ++d)
        {
          Pos pos;
          pos=v+Dir(d);
          int distancia=dist;
          if (bon_vei(pos) and pos!=u.pos and not visitados[pos.i][pos.j])
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

  bool bon_vei_wizards (Pos celda) 
  {
    return pos_ok(celda) and not balrog_troll(celda) and cell(celda.i,celda.j).type != Granite and cell(celda.i,celda.j).type != Abyss and cell(celda.i,celda.j).type != Rock; 
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

  //Metodo wizards
  void wizard(Unit &u)
  {
    vector<vector<bool>> visitados(60,vector<bool>(60,false));
    queue<pair<Pos,Dir>> q;
    q.push({u.pos,None});
    bool vist=true,moved=false;

    while (not q.empty() and not moved)
    {
      Pos v=q.front().first;
      Dir dir=q.front().second;
      q.pop();
      Cell c=cell(v);

      if (dwarv_mio(c.id) /*and unit(c.id).health<70*/) 
      {
        command(u.id,dir);
        moved=true;
      }
      
      else
      { 
        for (int d=0; d<8 and not moved; d+=2)
        {
          Pos pos=v+Dir(d);
          if (bon_vei(pos) and pos!=u.pos and not visitados[pos.i][pos.j])
          {
            if (vist) dir=Dir(d);
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
        if(pos_ok(u.pos+Dir(d)) and cell(u.pos+Dir(d)).treasure )
        {
          command(u.id,Dir(d));
          moved=true;
        }
        else if (pos_ok(u.pos+Dir(d))  and not unidad_mia( cell(u.pos+Dir(d)).id) )  //Hacer a mi manera
        {
          command(id,Dir(d));
          moved=true;
        }
        
      }
      if(round()<100 and not moved)
      {
        ir_tesoro(u);
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
