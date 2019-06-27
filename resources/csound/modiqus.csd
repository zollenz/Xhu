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

/*********************/
/* Globals           */
/*********************/

giSine          ftgen 1, 0, 4096, 10, 1

gipitchlow      = 100.0
gipitchhigh     = 1000.0
gipitchrange    = gipitchhigh - gipitchlow

instr 1

Sinstance sprintf "%f", p1
Spitchchn   strcat Sinstance, ".pitch"

puts    Spitchchn, 1

kpitch  chnget  Spitchchn

printf  "kpitch: %f", 1, kpitch

kpitch  = gipitchlow + kpitch * gipitchrange
apitch  interp  kpitch

asound oscili 0.5, apitch, 1
outs asound,asound
endin

</CsInstruments>
<CsScore>
</CsScore>
</CsoundSynthesizer>
