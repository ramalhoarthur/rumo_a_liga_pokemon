#ifndef VANTAGENS_TIPO_HPP
#define VANTAGENS_TIPO_HPP

#include <array>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

// A ordem deve permanecer alfabetica e coincide com as linhas/colunas da matriz.
enum class TipoPokemon : std::size_t {
    ACO,
    AGUA,
    DRAGAO,
    ELETRICO,
    FADA,
    FANTASMA,
    FOGO,
    GELO,
    INSETO,
    LUTADOR,
    NORMAL,
    PEDRA,
    PLANTA,
    PSIQUICO,
    SOMBRIO,
    TERRA,
    VENENOSO,
    VOADOR,
    QUANTIDADE
};

class TabelaVantagensTipo {
private:
    static constexpr std::size_t N = static_cast<std::size_t>(TipoPokemon::QUANTIDADE);
    using Matriz = std::array<std::array<float, N>, N>;

    static Matriz criarMatriz() {
        Matriz matriz{};
        for (auto& linha : matriz) linha.fill(1.0F);

        const auto definir = [&matriz](TipoPokemon atacante, std::initializer_list<TipoPokemon> defensores,
                                       float multiplicador) {
            for (const TipoPokemon defensor : defensores) {
                matriz[static_cast<std::size_t>(atacante)][static_cast<std::size_t>(defensor)] = multiplicador;
            }
        };

        // Vantagens e desvantagens transcritas da tabela fornecida.
        definir(TipoPokemon::ACO, {TipoPokemon::FADA, TipoPokemon::GELO, TipoPokemon::PEDRA}, 1.5F);
        definir(TipoPokemon::ACO, {TipoPokemon::FOGO, TipoPokemon::TERRA, TipoPokemon::LUTADOR}, 0.5F);
        definir(TipoPokemon::AGUA, {TipoPokemon::FOGO, TipoPokemon::PEDRA, TipoPokemon::TERRA}, 1.5F);
        definir(TipoPokemon::AGUA, {TipoPokemon::ELETRICO, TipoPokemon::PLANTA}, 0.5F);
        definir(TipoPokemon::DRAGAO, {TipoPokemon::DRAGAO}, 1.5F);
        // Dragao e Fantasma aparecem nas duas colunas da imagem contra o proprio tipo.
        // Para o dano, a vantagem declarada prevalece nesses dois confrontos.
        definir(TipoPokemon::DRAGAO, {TipoPokemon::FADA, TipoPokemon::GELO}, 0.5F);
        definir(TipoPokemon::ELETRICO, {TipoPokemon::AGUA, TipoPokemon::VOADOR}, 1.5F);
        definir(TipoPokemon::ELETRICO, {TipoPokemon::TERRA}, 0.5F);
        definir(TipoPokemon::FADA, {TipoPokemon::DRAGAO, TipoPokemon::LUTADOR, TipoPokemon::SOMBRIO}, 1.5F);
        definir(TipoPokemon::FADA, {TipoPokemon::ACO, TipoPokemon::VENENOSO}, 0.5F);
        definir(TipoPokemon::FANTASMA, {TipoPokemon::FANTASMA, TipoPokemon::PSIQUICO}, 1.5F);
        definir(TipoPokemon::FANTASMA, {TipoPokemon::SOMBRIO}, 0.5F);
        definir(TipoPokemon::FOGO, {TipoPokemon::ACO, TipoPokemon::GELO, TipoPokemon::INSETO, TipoPokemon::PLANTA}, 1.5F);
        definir(TipoPokemon::FOGO, {TipoPokemon::AGUA, TipoPokemon::PEDRA, TipoPokemon::TERRA}, 0.5F);
        definir(TipoPokemon::GELO, {TipoPokemon::DRAGAO, TipoPokemon::PLANTA, TipoPokemon::TERRA, TipoPokemon::VOADOR}, 1.5F);
        definir(TipoPokemon::GELO, {TipoPokemon::ACO, TipoPokemon::FOGO, TipoPokemon::LUTADOR, TipoPokemon::PEDRA}, 0.5F);
        definir(TipoPokemon::INSETO, {TipoPokemon::PLANTA, TipoPokemon::PSIQUICO, TipoPokemon::SOMBRIO}, 1.5F);
        definir(TipoPokemon::INSETO, {TipoPokemon::FOGO, TipoPokemon::PEDRA, TipoPokemon::VOADOR}, 0.5F);
        definir(TipoPokemon::LUTADOR, {TipoPokemon::ACO, TipoPokemon::GELO, TipoPokemon::NORMAL, TipoPokemon::PEDRA, TipoPokemon::SOMBRIO}, 1.5F);
        definir(TipoPokemon::LUTADOR, {TipoPokemon::FADA, TipoPokemon::PSIQUICO, TipoPokemon::VOADOR}, 0.5F);
        definir(TipoPokemon::NORMAL, {TipoPokemon::LUTADOR}, 0.5F);
        definir(TipoPokemon::PEDRA, {TipoPokemon::FOGO, TipoPokemon::GELO, TipoPokemon::INSETO, TipoPokemon::VOADOR}, 1.5F);
        definir(TipoPokemon::PEDRA, {TipoPokemon::ACO, TipoPokemon::AGUA, TipoPokemon::LUTADOR, TipoPokemon::PLANTA, TipoPokemon::TERRA}, 0.5F);
        definir(TipoPokemon::PLANTA, {TipoPokemon::AGUA, TipoPokemon::PEDRA, TipoPokemon::TERRA}, 1.5F);
        definir(TipoPokemon::PLANTA, {TipoPokemon::GELO, TipoPokemon::FOGO, TipoPokemon::INSETO, TipoPokemon::VENENOSO, TipoPokemon::VOADOR}, 0.5F);
        definir(TipoPokemon::PSIQUICO, {TipoPokemon::LUTADOR, TipoPokemon::VENENOSO}, 1.5F);
        definir(TipoPokemon::PSIQUICO, {TipoPokemon::INSETO, TipoPokemon::FANTASMA, TipoPokemon::SOMBRIO}, 0.5F);
        definir(TipoPokemon::SOMBRIO, {TipoPokemon::FANTASMA, TipoPokemon::PSIQUICO}, 1.5F);
        definir(TipoPokemon::SOMBRIO, {TipoPokemon::FADA, TipoPokemon::INSETO, TipoPokemon::LUTADOR}, 0.5F);
        definir(TipoPokemon::TERRA, {TipoPokemon::ACO, TipoPokemon::ELETRICO, TipoPokemon::FOGO, TipoPokemon::PEDRA, TipoPokemon::VENENOSO}, 1.5F);
        definir(TipoPokemon::TERRA, {TipoPokemon::AGUA, TipoPokemon::GELO, TipoPokemon::PLANTA}, 0.5F);
        definir(TipoPokemon::VENENOSO, {TipoPokemon::FADA, TipoPokemon::PLANTA}, 1.5F);
        definir(TipoPokemon::VENENOSO, {TipoPokemon::PSIQUICO, TipoPokemon::TERRA}, 0.5F);
        definir(TipoPokemon::VOADOR, {TipoPokemon::INSETO, TipoPokemon::LUTADOR, TipoPokemon::PLANTA}, 1.5F);
        definir(TipoPokemon::VOADOR, {TipoPokemon::ELETRICO, TipoPokemon::GELO, TipoPokemon::PEDRA}, 0.5F);
        return matriz;
    }

