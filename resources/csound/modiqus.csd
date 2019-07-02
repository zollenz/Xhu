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

instr 1

Sinstance       sprintf     "%f", p1
S_pitch_channel_in    sprintf     "i.%s.pitch", Sinstance
S_pitch_channel_out   sprintf     "o.%s.pitch", Sinstance

puts            S_pitch_channel_in, 1
puts            S_pitch_channel_out, 1

kpitch          chnget      S_pitch_channel_in
                chnset      kpitch, S_pitch_channel_out

                printf      "kpitch: %f", 1, kpitch

kpitch          =           gipitchlow + kpitch * gipitchrange
apitch          interp      kpitch

asound          oscili      0.5, apitch, 1
                outs        asound, asound
endin

</CsInstruments>
<CsScore>
</CsScore>
</CsoundSynthesizer>
