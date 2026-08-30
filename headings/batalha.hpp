#ifndef BATALHA_HPP
#define BATALHA_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <random>
#include <stdexcept>
#include <vector>

#include "pokemon.hpp"
#include "treinador.hpp"
#include "vantagens_tipo.hpp"

enum class VencedorDuelo { Desafiado, Desafiante };

struct ResultadoDuelo {
    VencedorDuelo vencedor;
    int turnos;
    int dano_total;
};

struct ResultadoBatalhaTreinadores {
    VencedorDuelo vencedor;
    int duelos_vencidos_desafiado;
    int duelos_vencidos_desafiante;
    int turnos;
};

// Combates usam AP/DP efetivos (pokemon + XP do treinador). As probabilidades
// sao |XP1 - XP2| / 1000, limitadas a 90% para garantir que a luta progrida.
class Batalha {
private:
    std::mt19937& gerador_;

    bool ocorre(double probabilidade) {
        return std::bernoulli_distribution(probabilidade)(gerador_);
    }

    static double probabilidadePorDiferencaXp(const Pokemon& atacante,
                                              const Pokemon& defensor) {
        const int diferenca = std::abs(atacante.xp() - defensor.xp());
        return std::min(0.90, diferenca / 1000.0);
    }

    int atacar(Pokemon& atacante, int xp_treinador_atacante, Pokemon& defensor,
               int xp_treinador_defensor) {
        const double probabilidade = probabilidadePorDiferencaXp(atacante, defensor);
        if (ocorre(probabilidade)) return 0; // Esquiva do defensor.

        const int dano_base = std::max(0, atacante.ataque() + xp_treinador_atacante -
                                           (defensor.defesa() + xp_treinador_defensor));
        int dano = static_cast<int>(std::floor(
            dano_base * TabelaVantagensTipo::multiplicador(atacante.tipos(), defensor.tipos())));
        if (dano > 0 && ocorre(probabilidade)) dano *= 2;
        if (dano > 0) {
            const int indisponibilidade = std::uniform_int_distribution<int>(10, 50)(gerador_);
            defensor.sofrerDano(dano, indisponibilidade);
        }
        return dano;
    }

public:
    explicit Batalha(std::mt19937& gerador) : gerador_(gerador) {}

    // O desafiado sempre ataca primeiro, conforme o enunciado.
    ResultadoDuelo duelo(Pokemon& desafiado, int xp_desafiado, Pokemon& desafiante,
                         int xp_desafiante) {
        if (!desafiado.podeBatalhar() || !desafiante.podeBatalhar()) {
            throw std::logic_error("Somente pokemons conscientes podem iniciar um duelo");
        }

        bool vez_do_desafiado = true;
        int turnos = 0;
        int dano_total = 0;
        constexpr int LIMITE_TURNOS = 10000;
        while (desafiado.podeBatalhar() && desafiante.podeBatalhar() && turnos < LIMITE_TURNOS) {
            dano_total += vez_do_desafiado
                               ? atacar(desafiado, xp_desafiado, desafiante, xp_desafiante)
                               : atacar(desafiante, xp_desafiante, desafiado, xp_desafiado);
            vez_do_desafiado = !vez_do_desafiado;
            ++turnos;
        }

        // AP <= DP dos dois lados pode impedir qualquer dano. Como nao ha empates,
        // o desempate usa HP e, persistindo a igualdade, sorteio.
        VencedorDuelo vencedor;
        if (!desafiante.podeBatalhar()) {
            vencedor = VencedorDuelo::Desafiado;
        } else if (!desafiado.podeBatalhar()) {
            vencedor = VencedorDuelo::Desafiante;
        } else if (desafiado.hp() != desafiante.hp()) {
            vencedor = desafiado.hp() > desafiante.hp() ? VencedorDuelo::Desafiado
                                                        : VencedorDuelo::Desafiante;
        } else {
            vencedor = ocorre(0.5) ? VencedorDuelo::Desafiado : VencedorDuelo::Desafiante;
        }

        if (vencedor == VencedorDuelo::Desafiado && desafiante.podeBatalhar()) {
            desafiante.tornarInconsciente(std::uniform_int_distribution<int>(10, 50)(gerador_));
        }
        if (vencedor == VencedorDuelo::Desafiante && desafiado.podeBatalhar()) {
            desafiado.tornarInconsciente(std::uniform_int_distribution<int>(10, 50)(gerador_));
        }

        if (vencedor == VencedorDuelo::Desafiado) {
            desafiado.registrarVitoriaContra(desafiante.xp());
            desafiante.registrarDerrota();
        } else {
            desafiante.registrarVitoriaContra(desafiado.xp());
            desafiado.registrarDerrota();
        }
        return {vencedor, turnos, dano_total};
    }

