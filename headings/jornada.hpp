#ifndef JORNADA_HPP
#define JORNADA_HPP

#include <algorithm>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "batalha.hpp"
#include "equipe_rocket.hpp"
#include "grafo.hpp"
#include "lider_ginasio.hpp"
#include "treinador.hpp"

class Jornada {
private:
    Grafo mapa_;
    Treinador jogador_;
    long long prazo_inscricao_;
    std::optional<long long> inicio_prazo_inscricao_;
    std::vector<LiderGinasio> lideres_;
    std::optional<EquipeRocket> equipe_rocket_;
    std::mt19937 gerador_;

    bool localPermiteBatalha() const {
        const Vertice& local = mapa_.vertice(jogador_.posicao());
        return !local.temCmp() && !local.temLaboratorio();
    }

    void iniciarPrazoInscricaoSeNecessario() {
        if (!inicio_prazo_inscricao_ && jogador_.quantidadeInsignias() >= 8) {
            inicio_prazo_inscricao_ = jogador_.distanciaPercorrida();
        }
    }

public:
    struct ResultadoConfrontoRocket {
        VencedorDuelo vencedor;
        bool pokemon_roubado;
        std::string insignia_roubada;
    };

    Jornada(Grafo mapa, Treinador jogador, long long prazo_inscricao, unsigned int semente = 0)
        : mapa_(std::move(mapa)),
          jogador_(std::move(jogador)),
          prazo_inscricao_(prazo_inscricao),
          gerador_(semente) {
        const long long soma = mapa_.somaPesosArestas();
        if (prazo_inscricao_ < 10 * soma || prazo_inscricao_ > 15 * soma) {
            throw std::invalid_argument("Prazo de inscricao deve estar entre 10 e 15 vezes a soma dos pesos");
        }
        (void)mapa_.vertice(jogador_.posicao());
        iniciarPrazoInscricaoSeNecessario();
    }

    const Grafo& mapa() const { return mapa_; }
    const Treinador& jogador() const { return jogador_; }
    Treinador& jogador() { return jogador_; }
    long long prazoInscricao() const { return prazo_inscricao_; }
    bool prazoInscricaoIniciado() const { return inicio_prazo_inscricao_.has_value(); }
    long long tempoDecorrido() const {
        return inicio_prazo_inscricao_ ? jogador_.distanciaPercorrida() - *inicio_prazo_inscricao_ : 0;
    }
    bool prazoExpirou() const { return prazoInscricaoIniciado() && tempoDecorrido() > prazo_inscricao_; }

    void adicionarLider(LiderGinasio lider) {
        const Vertice& local = mapa_.vertice(lider.ginasio());
        if (!local.temGinasio()) {
            throw std::invalid_argument("O vertice do lider precisa conter um ginasio");
        }
        lideres_.push_back(std::move(lider));
    }

    // Indices dos lideres que podem ser desafiados no local atual do jogador.
    std::vector<std::size_t> lideresNoLocalDoJogador() const {
        std::vector<std::size_t> indices;
        for (std::size_t i = 0; i < lideres_.size(); ++i) {
            if (lideres_[i].treinador().posicao() == jogador_.posicao()) indices.push_back(i);
        }
        return indices;
    }

    void adicionarEquipeRocket(EquipeRocket equipe) {
        (void)mapa_.vertice(equipe.treinador().posicao());
        equipe_rocket_ = std::move(equipe);
    }

    bool equipeRocketEstaNaPosicaoDoJogador() const {
        return equipe_rocket_ && equipe_rocket_->estaNaPosicao(jogador_.posicao());
    }

    const EquipeRocket* equipeRocket() const {
        return equipe_rocket_ ? &*equipe_rocket_ : nullptr;
    }

    void moverJogador(int destino) {
        const long long antes = jogador_.distanciaPercorrida();
        jogador_.moverParaVizinho(mapa_, destino);
        const int decorrido = static_cast<int>(jogador_.distanciaPercorrida() - antes);
        for (LiderGinasio& lider : lideres_) lider.avancarTempo(mapa_, decorrido);
        if (equipe_rocket_) {
            equipe_rocket_->avancarTempo(mapa_, decorrido, gerador_);
            equipe_rocket_->moverUmaEtapa(mapa_, gerador_);
        }
    }

