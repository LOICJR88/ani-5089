#ifndef CHAINE_H
#define CHAINE_H

#include <cstdint>

class ChaineEchange {
public:
    static constexpr std::uint32_t MaxImages = 4u;

    bool Creer(std::uint32_t nombre) noexcept {
        if (nombre == 0u || nombre > MaxImages) {
            return false;
        }
        mNombre = nombre;
        mSuivante = 0u;
        mEmpruntee = false;
        for (std::uint32_t i = 0u; i < MaxImages; ++i) {
            mPoignees[i] = 0u;
        }
        return true;
    }

    bool Emprunter(std::uint32_t& index) noexcept {
        if (mNombre == 0u || mEmpruntee) {
            return false;
        }
        mEmpruntee = true;
        index = mSuivante;
        return true;
    }

    bool Rendre() noexcept {
        if (!mEmpruntee) {
            return false;
        }
        mEmpruntee = false;
        mSuivante = (mSuivante + 1u) % mNombre;
        return true;
    }

    std::uint32_t DernierRendu() const noexcept {
        if (mNombre == 0u) {
            return 0u;
        }
        return (mSuivante + mNombre - 1u) % mNombre;
    }

    bool Enregistrer(std::uint32_t index, std::uint64_t poignee) noexcept {
        if (index >= mNombre) {
            return false;
        }
        mPoignees[index] = poignee;
        return true;
    }

private:
    std::uint64_t mPoignees[MaxImages] = {0u, 0u, 0u, 0u};
    std::uint32_t mNombre = 0u;
    std::uint32_t mSuivante = 0u;
    bool mEmpruntee = false;
};

#endif