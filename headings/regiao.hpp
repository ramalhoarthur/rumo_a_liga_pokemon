#ifndef REGIAO_HPP
#define REGIAO_HPP

#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "configuracao_regiao.hpp"
#include "pokemon.hpp"
#include "treinador.hpp"

struct PokemonSelvagem {
    Pokemon pokemon;
    int posicao;
    bool atributos_balanceados = false;
};

struct ErvaRegional {
    int posicao;
};

struct OvoRegional {
    OvoPokemon ovo;
    int posicao;
};

// Cria as entidades cuja posicao e atributos devem ser aleatorios no inicio.
class Regiao {
private:
    Grafo mapa_;
    std::vector<PokemonSelvagem> selvagens_;
    std::vector<Treinador> treinadores_;
    std::vector<ErvaRegional> ervas_;
    std::vector<OvoRegional> ovos_;

    static Pokemon criarPokemonAleatorio(const std::vector<EspeciePokemon>& especies,
                                         std::mt19937& gerador) {
        if (especies.empty()) throw std::logic_error("Nao ha especies cadastradas");
        const EspeciePokemon& especie =
            especies[std::uniform_int_distribution<std::size_t>(0, especies.size() - 1)(gerador)];
        return Pokemon(especie, std::uniform_int_distribution<int>(10, 30)(gerador),
                       std::uniform_int_distribution<int>(10, 30)(gerador),
                       std::uniform_int_distribution<int>(20, 100)(gerador),
                       std::uniform_int_distribution<int>(0, 999)(gerador));
    }

public:
    Regiao(const ConfiguracaoRegiao& configuracao, unsigned int semente = std::random_device{}())
        : mapa_(configuracao.caminho_mapa) {
        std::mt19937 gerador(semente);
        const auto posicao_aleatoria = [&]() {
            return std::uniform_int_distribution<int>(0, mapa_.quantidadeVertices() - 1)(gerador);
        };

        selvagens_.reserve(configuracao.quantidade_selvagens);
        for (int i = 0; i < configuracao.quantidade_selvagens; ++i) {
            selvagens_.push_back({criarPokemonAleatorio(configuracao.especies, gerador), posicao_aleatoria()});
        }

        treinadores_.reserve(configuracao.quantidade_treinadores);
        for (int i = 0; i < configuracao.quantidade_treinadores; ++i) {
            Treinador treinador("Treinador " + std::to_string(i + 1), posicao_aleatoria(),
                                 std::uniform_int_distribution<int>(0, 999)(gerador));
            for (int j = 0; j < 3; ++j) {
                treinador.receberPokemon(criarPokemonAleatorio(configuracao.especies, gerador));
            }
            treinadores_.push_back(std::move(treinador));
        }

        ervas_.reserve(configuracao.quantidade_ervas);
        for (int i = 0; i < configuracao.quantidade_ervas; ++i) {
            ervas_.push_back({posicao_aleatoria()});
        }

        ovos_.reserve(configuracao.quantidade_ovos);
        for (int i = 0; i < configuracao.quantidade_ovos; ++i) {
            const EspeciePokemon& especie = configuracao.especies[
                std::uniform_int_distribution<std::size_t>(0, configuracao.especies.size() - 1)(gerador)];
            ovos_.push_back({OvoPokemon(especie, std::uniform_int_distribution<int>(10, 30)(gerador),
                                        std::uniform_int_distribution<int>(10, 30)(gerador)),
                              posicao_aleatoria()});
        }
    }

    const Grafo& mapa() const { return mapa_; }
    const std::vector<PokemonSelvagem>& selvagens() const { return selvagens_; }
    std::vector<PokemonSelvagem>& selvagens() { return selvagens_; }
    const std::vector<Treinador>& treinadores() const { return treinadores_; }
    std::vector<Treinador>& treinadores() { return treinadores_; }
    const std::vector<ErvaRegional>& ervas() const { return ervas_; }
    std::vector<ErvaRegional>& ervas() { return ervas_; }
    const std::vector<OvoRegional>& ovos() const { return ovos_; }
    std::vector<OvoRegional>& ovos() { return ovos_; }

    // Cada entidade movel escolhe um dos vizinhos e avanca apenas uma aresta.
    void moverEntidadesUmaEtapa(std::mt19937& gerador) {
        const auto proximoVizinho = [&](int origem) {
            const auto& vizinhos = mapa_.vizinhos(origem);
            if (vizinhos.empty()) return origem;
            return vizinhos[std::uniform_int_distribution<std::size_t>(0, vizinhos.size() - 1)(gerador)].destino;
        };
        for (PokemonSelvagem& selvagem : selvagens_) {
            selvagem.posicao = proximoVizinho(selvagem.posicao);
        }
        for (Treinador& treinador : treinadores_) {
            const int destino = proximoVizinho(treinador.posicao());
            if (destino != treinador.posicao()) treinador.moverParaVizinho(mapa_, destino);
        }
    }
};

#endif
