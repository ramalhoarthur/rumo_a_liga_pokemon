#ifndef POKEMON_HPP
#define POKEMON_HPP

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

enum class EstadoSaude { Consciente, Inconsciente, GravementeFerido };

struct FormaPokemon {
    std::string nome;
    std::vector<std::string> tipos;
};

// Cada especie tem entre uma e tres formas, na ordem das evolucoes.
struct EspeciePokemon {
    std::vector<FormaPokemon> formas;

    void validar() const {
        if (formas.empty() || formas.size() > 3) {
            throw std::invalid_argument("Uma especie deve possuir entre uma e tres formas");
        }
        for (const FormaPokemon& forma : formas) {
            if (forma.nome.empty() || forma.tipos.empty()) {
                throw std::invalid_argument("Toda forma deve possuir nome e ao menos um tipo");
            }
        }
    }
};

class Pokemon {
private:
    EspeciePokemon especie_;
    std::size_t fase_ = 0;
    int xp_ = 0;
    int ataque_base_;
    int defesa_base_;
    int bonus_ataque_batalha_ = 0;
    int bonus_defesa_batalha_ = 0;
    int hp_ = 100;
    int distancia_xp_ = 0;
    int distancia_recuperacao_ = 0;
    int distancia_inconsciente_ = 0;

    void evoluirSeNecessario() {
        // Cada 1000 XP permite uma nova forma, respeitando o limite da especie.
        while (fase_ + 1 < especie_.formas.size() && xp_ >= static_cast<int>((fase_ + 1) * 1000)) {
            const int xp_atual = xp_ / 10;
            ataque_base_ = std::max(1, static_cast<int>(std::ceil(ataque() * 1.30)) -
                                      xp_atual - bonus_ataque_batalha_);
            defesa_base_ = std::max(1, static_cast<int>(std::ceil(defesa() * 1.30)) -
                                      xp_atual - bonus_defesa_batalha_);
            ++fase_;
        }
    }

public:
    Pokemon(EspeciePokemon especie, int ataque_inicial, int defesa_inicial, int hp_inicial = 100,
            int xp_inicial = 0)
        : especie_(std::move(especie)),
          xp_(xp_inicial),
          ataque_base_(ataque_inicial),
          defesa_base_(defesa_inicial),
          hp_(hp_inicial) {
        especie_.validar();
        if (ataque_base_ <= 0 || defesa_base_ <= 0 || xp_ < 0 || hp_ < 1 || hp_ > 100) {
            throw std::invalid_argument("Atributos iniciais do pokemon invalidos");
        }
        evoluirSeNecessario();
    }

    const std::string& nome() const { return especie_.formas[fase_].nome; }
    const std::vector<std::string>& tipos() const { return especie_.formas[fase_].tipos; }
    std::size_t fase() const { return fase_ + 1; }
    int xp() const { return xp_; }
    int hp() const { return hp_; }
    int ataque() const { return ataque_base_ + xp_ / 10 + bonus_ataque_batalha_; }
    int defesa() const { return defesa_base_ + xp_ / 10 + bonus_defesa_batalha_; }
    EstadoSaude estadoSaude() const {
        if (hp_ < 5) return EstadoSaude::GravementeFerido;
        if (hp_ < 20 || distancia_inconsciente_ > 0) return EstadoSaude::Inconsciente;
        return EstadoSaude::Consciente;
    }
    bool podeBatalhar() const { return estadoSaude() == EstadoSaude::Consciente; }
    bool precisaPMC() const { return hp_ < 5; }
    int distanciaInconscienteRestante() const { return distancia_inconsciente_; }

    // Define os atributos de um encontro selvagem, preservando a especie.
    void definirAtributosDeEncontro(int ataque, int defesa, int hp, int xp) {
        if (ataque <= 0 || defesa <= 0 || hp < 1 || hp > 100 || xp < 0) {
            throw std::invalid_argument("Atributos de encontro invalidos");
        }
        xp_ = xp;
        fase_ = 0;
        bonus_ataque_batalha_ = 0;
        bonus_defesa_batalha_ = 0;
        ataque_base_ = 1;
        defesa_base_ = 1;
        evoluirSeNecessario();
        ataque_base_ = std::max(1, ataque - xp_ / 10);
        defesa_base_ = std::max(1, defesa - xp_ / 10);
        hp_ = hp;
        distancia_xp_ = 0;
        distancia_recuperacao_ = 0;
        distancia_inconsciente_ = 0;
    }

    void ganharXp(int quantidade) {
        if (quantidade < 0) throw std::invalid_argument("XP nao pode ser negativo");
        xp_ += quantidade;
        evoluirSeNecessario();
    }

    void registrarVitoriaContra(int xp_oponente) {
        if (xp_oponente >= xp_) {
            ++bonus_ataque_batalha_;
            ++bonus_defesa_batalha_;
        }
        ganharXp(10);
    }

    void registrarDerrota() { ganharXp(3); }

    void sofrerDano(int dano, int indisponibilidade) {
        if (dano < 0 || indisponibilidade < 0) {
            throw std::invalid_argument("Dano e indisponibilidade devem ser nao negativos");
        }
        hp_ = std::max(1, hp_ - dano);
        if (hp_ < 20) {
            distancia_inconsciente_ = std::max(distancia_inconsciente_, indisponibilidade);
        }
    }

    void tornarInconsciente(int indisponibilidade) {
        if (indisponibilidade < 0) {
            throw std::invalid_argument("Indisponibilidade deve ser nao negativa");
        }
        hp_ = std::min(hp_, 19);
        distancia_inconsciente_ = std::max(distancia_inconsciente_, indisponibilidade);
    }

    bool recuperarComErva() {
        if (estadoSaude() != EstadoSaude::Consciente) return false;
        const int hp_antes = hp_;
        hp_ = std::min(100, hp_ + 10);
        return hp_ > hp_antes;
    }

    void tratarNoPMC() {
        hp_ = 100;
        distancia_inconsciente_ = 0;
        distancia_recuperacao_ = 0;
    }

    // Atualiza XP, recuperacao natural e indisponibilidade apos percorrer uma distancia.
    void avancarDistancia(int distancia) {
        if (distancia < 0) throw std::invalid_argument("Distancia nao pode ser negativa");
        distancia_xp_ += distancia;
        while (distancia_xp_ >= 100) {
            distancia_xp_ -= 100;
            ganharXp(1);
        }

        if (!precisaPMC()) {
            distancia_recuperacao_ += distancia;
            while (distancia_recuperacao_ >= 10) {
                distancia_recuperacao_ -= 10;
                hp_ = std::min(100, hp_ + 1);
            }
        }
        distancia_inconsciente_ = std::max(0, distancia_inconsciente_ - distancia);
    }
};

#endif
