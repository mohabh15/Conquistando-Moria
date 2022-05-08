#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Git


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
  
    bool posicion_valida_dwarf(const Pos& newpos,const vector<vector<bool>>& visited){
        return pos_ok(newpos) and not visited[newpos.i][newpos.j] and (cell(newpos).type==Cave or cell(newpos).type==Rock or cell(newpos).type==Outside);
    }
  
    bool posicion_valida_wizard(const Pos& newpos,const vector<vector<bool>>& visited){
        return pos_ok(newpos) and not visited[newpos.i][newpos.j] and (cell(newpos).type==Cave or cell(newpos).type==Outside);
    }
    
    void balrog_troll(vector<vector<bool>>& visited){
        Pos bal_pos=unit(balrog_id()).pos;
        for (int p=-3; p<=3;++p){
            for (int k=-3; k<=3; ++k){
                if (pos_ok((bal_pos.i)+p,(bal_pos.j)+k)) visited[(bal_pos.i)+p][(bal_pos.j)+k]=true;
            }
        }
        vector<int> trolls_id=trolls();
        int n=trolls_id.size();
        for (int k=0; k<n; ++k){
            for (int p=-2; p<=2;++p){
                for (int u=-2; u<=2; ++u){
                    Pos troll_pos=unit(trolls_id[k]).pos;
                    if (pos_ok((troll_pos.i)+p,(troll_pos.j)+u)) visited[(troll_pos.i)+p][(troll_pos.j)+u]=true;
                }
            }
        }
    }
    
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
                            newpos=act_pos+Dir(p);
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

    void move_wizards(){
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
  virtual void play () {
      move_dwarves();
      move_wizards();
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);