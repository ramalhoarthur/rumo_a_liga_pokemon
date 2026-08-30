#ifndef CONFIGURACAO_REGIAO_HPP
#define CONFIGURACAO_REGIAO_HPP

#include <array>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "grafo.hpp"
#include "pokemon.hpp"

struct ConfiguracaoRegiao {
    std::string caminho_mapa;
    long long prazo_inscricao = 0;
    int quantidade_selvagens = 0;
    int quantidade_treinadores = 0;
    int quantidade_ervas = 0;
    int quantidade_ovos = 0;
    std::vector<EspeciePokemon> especies;

    static ConfiguracaoRegiao ler(const std::string& caminho_arquivo) {
        std::ifstream arquivo(caminho_arquivo);
        if (!arquivo) {
            throw std::runtime_error("Nao foi possivel abrir o arquivo de cenario: " + caminho_arquivo);
        }

        ConfiguracaoRegiao configuracao;
        std::unordered_set<std::string> campos_lidos;
        std::string chave;
        while (arquivo >> chave) {
            if (chave == "MAPA") {
                if (!(arquivo >> std::quoted(configuracao.caminho_mapa))) {
                    throw std::runtime_error("Caminho do mapa invalido");
                }
                const std::filesystem::path mapa(configuracao.caminho_mapa);
                if (mapa.is_relative()) {
                    configuracao.caminho_mapa =
                        (std::filesystem::path(caminho_arquivo).parent_path() / mapa).lexically_normal().string();
                }
            } else if (chave == "PRAZO") {
                arquivo >> configuracao.prazo_inscricao;
            } else if (chave == "SELVAGENS") {
                arquivo >> configuracao.quantidade_selvagens;
            } else if (chave == "TREINADORES") {
                arquivo >> configuracao.quantidade_treinadores;
            } else if (chave == "ERVAS") {
                arquivo >> configuracao.quantidade_ervas;
            } else if (chave == "OVOS") {
                arquivo >> configuracao.quantidade_ovos;
            } else if (chave == "ESPECIES") {
                int quantidade_especies;
                if (!(arquivo >> quantidade_especies) || quantidade_especies <= 0) {
                    throw std::runtime_error("Quantidade de especies invalida");
                }
                for (int i = 0; i < quantidade_especies; ++i) {
                    int quantidade_formas;
                    if (!(arquivo >> quantidade_formas) || quantidade_formas < 1 || quantidade_formas > 3) {
                        throw std::runtime_error("Quantidade de formas invalida");
                    }
                    EspeciePokemon especie;
                    for (int j = 0; j < quantidade_formas; ++j) {
                        FormaPokemon forma;
                        int quantidade_tipos;
                        if (!(arquivo >> std::quoted(forma.nome) >> quantidade_tipos) ||
                            quantidade_tipos < 1) {
                            throw std::runtime_error("Forma de pokemon invalida");
                        }
                        for (int k = 0; k < quantidade_tipos; ++k) {
                            std::string tipo;
                            if (!(arquivo >> tipo)) throw std::runtime_error("Tipo de pokemon ausente");
                            forma.tipos.push_back(std::move(tipo));
                        }
                        especie.formas.push_back(std::move(forma));
                    }
                    especie.validar();
                    configuracao.especies.push_back(std::move(especie));
                }
            } else {
                throw std::runtime_error("Chave desconhecida no cenario: " + chave);
            }
            campos_lidos.insert(chave);
            if (!arquivo) throw std::runtime_error("Valor invalido para " + chave);
        }

        const std::array<std::string, 6> obrigatorios = {
            "MAPA", "PRAZO", "SELVAGENS", "TREINADORES", "ERVAS", "ESPECIES"};
        for (const std::string& campo : obrigatorios) {
            if (!campos_lidos.count(campo)) throw std::runtime_error("Campo obrigatorio ausente: " + campo);
        }
        if (configuracao.quantidade_selvagens < 0 || configuracao.quantidade_treinadores < 0 ||
            configuracao.quantidade_ervas < 0 || configuracao.quantidade_ovos < 0) {
            throw std::runtime_error("Quantidades do cenario nao podem ser negativas");
        }

        const Grafo mapa(configuracao.caminho_mapa);
        const long long soma = mapa.somaPesosArestas();
        if (configuracao.prazo_inscricao < 10 * soma || configuracao.prazo_inscricao > 15 * soma) {
            throw std::runtime_error("Prazo fora do intervalo de 10 a 15 vezes a soma dos pesos");
        }
        return configuracao;
    }
};

#endif
