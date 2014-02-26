<CsoundSynthesizer>

/***********/
/* Options */
/***********/

<CsOptions>
-o dac -+rtaudio=auhal -b 512 -B 1024
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

instr 1
	asound oscili 15000, 440, 1
	out asound
endin

</CsInstruments>
<CsScore>
</CsScore>
</CsoundSynthesizer>