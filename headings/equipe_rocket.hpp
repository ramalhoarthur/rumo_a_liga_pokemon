#ifndef EQUIPE_ROCKET_HPP
#define EQUIPE_ROCKET_HPP

#include <algorithm>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "grafo.hpp"
#include "treinador.hpp"

class EquipeRocket {
private:
    Treinador treinador_;
    bool invisivel_ = false;
    int tempo_invisivel_restante_ = 0;

    bool localValidoParaAparecer(const Grafo& mapa, int id) const {
        const Vertice& vertice = mapa.vertice(id);
        return !vertice.temCmp() && !vertice.temLaboratorio();
    }

public:
    explicit EquipeRocket(Treinador treinador) : treinador_(std::move(treinador)) {}

    Treinador& treinador() { return treinador_; }
    const Treinador& treinador() const { return treinador_; }
    bool estaInvisivel() const { return invisivel_; }
    int tempoInvisivelRestante() const { return tempo_invisivel_restante_; }
    bool estaNaPosicao(int posicao) const {
        return !invisivel_ && treinador_.posicao() == posicao;
    }

    void moverUmaEtapa(const Grafo& mapa, std::mt19937& gerador) {
        if (invisivel_) return;
        const auto& vizinhos = mapa.vizinhos(treinador_.posicao());
        if (vizinhos.empty()) return;
        const Aresta& escolhida = vizinhos[
            std::uniform_int_distribution<std::size_t>(0, vizinhos.size() - 1)(gerador)];
        treinador_.moverParaVizinho(mapa, escolhida.destino);
    }

    void avancarTempo(const Grafo& mapa, int tempo, std::mt19937& gerador) {
        if (tempo < 0) throw std::invalid_argument("Tempo nao pode ser negativo");
        treinador_.passarTempo(tempo);
        if (!invisivel_) return;

        tempo_invisivel_restante_ -= tempo;
        if (tempo_invisivel_restante_ <= 0) {
            std::vector<int> candidatos;
            for (int id = 0; id < mapa.quantidadeVertices(); ++id) {
                if (localValidoParaAparecer(mapa, id)) candidatos.push_back(id);
            }
            if (candidatos.empty()) throw std::logic_error("Nao ha local valido para a Equipe Rocket reaparecer");
            treinador_.reposicionar(candidatos[
                std::uniform_int_distribution<std::size_t>(0, candidatos.size() - 1)(gerador)]);
            invisivel_ = false;
            tempo_invisivel_restante_ = 0;
        }
    }

    // Ao perder, a equipe e enviada para uma posicao aleatoria distante do confronto.
    void reposicionarLongeDo(const Grafo& mapa, int origem, std::mt19937& gerador) {
        long long maior_distancia = -1;
        std::vector<std::pair<int, long long>> alcancaveis;
        for (int id = 0; id < mapa.quantidadeVertices(); ++id) {
            if (!localValidoParaAparecer(mapa, id)) continue;
            const Caminho caminho = mapa.caminhoMinimo(origem, id);
            if (!caminho.existe()) continue;
            maior_distancia = std::max(maior_distancia, caminho.distancia);
            alcancaveis.push_back({id, caminho.distancia});
        }
        if (alcancaveis.empty()) throw std::logic_error("Nao ha destino para a Equipe Rocket");

        std::vector<int> distantes;
        for (const auto& [id, distancia] : alcancaveis) {
            if (distancia * 2 >= maior_distancia) distantes.push_back(id);
        }
        treinador_.reposicionar(distantes[
            std::uniform_int_distribution<std::size_t>(0, distantes.size() - 1)(gerador)]);
        invisivel_ = false;
        tempo_invisivel_restante_ = 0;
    }

    // Ao vencer, foge e permanece invisivel antes de surgir em local aleatorio.
    void fugirEficarInvisivel(std::mt19937& gerador) {
        invisivel_ = true;
        tempo_invisivel_restante_ = std::uniform_int_distribution<int>(20, 100)(gerador);
    }
};

#endif
