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


  //metodo dwarvs
  void dwarvs(Unit &u)
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





  //Metodo wizards
  void wizard(Unit &u)
  {
    
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
        if (pos_ok(u.pos+Dir(d)) and cell(u.pos+Dir(d)).id!=-1 and unit(cell(u.pos+Dir(d)).id).player!=me())
        {
          command(id,Dir(d));
          moved=true;
        }
      }
      if(not moved) 
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
