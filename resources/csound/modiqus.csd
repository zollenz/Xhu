<CsoundSynthesizer>

/***********/
/* Options */
/***********/

<CsOptions>
-o dac -+rtaudio=auhal
</CsOptions>
<CsInstruments>

/*********************/
/* Header statements */
/*********************/

sr = 44100
kr = 4410    
ksmps = 10
nchnls = 2
0dbfs = 1

giSine          ftgen 1, 0, 4096, 10, 1

instr 1
asound oscili 0.5, 440, 1
outs asound,asound
endin

</CsInstruments>
<CsScore>
</CsScore>
</CsoundSynthesizer>