    ResultadoBatalhaTreinadores treinadores(Treinador& desafiado, Treinador& desafiante,
                                             bool desafiado_desiste = false) {
        if (!desafiado.podeBatalharContraTreinador() ||
            !desafiante.podeBatalharContraTreinador()) {
            throw std::logic_error("Ambos os treinadores precisam de tres pokemons conscientes");
        }
        if (desafiado_desiste) {
            desafiante.ganharXpPorVitoria(desafiado.xp());
            return {VencedorDuelo::Desafiante, 0, 3, 0};
        }

        int vitorias_desafiado = 0;
        int vitorias_desafiante = 0;
        int turnos = 0;
        auto& time_desafiado = desafiado.pokemonAtivos();
        auto& time_desafiante = desafiante.pokemonAtivos();
        std::vector<std::size_t> escolhidos_desafiado;
        std::vector<std::size_t> escolhidos_desafiante;
        for (std::size_t i = 0; i < time_desafiado.size() && escolhidos_desafiado.size() < 3; ++i) {
            if (time_desafiado[i].podeBatalhar()) escolhidos_desafiado.push_back(i);
        }
        for (std::size_t i = 0; i < time_desafiante.size() && escolhidos_desafiante.size() < 3; ++i) {
            if (time_desafiante[i].podeBatalhar()) escolhidos_desafiante.push_back(i);
        }
        for (std::size_t i = 0; i < 3; ++i) {
            ResultadoDuelo resultado = duelo(time_desafiado[escolhidos_desafiado[i]], desafiado.xp(),
                                              time_desafiante[escolhidos_desafiante[i]], desafiante.xp());
            turnos += resultado.turnos;
            if (resultado.vencedor == VencedorDuelo::Desafiado) ++vitorias_desafiado;
            else ++vitorias_desafiante;
        }

        const VencedorDuelo vencedor = vitorias_desafiado > vitorias_desafiante
                                           ? VencedorDuelo::Desafiado
                                           : VencedorDuelo::Desafiante;
        if (vencedor == VencedorDuelo::Desafiado) {
            desafiado.ganharXpPorVitoria(desafiante.xp());
        } else {
            desafiante.ganharXpPorVitoria(desafiado.xp());
        }
        return {vencedor, vitorias_desafiado, vitorias_desafiante, turnos};
    }

    // Retorna verdadeiro se o selvagem ficou inconsciente e foi capturado.
    bool capturar(Treinador& treinador, Pokemon& escolhido, Pokemon& selvagem) {
        if (!escolhido.podeBatalhar()) {
            throw std::logic_error("O pokemon escolhido precisa estar consciente");
        }
        if (!treinador.podeCapturar()) {
            throw std::logic_error("O treinador nao possui pokebola de captura disponivel");
        }
        const ResultadoDuelo resultado = duelo(selvagem, 0, escolhido, treinador.xp());
        if (resultado.vencedor != VencedorDuelo::Desafiante) return false;

        treinador.ganharXpPorVitoria(0);
        escolhido.ganharXp(3);
        treinador.receberPokemon(std::move(selvagem));
        return true;
    }
};

#endif