    static const Matriz& matriz() {
        static const Matriz tabela = criarMatriz();
        return tabela;
    }

public:
    static TipoPokemon converter(const std::string& tipo) {
        static const std::array<std::string, N> nomes = {
            "ACO", "AGUA", "DRAGAO", "ELETRICO", "FADA", "FANTASMA", "FOGO", "GELO", "INSETO",
            "LUTADOR", "NORMAL", "PEDRA", "PLANTA", "PSIQUICO", "SOMBRIO", "TERRA", "VENENOSO", "VOADOR"};
        const auto encontrado = std::find(nomes.begin(), nomes.end(), tipo);
        if (encontrado == nomes.end()) throw std::invalid_argument("Tipo pokemon desconhecido: " + tipo);
        return static_cast<TipoPokemon>(std::distance(nomes.begin(), encontrado));
    }

    static float multiplicador(TipoPokemon atacante, TipoPokemon defensor) {
        return matriz()[static_cast<std::size_t>(atacante)][static_cast<std::size_t>(defensor)];
    }

    static float multiplicador(const std::vector<std::string>& tipos_atacante,
                               const std::vector<std::string>& tipos_defensor) {
        if (tipos_atacante.empty() || tipos_defensor.empty()) {
            throw std::invalid_argument("Pokemons precisam possuir ao menos um tipo");
        }
        float melhor = 0.0F;
        for (const std::string& tipo_atacante : tipos_atacante) {
            float efeito = 1.0F;
            const TipoPokemon atacante = converter(tipo_atacante);
            for (const std::string& tipo_defensor : tipos_defensor) {
                efeito *= multiplicador(atacante, converter(tipo_defensor));
            }
            melhor = std::max(melhor, efeito);
        }
        return melhor;
    }
};

#endif
