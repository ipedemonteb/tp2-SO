#include <syscaller.h>
#include <sounds.h>

uint32_t bpm = 90;

uint32_t figureInBpm(FIGURES fig)
{
    switch (fig)
    {
    case BLANCA:
        return 1000 * (2 / (bpm / 60));
        break;
    case NEGRA:
        return 1000 * (1 / (bpm / 60));
        break;
    case CORCHEA:
        return 1000 * (0.5 / (bpm / 60));
        break;
    case SEMICORCHEA:
        return 1000 * (0.25 / (bpm / 60));
        break;
    case FUSA:
        return 1000 * (0.125 / (bpm / 60));
        break;
    default:
        return 0;
        break;
    }
}

void setBpm(uint32_t nBpm)
{
    bpm = nBpm;
}

void endGameSound()
{
    setBpm(120);

    beepSysCaller(UNUSED, 450, figureInBpm(SEMICORCHEA));
    beepSysCaller(UNUSED, 435, figureInBpm(SEMICORCHEA));
    beepSysCaller(UNUSED, 400, figureInBpm(CORCHEA));
}

void shellErrSound()
{
    setBpm(200);
    beepSysCaller(UNUSED, 70, figureInBpm(FUSA));
    beepSysCaller(UNUSED, 70, figureInBpm(FUSA));
}

void eliminatorChoiceSound()
{
    setBpm(190);

    beepSysCaller(UNUSED, 550, figureInBpm(FUSA));
    beepSysCaller(UNUSED, 450, figureInBpm(FUSA));
}