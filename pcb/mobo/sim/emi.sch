<Qucs Schematic 24.2.1>
<Properties>
  <View=-353,7,1647,888,1.46178,365,0>
  <Grid=10,10,1>
  <DataSet=emi.dat>
  <DataDisplay=emi.dpl>
  <OpenDisplay=0>
  <Script=emi.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <C C1 1 160 140 17 -26 0 1 "100 nF" 1 "" 0 "neutral" 0>
  <GND * 1 160 170 0 0 0 0>
  <L L1 1 250 80 -26 10 0 0 "4.7 uH" 1 "" 0>
  <R R1 1 330 80 -26 15 0 0 "105 mOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.AC AC1 1 90 300 0 42 0 0 "lin" 1 "1 Hz" 1 "10 MHz" 1 "10000" 1 "no" 0>
  <C C4 1 70 140 17 -26 0 1 "22 uF" 1 "" 0 "neutral" 0>
  <GND * 1 70 170 0 0 0 0>
  <NutmegEq NutmegEq1 1 110 470 -28 17 0 0 "ALL" 1 "gaindB=dB(vin/circuit)" 1>
  <GND * 1 430 170 0 0 0 0>
  <C C5 1 430 140 17 -26 0 1 "4.7 uF" 1 "" 0 "neutral" 0>
  <Vac V1 1 660 140 18 -26 0 1 "1 V" 1 "1 kHz" 0 "0" 0 "0" 0 "0" 0 "0" 0>
  <GND * 1 660 170 0 0 0 0>
  <C C6 1 530 140 17 -26 0 1 "100 nF" 1 "" 0 "neutral" 0>
  <GND * 1 530 170 0 0 0 0>
</Components>
<Wires>
  <160 80 160 110 "" 0 0 0 "">
  <160 80 220 80 "" 0 0 0 "">
  <280 80 300 80 "" 0 0 0 "">
  <70 80 160 80 "" 0 0 0 "">
  <70 80 70 110 "" 0 0 0 "">
  <360 80 430 80 "" 0 0 0 "">
  <430 80 430 110 "" 0 0 0 "">
  <660 80 660 110 "" 0 0 0 "">
  <430 80 530 80 "" 0 0 0 "">
  <530 80 660 80 "" 0 0 0 "">
  <530 80 530 110 "" 0 0 0 "">
  <160 80 160 80 "Vin" 190 50 0 "">
  <660 80 660 80 "circuit" 690 50 0 "">
</Wires>
<Diagrams>
  <Rect 270 522 392 232 3 #c0c0c0 1 10 1 0 1 0 1 0 1 0 1 -1 1 1 315 0 225 1 1 0 "" "" "">
	<"ngspice/ac.gaindb" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
