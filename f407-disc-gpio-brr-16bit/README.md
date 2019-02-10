# Test for non-persistent bits

Bug [#62](https://github.com/gnu-mcu-eclipse/qemu/issues/62)

The problem was caused by an erroneous persistence of the BRR/BSRR 
registers. Setting bits in these registers should have only side 
effects (changing bits in ODR or triggering interrupts) and **do not** 
leave bits set, since they'll interfere with subsequent writes, as 
reported.

The solution was to add `persistent-bits` to the peripheral register 
definition, and do not save non-persistent bits.

The test should issue separate accesses via upper/lower half-words.