    bool desafiarLider(std::size_t indice, bool lider_desiste = false) {
        if (indice >= lideres_.size()) throw std::out_of_range("Lider de ginasio inexistente");
        if (!localPermiteBatalha()) {
            throw std::logic_error("Batalhas sao proibidas no CMP e no laboratorio");
        }
        LiderGinasio& lider = lideres_[indice];
        if (lider.treinador().posicao() != jogador_.posicao()) {
            throw std::logic_error("O lider nao esta na mesma posicao do jogador");
        }
    
        Batalha batalha(gerador_);
        const ResultadoBatalhaTreinadores resultado =
            batalha.treinadores(lider.treinador(), jogador_, lider_desiste);
        jogador_.passarTempo(1);
        for (LiderGinasio& outro_lider : lideres_) outro_lider.avancarTempo(mapa_, 1);
        if (resultado.vencedor == VencedorDuelo::Desafiante) {
            jogador_.receberInsignia(lider.insignia());
            iniciarPrazoInscricaoSeNecessario();
            return true;
        }
        return false;
    }

    int tratarEquipeNoPMC() {
        if (!mapa_.vertice(jogador_.posicao()).temCmp()) {
            throw std::logic_error("O tratamento so pode ser realizado no Centro Medico Pokemon");
        }
        const int tempo_tratamento = std::uniform_int_distribution<int>(10, 50)(gerador_);
        jogador_.passarTempo(tempo_tratamento);
        for (Pokemon& pokemon : jogador_.pokemonAtivos()) pokemon.tratarNoPMC();
        for (LiderGinasio& lider : lideres_) lider.avancarTempo(mapa_, tempo_tratamento);
        if (equipe_rocket_) equipe_rocket_->avancarTempo(mapa_, tempo_tratamento, gerador_);
        return tempo_tratamento;
    }

    ResultadoConfrontoRocket enfrentarEquipeRocket() {
        if (!equipe_rocket_) throw std::logic_error("A Equipe Rocket nao foi configurada");
        if (!localPermiteBatalha()) throw std::logic_error("Batalhas sao proibidas no CMP e no laboratorio");
        if (!equipe_rocket_->estaNaPosicao(jogador_.posicao())) {
            throw std::logic_error("A Equipe Rocket nao esta nesta posicao");
        }

        Batalha batalha(gerador_);
        const ResultadoBatalhaTreinadores resultado =
            batalha.treinadores(equipe_rocket_->treinador(), jogador_);
        jogador_.passarTempo(1);
        for (LiderGinasio& lider : lideres_) lider.avancarTempo(mapa_, 1);

        ResultadoConfrontoRocket retorno{resultado.vencedor, false, {}};
        if (resultado.vencedor == VencedorDuelo::Desafiante) {
            equipe_rocket_->reposicionarLongeDo(mapa_, jogador_.posicao(), gerador_);
        } else {
            const bool pode_roubar_insignia = jogador_.quantidadeInsignias() > 0;
            const bool roubar_insignia = pode_roubar_insignia &&
                                         std::bernoulli_distribution(0.5)(gerador_);
            if (roubar_insignia) {
                retorno.insignia_roubada = jogador_.removerUmaInsignia();
                equipe_rocket_->treinador().receberInsignia(retorno.insignia_roubada);
            } else if (jogador_.quantidadeAtivos() > 0) {
                const std::size_t indice = std::uniform_int_distribution<std::size_t>(
                    0, jogador_.quantidadeAtivos() - 1)(gerador_);
                equipe_rocket_->treinador().receberPokemon(jogador_.removerPokemonAtivo(indice));
                retorno.pokemon_roubado = true;
            }
            equipe_rocket_->fugirEficarInvisivel(gerador_);
        }
        equipe_rocket_->avancarTempo(mapa_, 1, gerador_);
        return retorno;
    }

    bool podeInscreverNaLiga() const {
        const Vertice& local = mapa_.vertice(jogador_.posicao());
        return local.descobrirTipoVertice() == "LIGA" && jogador_.quantidadeInsignias() >= 8 &&
               !prazoExpirou();
    }
};

#endif
