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

gipitchlow      =           100.0
gipitchhigh     =           1000.0
gipitchrange    =           gipitchhigh - gipitchlow

giSine          ftgen       1, 0, 4096, 10, 1

/*********************/
/* inst_end          */
/*********************/

instr 2, inst_end

Sname  sprintf "callback-%d", p4
Svalue      =        "sad"
outvalue Sname, Svalue

endin

/*********************/
/* sine_oscil        */
/*********************/

instr 1, sine_oscil

if p3 > - 1 then
event_i "i", 2, p3, 0, 8
endif


Sinstance   sprintf "%f", p1
Spitchin    sprintf "i.%s.pitch", Sinstance
Spitchout   sprintf "o.%s.pitch", Sinstance

            puts    Spitchin, 1
            puts    Spitchout, 1

kpitch      chnget  Spitchin
            //chnset  kpitch, Spitchout

            printf  "kpitch: %f", 1, kpitch

kpitch      =       gipitchlow + kpitch * gipitchrange
apitch      interp  kpitch

asound      oscili  0.5, apitch, 1
            outs    asound, asound

endin

</CsInstruments>
<CsScore>
</CsScore>
</CsoundSynthesizer>
