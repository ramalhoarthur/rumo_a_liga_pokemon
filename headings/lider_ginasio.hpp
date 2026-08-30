#ifndef LIDER_GINASIO_HPP
#define LIDER_GINASIO_HPP

#include <stdexcept>
#include <string>
#include <utility>

#include "treinador.hpp"

class LiderGinasio {
private:
    Treinador treinador_;
    std::string insignia_;
    int ginasio_;
    bool itinerante_;
    int periodo_retorno_;
    int tempo_desde_retorno_ = 0;

public:
    LiderGinasio(Treinador treinador, std::string insignia, int ginasio, bool itinerante = false,
                 int periodo_retorno = 0)
        : treinador_(std::move(treinador)),
          insignia_(std::move(insignia)),
          ginasio_(ginasio),
          itinerante_(itinerante),
          periodo_retorno_(periodo_retorno) {
        if (insignia_.empty() || ginasio_ < 0 || (itinerante_ && periodo_retorno_ <= 0)) {
            throw std::invalid_argument("Dados do lider de ginasio invalidos");
        }
    }

    Treinador& treinador() { return treinador_; }
    const Treinador& treinador() const { return treinador_; }
    const std::string& insignia() const { return insignia_; }
    int ginasio() const { return ginasio_; }
    bool estaNoGinasio() const { return treinador_.posicao() == ginasio_; }

    // Lideres itinerantes podem se deslocar livremente, mas retornam periodicamente.
    void avancarTempo(const Grafo& mapa, int unidades) {
        if (unidades < 0) throw std::invalid_argument("Tempo nao pode ser negativo");
        treinador_.passarTempo(unidades);
        if (!itinerante_) return;

        tempo_desde_retorno_ += unidades;
        if (tempo_desde_retorno_ >= periodo_retorno_) {
            const Caminho retorno = mapa.caminhoMinimo(treinador_.posicao(), ginasio_);
            if (!retorno.existe()) {
                throw std::logic_error("O lider nao consegue retornar ao seu ginasio");
            }
            for (std::size_t i = 1; i < retorno.vertices.size(); ++i) {
                treinador_.moverParaVizinho(mapa, retorno.vertices[i]);
            }
            tempo_desde_retorno_ = 0;
        }
    }

    void moverParaVizinho(const Grafo& mapa, int destino) {
        if (!itinerante_) throw std::logic_error("Lider fixo nao pode se deslocar");
        treinador_.moverParaVizinho(mapa, destino);
    }
};

#endif
