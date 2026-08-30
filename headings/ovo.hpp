#ifndef OVO_HPP
#define OVO_HPP

#include <stdexcept>
#include <utility>

#include "pokemon.hpp"

// A especie fica armazenada no ovo, mas so e revelada ao chocar.
class OvoPokemon {
private:
    EspeciePokemon especie_;
    int ataque_inicial_;
    int defesa_inicial_;
    int distancia_incubada_ = 0;
    bool chocado_ = false;

public:
    OvoPokemon(EspeciePokemon especie, int ataque_inicial, int defesa_inicial)
        : especie_(std::move(especie)),
          ataque_inicial_(ataque_inicial),
          defesa_inicial_(defesa_inicial) {
        especie_.validar();
        if (ataque_inicial_ <= 0 || defesa_inicial_ <= 0) {
            throw std::invalid_argument("Atributos iniciais do ovo invalidos");
        }
    }

    int distanciaRestante() const { return 100 - distancia_incubada_; }
    bool prontoParaChocar() const { return !chocado_ && distancia_incubada_ >= 100; }
    bool foiChocado() const { return chocado_; }

    void avancarDistancia(int distancia) {
        if (distancia < 0) throw std::invalid_argument("Distancia nao pode ser negativa");
        if (!chocado_) distancia_incubada_ += distancia;
    }

    Pokemon chocar() {
        if (!prontoParaChocar()) {
            throw std::logic_error("O ovo ainda nao completou a incubacao");
        }
        chocado_ = true;
        return Pokemon(especie_, ataque_inicial_, defesa_inicial_);
    }
};

#endif
