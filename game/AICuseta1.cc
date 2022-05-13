#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Cuseta1


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



  //Variables i Estr. Dades
  //const int INF = 1e9;
  //vector<pair<int,int>> dirs = { {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1} };
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














  //Buscar el duende mas cerca con vida < 100
  vector<pair<int,int>> dirs_wizards = { {0,1},{1,0},{0,-1},{-1,0}};
   const int INF = 1e9;
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
          if (dwarv_mio(cell(v.i,v.j).id)  ) return Pos(v.i,v.j);   
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
    for (int d = 0; d < 8 and not moved; ++d) 
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

  void dwarv_cueva(Unit &u)
  {
    bool moved=false;
    for (int d = 0; d < 8 and not moved; ++d) 
    {
      Pos p1 = u.pos + Dir(d);
      Cell c1 = cell(p1);

      if(c1.treasure)
      {
        command(u.id,Dir(d));
        moved=true;
      }
    }
    if(not moved)
    {
      ir_tesoro(u);   
    }
  }








                            




  bool bon_vei(Pos& pos)
  {
    return pos_ok(pos) and not balrog_troll(pos) and cell(pos).type!=Granite and cell(pos).type!=Abyss;
  }

  bool balrog_troll(Pos &pos)
  {
    bool res=false;
    Pos balrog_pos=unit(balrog_id()).pos;
    for (int i=-3; i<=3;++i)
    {
      for (int j=-3; j<=3; ++j)
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













  /**
   * Play method, invoked once per each round.
   */
  virtual void play () 
  {
    //mover dwarves
    //ir_dwarves();
    for(unsigned int i=0; i< dwarves(me()).size(); ++i)
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
        dwarv_cueva(u);      
      }
    }
    //mover wizards
    //move_wizards();
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












/*

void dwarvs2(Unit &u)
  {
    vector<vector<bool>> visitados(60,vector<bool>(60,false));
    queue<pair<Pos,Dir>> q;
    vector<vector<int>> dist(rows(),vector<int>(cols(), INF));   
    q.push({u.pos,None});
    bool vist=true,moved=false;
    
    
    
    while (not q.empty() and not moved)
    {
      Pos p=q.front().first;
      Dir dir=q.front().second;
      dist[p.i][p.j]=0;
      q.pop();
      Cell c=cell(p);

      if ( c.id==-1 and (c.treasure or (c.owner!=me() and c.type==Cave)) )
      {
        command(u.id, dir);
        moved=true;
      }

      else 
      {
        for (int d=0; d<8 and not moved; ++d)
        {
          Pos v=p+Dir(d);  
          if (bon_vei(v) and not visitados[v.i][v.j]) 
          {
            Cell c = cell(v);
            if(c.type == Cave or c.type == Outside)
            {
              if(dist[v.i][v.j] > dist[p.i][p.j] + 1)
              {
                dist[v.i][v.j] = dist[p.i][p.j] + 1;
                if (vist) dir=Dir(d);
                visitados[v.i][v.j]=true;
                q.push({v,dir}); 
              }
            }
            else
            {
              if(dist[v.i][v.j] > dist[p.i][p.j] + c.turns)
              {
                dist[v.i][v.j] = dist[p.i][p.j] + c.turns;
                if (vist) dir=Dir(d);
                visitados[v.i][v.j]=true;
                q.push({v,dir}); 
              }
            }
          }
        }
        vist=false;
      }
    }
  }


*/







/*
IMPORTANTE

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
        if (bon_vei(v) ) 
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









*/





