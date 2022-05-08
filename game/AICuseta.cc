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


 void move_dwarves()
 {
    for (unsigned int i=0; i<dwarves(me()).size(); ++i)
    {
      int id=dwarves(me())[i];
      Unit u = unit(id);

      bool moved=false;
      for (int d=0; d<8 and not moved;++d)
      {
        if (pos_ok(u.pos+Dir(d)) and cell(u.pos+Dir(d)).id!=-1 and unit(cell(u.pos+Dir(d)).id).player!=me())
        {
          command(id,Dir(d));
          moved=true;
        }
      }
      if(not moved) 
      {
        tesoro(u);    
      }
    }
  }


  void tesoro(Unit &u)
  {
    vector<vector<bool>> visitados(60,vector<bool>(60,false));
    bool vist=true, moved=false;
    priority_queue<pair<int,pair<Pos,Dir>>,vector<pair<int,pair<Pos,Dir>>>,greater<pair<int,pair<Pos,Dir>>>> q;
    q.push({0,{u.pos,None}});

    while (not q.empty() and not moved)
    {
      Pos v=q.top().second.first;
      Dir dir=q.top().second.second;
      int dist=q.top().first;
      q.pop();
      Cell c=cell(v);

      if (c.id==-1 and not (c.id!=-1 and unit(c.id).player!=me()) and (c.treasure or (c.owner!=me() and c.type==Cave)))
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





//Transformar
void move_wizards()
{
  vector<int> W = wizards(me());
  int n=W.size();
  for (int r=0; r<n; ++r){
      int id=W[r];
      Pos inicial=unit(id).pos;
      vector<vector<bool>> visited(60,vector<bool>(60,false));
      visited[inicial.i][inicial.j]=true;
      balrog_troll(visited); //pone las celdas donde esta el balrog como visitadas para no visitarlas
      queue<pair<Pos,Dir>> qu;
      Dir newdir=None;
      bool primera=true;
      qu.push(make_pair(inicial,newdir));
      bool found=false;
      while (not qu.empty() and not found){
          bool enemigo=false;
          Pos act_pos=qu.front().first;
          Dir act_dir=qu.front().second; qu.pop();
          Cell c=cell(act_pos);
          newdir=act_dir;
          
          if (c.id!=-1 and (unit(c.id).player!=me() or (unit(c.id).player==me() and unit(c.id).type==Wizard))) enemigo=true;
          if (c.id!=-1 and not enemigo and unit(c.id).player==me() and unit(c.id).type == Dwarf) {
              found=true;
              command(id,newdir);
          }
          else { //mirar veins
              for (int p=0; p<4 and not found; ++p){
                  Pos newpos;
                  newpos=act_pos+Dir(p*2);
                  if (posicion_valida_wizard(newpos,visited)){
                      if (primera) {
                          if (p==0) newdir=Bottom;
                          else if (p==1) newdir=Right;
                          else if (p==2) newdir=Top;
                          else if (p==3) newdir=Left;
                      }
                      visited[newpos.i][newpos.j]=true;
                      qu.push(make_pair(newpos,newdir));
                  }
              }
              primera=false;
          }
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






















