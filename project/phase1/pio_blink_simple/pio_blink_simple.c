#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "blink.pio.h"

// set max clock in platform_defs.h


void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq) {
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    // PIO counter program takes 3 more cycles in total than we pass as
    // input (wait for n + 1; mov; jmp)
    /* Direct write access to the TX FIFO for this state machine
    so load the value on the right into the TX FIFO 0, in this case
     */
    pio->txf[sm] = (SYS_CLK_HZ / (2 * freq)) - 3;
}



int main()
{
    // PIO Blinking example
    /* Explicitly grab pio0, since there is only one thing happening in this code,
    everything is available. If more activity was happening, we would want to leverage
    pio_claim_free_sm_and_add_program to get the available resource
    */
    PIO pio = pio0;
    // arbitrarily choose state machine 0 of pio0
    uint sm = 0;
    /* &blink_program is the name of the struct in the compiled *.pio.h file that
    points to the instructions, instruction length, origin, and pio version. pioasm will
    autogenerate a file from the pio file. just know the NAME_program is the NAME of 
    the pio file on the "".program blink"

    offset refers to the memory address where a PIO program is loaded into the PIO's 
    instruction memory*/
    uint offset = pio_add_program(pio, &blink_program);
    
    uint output_pin = 2;
    uint frequency = 1000000;  // this is the value used and manipulated to be loaded int the tx fifo

    blink_pin_forever(pio, sm, offset, output_pin, frequency);

    while (true) {}
}
