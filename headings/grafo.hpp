#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <iomanip>
#include <limits>
#include <queue>
#include <stdexcept>
#include "vertice.hpp"
struct Aresta {
    int origem;
    int destino;
    int peso;
};

struct Caminho {
    long long distancia = 0;
    std::vector<int> vertices;

    bool existe() const { return !vertices.empty(); }
};

// Grafo ponderado e nao direcionado que representa o mapa da regiao.
class Grafo {
private:
    int n_ = 0;
    int m_ = 0; // Numero de arestas do arquivo; cada uma aparece duas vezes na lista.
    std::vector<Vertice> vertices_;
    std::vector<std::vector<Aresta>> lista_adjacencia_;

    void validarId(int id) const {
        if (id < 0 || id >= n_) {
            throw std::out_of_range("Id de vertice fora dos limites do grafo");
        }
    }

public:
    explicit Grafo(const std::string& caminho_arquivo) {
        std::ifstream arquivo(caminho_arquivo);
        if (!arquivo) {
            throw std::runtime_error("Nao foi possivel abrir o arquivo do mapa: " + caminho_arquivo);
        }

        if (!(arquivo >> n_ >> m_) || n_ <= 0 || m_ < 0) {
            throw std::runtime_error("Cabecalho do mapa invalido: informe N > 0 e M >= 0");
        }

        vertices_.reserve(n_);
        lista_adjacencia_.resize(n_);

        for (int esperado = 0; esperado < n_; ++esperado) {
            int id;
            std::string nome;
            std::string tipo;
            int cmp;
            int ginasio;
            int laboratorio;

            if (!(arquivo >> id >> std::quoted(nome) >> tipo >> cmp >> ginasio >> laboratorio)) {
                throw std::runtime_error("Nao foi possivel ler o vertice " + std::to_string(esperado));
            }
            if (id != esperado) {
                throw std::runtime_error("Os vertices devem estar em ordem, com IDs de 0 a N-1");
            }
            if ((cmp != 0 && cmp != 1) || (ginasio != 0 && ginasio != 1) ||
                (laboratorio != 0 && laboratorio != 1)) {
                throw std::runtime_error("Indicadores de CMP, ginasio e laboratorio devem ser 0 ou 1");
            }

            vertices_.emplace_back(id, nome, tipo, cmp == 1, ginasio == 1, laboratorio == 1);
        }

        for (int i = 0; i < m_; ++i) {
            int origem;
            int destino;
            int peso;

            if (!(arquivo >> origem >> destino >> peso)) {
                throw std::runtime_error("Nao foi possivel ler a aresta " + std::to_string(i));
            }
            validarId(origem);
            validarId(destino);
            if (origem == destino || peso <= 0) {
                throw std::runtime_error("Arestas devem ligar vertices distintos e ter peso positivo");
            }

            lista_adjacencia_[origem].push_back({origem, destino, peso});
            lista_adjacencia_[destino].push_back({destino, origem, peso});
        }
    }

    int quantidadeVertices() const { return n_; }
    int quantidadeArestas() const { return m_; }

    long long somaPesosArestas() const {
        long long soma = 0;
        for (const auto& adjacencias : lista_adjacencia_) {
            for (const Aresta& aresta : adjacencias) {
                soma += aresta.peso;
            }
        }
        return soma / 2;
    }

    const Vertice& vertice(int id) const {
        validarId(id);
        return vertices_[id];
    }

    const std::vector<Aresta>& vizinhos(int id) const {
        validarId(id);
        return lista_adjacencia_[id];
    }

    // Busca em largura: informa se dois pontos pertencem ao mesmo componente.
    bool existeCaminho(int origem, int destino) const {
        validarId(origem);
        validarId(destino);

        std::vector<bool> visitado(n_, false);
        std::queue<int> fila;
        visitado[origem] = true;
        fila.push(origem);

        while (!fila.empty()) {
            const int atual = fila.front();
            fila.pop();
            if (atual == destino) {
                return true;
            }

            for (const Aresta& aresta : lista_adjacencia_[atual]) {
                if (!visitado[aresta.destino]) {
                    visitado[aresta.destino] = true;
                    fila.push(aresta.destino);
                }
            }
        }
        return false;
    }

    // Dijkstra para pesos positivos. Retorna caminho vazio se o destino for inalcançável.
    Caminho caminhoMinimo(int origem, int destino) const {
        validarId(origem);
        validarId(destino);

        const long long infinito = std::numeric_limits<long long>::max();
        std::vector<long long> distancias(n_, infinito);
        std::vector<int> anterior(n_, -1);
        std::priority_queue<std::pair<long long, int>,
                            std::vector<std::pair<long long, int>>,
                            std::greater<std::pair<long long, int>>> fila;

        distancias[origem] = 0;
        fila.push({0, origem});

        while (!fila.empty()) {
            const auto [distancia_atual, atual] = fila.top();
            fila.pop();
            if (distancia_atual != distancias[atual]) {
                continue;
            }
            if (atual == destino) {
                break;
            }

            for (const Aresta& aresta : lista_adjacencia_[atual]) {
                const long long candidata = distancia_atual + aresta.peso;
                if (candidata < distancias[aresta.destino]) {
                    distancias[aresta.destino] = candidata;
                    anterior[aresta.destino] = atual;
                    fila.push({candidata, aresta.destino});
                }
            }
        }

        if (distancias[destino] == infinito) {
            return {};
        }

        Caminho resultado;
        resultado.distancia = distancias[destino];
        for (int atual = destino; atual != -1; atual = anterior[atual]) {
            resultado.vertices.push_back(atual);
        }
        std::reverse(resultado.vertices.begin(), resultado.vertices.end());
        return resultado;
    }
};

#endif
