#include <bits/stdc++.h>
using namespace std;

class Pokemon{
    private:
     string nome;
     int ID;
     string tipo;
     int IDtipo;
     int HP;
     int AP;
     int DP;
     int XP;
     string status;
     int ID_evol;
     int dist;

     public:
      Pokemon(string nome, int ID, string tipo, int IDtipo, int HP, int AP, int DP, int XP, string status, int ID_evol, int dist){
        this->nome = nome;
        this->ID = ID;
        this->tipo = tipo;
        this->IDtipo = IDtipo;
        this->HP = HP;
        this->AP = AP;
        this->DP = DP;
        this->XP = XP;
        this->status = status;
        this->ID_evol = ID_evol;
        this->dist = dist;
      }

      void XP_Andando(){
        if(this->dist >= 100){
            this->XP++;
            this->dist = this->dist - 100;
        }
      }

      void Demaiar(){
        if(this->status == "normal" && this->HP < 20){
            this->status = "desmaiado";
        }
      }

      void evoluir(){
        if(this->XP >= 1000){
            this->ID = this->ID_evol;
            this->AP = this->AP*1.3;
            this->DP = this->DP*1.3; 
        }
      }
      
};