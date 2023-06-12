# stm32LCRMeter
Crude test to see how viable it is to make LCR meter with 72mhz stm32. Sinewave is applied across device under test and LCR properties are determined by measuring voltage and current phase difference. Sinewave is created by dma->pwm stream so any arbitrary test signal can be created below few Mhz. Device can measure within 2% accuracy if range resistor is proper match with the dut, larger selection of range resistors are required for useful design.

Schematic
![schematic](https://github.com/aWanha/stm32LCRMeter/blob/main/schematic.JPG)

# Typical quality of test signal, 1Khz
![1kHzSinewave](https://github.com/aWanha/stm32LCRMeter/blob/main/output.png)
