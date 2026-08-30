#ifndef TREINADOR_HPP
#define TREINADOR_HPP

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "grafo.hpp"
#include "ovo.hpp"
#include "pokemon.hpp"

class Treinador {
private:
    static constexpr std::size_t LIMITE_ATIVOS = 6;

    std::string nome_;
    int xp_ = 0;
    int posicao_;
    long long distancia_percorrida_ = 0;
    int ervas_ = 0;
    int pokebolas_equipe_ = 6;
    int pokebola_captura_ = 1;
    std::vector<Pokemon> pokemon_ativos_;
    std::vector<Pokemon> pokemon_no_professor_;
    std::optional<OvoPokemon> ovo_;
    std::unordered_set<std::string> insignias_;

    void avancarTempo(int distancia) {
        distancia_percorrida_ += distancia;
        for (Pokemon& pokemon : pokemon_ativos_) {
            pokemon.avancarDistancia(distancia);
        }
        if (ovo_) {
            ovo_->avancarDistancia(distancia);
            if (ovo_->prontoParaChocar()) {
                Pokemon nascido = ovo_->chocar();
                receberPokemon(std::move(nascido));
                ovo_.reset();
            }
        }
    }

public:
    Treinador(std::string nome, int posicao_inicial, int xp_inicial = 0)
        : nome_(std::move(nome)), xp_(xp_inicial), posicao_(posicao_inicial) {
        if (nome_.empty() || xp_ < 0 || posicao_ < 0) {
            throw std::invalid_argument("Dados iniciais do treinador invalidos");
        }
    }

    const std::string& nome() const { return nome_; }
    int xp() const { return xp_; }
    int posicao() const { return posicao_; }
    long long distanciaPercorrida() const { return distancia_percorrida_; }
    int ervas() const { return ervas_; }
    int pokebolasEquipe() const { return pokebolas_equipe_; }
    int pokebolasCaptura() const { return pokebola_captura_; }
    bool podeCapturar() const { return pokebola_captura_ > 0; }
    std::size_t quantidadeAtivos() const { return pokemon_ativos_.size(); }
    std::size_t quantidadeNoProfessor() const { return pokemon_no_professor_.size(); }
    bool possuiOvo() const { return ovo_.has_value(); }
    std::size_t quantidadeInsignias() const { return insignias_.size(); }
    const std::vector<Pokemon>& pokemonAtivos() const { return pokemon_ativos_; }
    std::vector<Pokemon>& pokemonAtivos() { return pokemon_ativos_; }

    void trocarAtivoComPokemonDoProfessor(std::size_t indice_ativo, std::size_t indice_professor) {
        if (indice_ativo >= pokemon_ativos_.size() || indice_professor >= pokemon_no_professor_.size()) {
            throw std::out_of_range("Indice de pokemon invalido para troca com o Professor Carvalho");
        }
        std::swap(pokemon_ativos_[indice_ativo], pokemon_no_professor_[indice_professor]);
    }

    void receberPokemon(Pokemon pokemon) {
        if (pokemon_ativos_.size() < LIMITE_ATIVOS) {
            pokemon_ativos_.push_back(std::move(pokemon));
        } else {
            pokemon_no_professor_.push_back(std::move(pokemon));
        }
    }

    void coletarOvo(OvoPokemon ovo) {
        if (ovo_) throw std::logic_error("O treinador ja possui um ovo nao chocado");
        ovo_ = std::move(ovo);
    }

    void ganharXpPorVitoria(int xp_oponente) {
        xp_ += xp_oponente >= xp_ ? 3 : 1;
    }

    void adicionarErva() { ++ervas_; }

    bool usarErva() {
        if (ervas_ == 0) return false;
        --ervas_;
        for (Pokemon& pokemon : pokemon_ativos_) pokemon.recuperarComErva();
        return true;
    }

    int quantidadeConscientes() const {
        return static_cast<int>(std::count_if(pokemon_ativos_.begin(), pokemon_ativos_.end(),
                                               [](const Pokemon& p) { return p.podeBatalhar(); }));
    }

    bool podeBatalharContraTreinador() const { return quantidadeConscientes() >= 3; }

    void receberInsignia(const std::string& insignia) {
        if (insignia.empty()) throw std::invalid_argument("Insignia nao pode ser vazia");
        insignias_.insert(insignia);
    }

    bool possuiInsignia(const std::string& insignia) const {
        return insignias_.count(insignia) != 0;
    }

    std::string removerUmaInsignia() {
        if (insignias_.empty()) return {};
        const auto encontrada = insignias_.begin();
        const std::string insignia = *encontrada;
        insignias_.erase(encontrada);
        return insignia;
    }

    Pokemon removerPokemonAtivo(std::size_t indice) {
        if (indice >= pokemon_ativos_.size()) {
            throw std::out_of_range("Indice de pokemon ativo invalido");
        }
        Pokemon removido = std::move(pokemon_ativos_[indice]);
        pokemon_ativos_.erase(pokemon_ativos_.begin() + static_cast<std::ptrdiff_t>(indice));
        return removido;
    }

    void reposicionar(int nova_posicao) {
        if (nova_posicao < 0) throw std::invalid_argument("Posicao do treinador invalida");
        posicao_ = nova_posicao;
    }

    // O deslocamento so e permitido para um vertice adjacente, um passo por vez.
    void moverParaVizinho(const Grafo& mapa, int destino) {
        const auto& adjacencias = mapa.vizinhos(posicao_);
        const auto it = std::find_if(adjacencias.begin(), adjacencias.end(),
                                     [destino](const Aresta& aresta) {
                                         return aresta.destino == destino;
                                     });
        if (it == adjacencias.end()) {
            throw std::invalid_argument("O destino nao e vizinho da posicao atual");
        }
        posicao_ = destino;
        avancarTempo(it->peso);
    }

    // Usado para o tempo consumido por batalhas e tratamento no PMC.
    void passarTempo(int unidades) { avancarTempo(unidades); }
};

#endif
