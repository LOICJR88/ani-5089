#pragma once

enum class Espace { View, Local, Stage };

struct Vue {
    int image = -1;        
    float lacetRendu = 0;  
};

struct CoucheProjection {
    Espace espace = Espace::Stage;
    Vue vues[2];          
};