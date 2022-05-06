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


  //Entrar dentro
  void entrar_dentro(Unit &u)   //añadir control unidades enemigas
  {
    if(u.pos.i==0 or u.pos.i==1) command(u.id,Bottom);
    else if(u.pos.i==59 or u.pos.i==58) command(u.id,Top);

    else if(u.pos.j== 0 or u.pos.j== 1) command(u.id,Right);
    else command(u.id,Left);
  }




  //Si hay Balrog cerca
  bool balrog_cerca(Unit &u) //DEVOLVER LA POSICION DEL BALROG
  {
    bool res=false;
    for(int i=0; i<4; ++i)
    {
      int d=0;
      Pos pos_balrog = unit(balrog_id()).pos;
      if( pos_balrog == (u.pos+ Dir(d))+Dir(d) ) res=true;
      d+=2;
    }
    return res;
  }

  //Buscar el duende mas cerca con menos vida
  void curar_dwarv()
  {

  }


  //Variables i Estr. Dades
  const int INF = 1e9;
  vector<pair<int,int>> dirs = { {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1} };

  bool bon_vei (Pos celda) {
    return celda.i >= 2 and celda.j >= 2 and celda.i < rows() and celda.j < cols() and cell(celda.i,celda.j).type != Granite and cell(celda.i,celda.j).type != Abyss; 
  }

  Pos bfs_tresor (vector<vector<Pos>> &previos, Pos origen) 
  {
    vector<vector<int>> dist(rows(),vector<int>(cols(), INF));   //pones todos a distancia infinita
    queue<Pos> Q;

    Q.push(origen);
    dist[origen.i][origen.j] = 0;
    //cout<<origen.i<<origen.j<<endl;

    while (not Q.empty()) 
    {
      Pos p = Q.front(); 
      Q.pop();
      for (auto& d : dirs) {
        Pos v=p+Pos(d.first,d.second);  //suma la posicio deberia estar bien?
        //cout<<p.i<<v.i<<endl;
        if (bon_vei(v) and dist[v.i][v.j] == INF) 
        {
	        dist[v.i][v.j] = dist[p.i][p.j] + 1;
          previos[v.i][v.j]=p;
	        Q.push(v);
          //cout<<v.i<<v.j<<endl;
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

    Pos Destino = bfs_tresor(previos,u.pos);
    
    //cout<<1<<endl;

    Pos siguiente;
    Pos previo = previos[Destino.i][Destino.j];
    while(previo != u.pos)
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
      else if(col==1) command(u.id,RT);  //aqui
      else command(u.id,RT);
    }
    else if(fila==-1)
    {
      if(col==0) command(u.id,Bottom);
      else if(col==1) command(u.id,LB);
      else command(u.id,BR);
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
      //Si hay balrog escapar
      else if(balrog_cerca(u))
      {
        //escapar

      }
      //Si hay enemigo con poca vida
      else if(c1.id!= -1 and not unidad_mia(c1.id))
      {
        if(unit(c1.id).health < 20 ) 
        {
          command(u.id,Dir(d));
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
      else
      {
        en_cueva(u);      
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
        entrar_dentro(u);
      }
      else
      {
        //ir al dwarv con menos vida mas cerca
        curar_dwarv();
      }
    }
  }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);





