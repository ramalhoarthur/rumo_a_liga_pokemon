#ifndef VERTICE_HPP
#define VERTICE_HPP

#include <string>
#include <utility>

class Vertice {
private:
    int id_; // De 0 a N-1
    std::string nome_;
    std::string tipo_; // CIDADE, ROTA ou LIGA
    bool possui_cmp_;
    bool possui_ginasio_;
    bool possui_laboratorio_;

public:
    Vertice(int id, std::string nome, std::string tipo, bool possui_cmp,
            bool possui_ginasio, bool possui_laboratorio)
        : id_(id),
          nome_(std::move(nome)),
          tipo_(std::move(tipo)),
          possui_cmp_(possui_cmp),
          possui_ginasio_(possui_ginasio),
          possui_laboratorio_(possui_laboratorio) {}

    int descobrirIdVertice() const { return id_; }
    const std::string& descobrirNomeVertice() const { return nome_; }
    const std::string& descobrirTipoVertice() const { return tipo_; }
    bool temCmp() const { return possui_cmp_; }
    bool temGinasio() const { return possui_ginasio_; }
    bool temLaboratorio() const { return possui_laboratorio_; }
};

#endif
