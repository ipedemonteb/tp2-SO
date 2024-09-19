#ifndef NOTES_H
#define NOTES_H

typedef enum FIGURES
{
    BLANCA,
    NEGRA,
    CORCHEA,
    SEMICORCHEA,
    FUSA
} FIGURES;

void shellErrSound();
uint32_t figureInBpm(FIGURES fig);
void setBpm(uint32_t nBpm);
void endGameSound();
void eliminatorChoiceSound();

#endif
