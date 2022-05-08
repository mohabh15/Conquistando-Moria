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



  //Variables i Estr. Dades
  const int INF = 1e9;
  vector<pair<int,int>> dirs = { {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1} };
  bool bon_vei (Pos celda) {
    return pos_ok(celda) and not balrog_troll(celda) and cell(celda.i,celda.j).type != Granite and cell(celda.i,celda.j).type != Abyss; 
  }
  Pos djsktra_tresor (vector<vector<Pos>> &previos, Pos origen) 
  {
    vector<vector<int>> dist(rows(),vector<int>(cols(), INF));   //pones todos a distancia infinita
    priority_queue<Pos> Q;
    //priority_queue<pair<int,pair<Pos,Dir>>,vector<pair<int,pair<Pos,Dir>>>,greater<pair<int,pair<Pos,Dir>>>> q;
    Q.push(origen);
    dist[origen.i][origen.j] = 0;
    while (not Q.empty()) 
    {
      Pos p = Q.top(); 
      Q.pop();
      for (auto& d : dirs) {
        Pos v=p+Pos(d.first,d.second);  
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
          if ( (c.id==-1 and (c.treasure or (c.owner!=me() and c.type==Cave))) or (c.id!=-1 and unit(c.id).player!=me()) ) return v;   //si encuentra un tesoro acaba == al destino/tesoro mas cerca debe debolver esta posicion
        }
      }
    }
    return Pos(0,0);
  }
  void ir_tesoro(Unit &u)
  {
    int n=rows(), m=cols();
    vector<vector<Pos>> previos(n,vector<Pos>(m));

    //Pos Destino = djsktra_tresor(previos,u.pos);
    Pos siguiente = djsktra_tresor(previos,u.pos);;
    //Pos previo = previos[Destino.i][Destino.j];
    /*while(Destino.i!=0 and Destino.j!=0 and previo != u.pos)
    {
      siguiente=previo;
      previo = previos[previo.i][previo.j];
    }*/
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
          if (dwarv_mio(cell(v.i,v.j).id) and unit(cell(v.i,v.j).id).health !=100 ) return Pos(v.i,v.j);   
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
    //cout<<Destino.i<<Destino.j<<endl;
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


  //dwarv en cueva 
  void dwarv_cueva(Unit &u)
  {
    bool moved=false;
    for (int j=0;j<8 and not moved;++j)
    {
        if (pos_ok(u.pos+Dir(j)) and cell(u.pos+Dir(j)).id!=-1 and unit(cell(u.pos+Dir(j)).id).player!=me())
        {
          command(u.id,Dir(j));
          moved=true;
        }
    }
    if(not moved)
    {
      ir_tesoro(u);
    }
  }



   bool posicion_valida_dwarf(Pos& newpos)
   {
     return pos_ok(newpos) and not balrog_troll(newpos) and (cell(newpos).type==Cave or cell(newpos).type==Rock or cell(newpos).type==Outside);
   }
    

    bool balrog_troll(Pos &pos)
    {
      bool res=false;

      Pos bal_pos=unit(balrog_id()).pos;
      for (int p=-3; p<=3;++p)
      {
        for (int k=-3; k<=3; ++k)
        {
          if (Pos(bal_pos.i+p,bal_pos.j+k) == pos ) res=true;
        }
      }
      
      vector<int> trolls_id=trolls();
      int n=trolls_id.size();
      for (int k=0; k<n and not res; ++k)
      {
        for (int p=-2; p<=2;++p)
        {
          for (int u=-2; u<=2; ++u)
          {
            Pos troll_pos=unit(trolls_id[k]).pos;
            if (Pos(troll_pos.i+p,troll_pos.j+u) == pos) res=true;
          }
        }
      }
      return res;
    }

































 void move_dwarves()
 {
    for (unsigned int i=0; i<dwarves(me()).size(); ++i)
    {
      int id=dwarves(me())[i];
      Unit u = unit(id);

      bool enemigo=false;
      for (int d=0; d<8 and not enemigo;++d)
      {
        if (pos_ok(u.pos+Dir(d)) and cell(u.pos+Dir(d)).id!=-1 and unit(cell(u.pos+Dir(d)).id).player!=me())
        {
          enemigo=true;
          command(id,Dir(d));
        }
      }
      if(not enemigo) 
      {
        tesoro(u);    
      }
    }
  }


  void tesoro(Unit &u)
  {
    vector<vector<bool>> visited(60,vector<bool>(60,false));

    bool primera=true, found=false;
    Dir newdir=None;
    priority_queue<pair<int,pair<Pos,Dir>>,vector<pair<int,pair<Pos,Dir>>>,greater<pair<int,pair<Pos,Dir>>>> q;
    q.push({0,{u.pos,None}});

    while (not q.empty() and not found)
    {
      Pos act_pos=q.top().second.first;
      Dir act_dir=q.top().second.second;
      int act_distance=q.top().first;
      q.pop();
      Cell c=cell(act_pos);
      newdir=act_dir;

      if (c.id==-1 and not (c.id!=-1 and unit(c.id).player!=me()) and (c.treasure or (c.owner!=me() and c.type==Cave)))
      {
        found=true;
        command(u.id, act_dir);
      }

      else 
      {
        for (int p=0; p<8 and not found; ++p)
        {
          Pos newpos;
          newpos=act_pos+Dir(p);
          int new_distance=act_distance;
          if (posicion_valida_dwarf(newpos) and newpos!=u.pos and not visited[newpos.i][newpos.j])
          {
            if (cell(newpos).type==Rock) new_distance+=cell(newpos).turns;
            else if (cell(newpos).treasure) new_distance=0;
            else ++new_distance;
            if (primera) newdir=Dir(p);
            visited[newpos.i][newpos.j]=true;
            q.push({new_distance,{newpos,newdir}});
          }
        }
        primera=false;
      }
    }
  }


















  /**
   * Play method, invoked once per each round.
   */
  virtual void play () 
  {
    //mover dwarves
    move_dwarves();
    /*for(unsigned int i=0; i< dwarves(me()).size(); ++i)
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
    }*/
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


























/*
  
    
    void move_dwarves(){
        vector<int> D = dwarves(me());
        int n=D.size();
        for (int i=0; i<n; ++i){
            int id=D[i];
            Pos inicial=unit(id).pos;
            vector<vector<bool>> visited(60,vector<bool>(60,false));
            visited[inicial.i][inicial.j]=true;
            balrog_troll(visited);
            priority_queue<pair<int,pair<Pos,Dir>>,vector<pair<int,pair<Pos,Dir>>>,greater<pair<int,pair<Pos,Dir>>>> q;
            Dir newdir=None;
            q.push({0,{inicial,newdir}});
            bool primera=true, found=false, enemy=false,team=false;
            for (int j=0;j<8 and not enemy;++j){
                if (pos_ok(inicial+Dir(j)) and cell(inicial+Dir(j)).id!=-1 and unit(cell(inicial+Dir(j)).id).player!=me()){
                    enemy=true;
                    newdir=Dir(j);
                }
            }
            if (enemy){
                command(id,newdir);
            }



            else {
                while (not q.empty() and not found){
                    team=false;
                    Pos act_pos=q.top().second.first;
                    Dir act_dir=q.top().second.second;
                    int act_distance=q.top().first;
                    q.pop();
                    Cell c=cell(act_pos);
                    newdir=act_dir;
                    if (c.id!=-1 and unit(c.id).player!=me()) {
                        team=true;
                    }
                    //no unidad  i o hay tesoro o es cueva y no es mia 
                    if (c.id==-1 and not team and (c.treasure or (c.owner!=me() and c.type==Cave))){
                        found=true;
                        command(id, act_dir);
                    }
                    else {
                        for (int p=0; p<8 and not found; ++p){
                            Pos newpos;
                            newpos=act_pos+Dir(p);  //mira el vecino
                            int new_distance=act_distance;
                            if (posicion_valida_dwarf(newpos,visited)){
                                if (cell(newpos).type==Rock) new_distance+=cell(newpos).turns;
                                else if (cell(newpos).treasure) new_distance=0;
                                else ++new_distance;
                                if (primera) {
                                    if (p==0) newdir=Bottom;
                                    else if (p==1) newdir=BR;
                                    else if (p==2) newdir=Right;
                                    else if (p==3) newdir=RT;
                                    else if (p==4) newdir=Top;
                                    else if (p==5) newdir=TL;
                                    else if (p==6) newdir=Left;
                                    else if (p==7) newdir=LB;
                                }
                                visited[newpos.i][newpos.j]=true;
                                q.push({new_distance,{newpos,newdir}});
                            }
                        }
                        primera=false;
                    }
                }
            }

        }
    }

  */