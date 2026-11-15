#pragma once

struct EtatAction {
    bool courant = false;   
    bool change  = false;   
    bool actif   = false;   
};

class Action {
public:
    explicit Action(bool lie) : mLie(lie) {}

    void Synchroniser(bool touchePressee) {
        const bool nouveau = mLie ? touchePressee : false;
        mEtat.actif   = mLie;
        mEtat.change  = (nouveau != mEtat.courant);   
        mEtat.courant = nouveau;
    }

    const EtatAction& Etat() const { return mEtat; }

private:
    bool mLie;
    EtatAction mEtat;
};