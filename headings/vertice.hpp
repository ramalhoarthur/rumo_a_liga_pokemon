#include <iostream>
#include <string>
using namespace std;

class Vertice {
       private: 
              int id; //De 0 a N-1
              string nome;
              string tipo; //CIDADE, ROTA ou LIGA
              bool cmp;
              bool ginasio;
              bool laboratorio;

       public: 
              Vertice (int i, string n, string t, bool c, bool g, bool l) {
                     id = i;
                     nome = n;
                     tipo = t;
                     cmp = c;
                     ginasio = g;
                     laboratorio = l;
              }

              int descobrirIdVertice () const { return id; }
              
              string descobrirNomeVertice () const { return nome; }
                     
              string descobrirTipoVertice () const { return tipo; }
                     
              bool temCmp () const { return cmp; }
                     
              bool temGinasio () const { return ginasio; }
                     
              bool temLaboratorio () const { return laboratorio; }
                     
};