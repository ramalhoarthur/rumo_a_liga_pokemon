#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include "vertice.hpp"
using namespace std;

struct Aresta {
       int origem;
       int destino;
       int peso;
};

class Grafo {
       private:
              int n;
              int m;
              vector<Vertice> vertices;
              vector<vector<Aresta>> lista_adjacencia; 
       public:
              Grafo (string entrada) {
                     ifstream arquivo(entrada);

                     if(!arquivo.is_open()) {
                            throw runtime_error("Erro ao abrir texto de entrada do grafo");
                     }
                     
                     arquivo >> n >> m;

                     for (int i = 0 ; i < n ; i++) {
                            int id;
                            string nome, tipo;
                            bool cmp, ginasio, laboratorio;

                            arquivo >> id >> quoted(nome) >> tipo >> cmp >> ginasio >> laboratorio;

                            Vertice v(id, nome, tipo, cmp, ginasio, laboratorio);
                            vertices.push_back(v);
                     }

                     for (int i = 0; i < m; i++) {
                            int origem, destino, peso;
                            arquivo >> origem >> destino >> peso;

                            Aresta ida;
                            ida.origem = origem;
                            ida.destino = destino;
                            ida.peso = peso;
                            lista_adjacencia[origem].push_back(ida);

                            Aresta volta;
                            volta.origem = destino;
                            volta.destino = origem;
                            volta.peso = peso;
                            lista_adjacencia[destino].push_back(volta);
                            }
              }
};