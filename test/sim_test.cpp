/*
 * Copyright (c) GrissinoPublishing 2024
 *
 *  Licenced under MIT Open Source Licence
 *
 */

/*


    _______ Q     __________
    | MAR | ----- | EEPROM |- - - - ME, WE, LM
    -------   ^   ----------
     D |    marBus    |
       |______________|
              |
              |      D ______
              |________| PC |- - - CP, LP
        8     |        ------
              |          || pcBus
        b     |        _______
        i     |--------| PCB |- - - PE
        t     |      Q -------
              |
        b     |
        u     |      D ______
        s     |--------| IR |- - - LI
              |        ------
              |          ||  irBus
              |        _______
              |-=-=-=-=| IRB |- - - IE (4bit)
              |      Q -------
              |
              
              
              
              
              
              
*/    


#define DEBUG_PRINT

#include <cstdio>
#include <sim.hpp>
#include <Bus.hpp>
#include <AT28C64.hpp>
#include <app.hpp>
#include <Latch.hpp>
#include <Buffer.hpp>
#include <Counter.hpp>
#include <Clock.hpp>

// Pin State type PinState_E_t
PinState_E_t test_pinState {kMaxState};

// Pin Value type PinValue_E_t
PinValue_E_t test_pinValue {kPinValMax};

// Byte type byte_t
byte_t test_dataByte {0};

// Electrical Pin type Pin
Pin test_pin;

// pin_group_s pinGroup_t test
#define TEST_ARR_SIZE 10
Pin test_pinArr[TEST_ARR_SIZE];
pinGroup_t test_pinGroup {TEST_ARR_SIZE, test_pinArr};

// Electrical Input Pin type InputPin
InputPin    test_inputpin;

// Electrical Output Pin type OutputPin
OutputPin   test_outputpin;

// Electrical Ground Pin type GroundPin
GroundPin   test_groundpin;

// Electrical SourcePin type SourcePin
SourcePin   test_sourcepin;

// Electrical High Impeadence Pin type HighZPin
HighZPin    test_highZpin;

// Electrical Input and Output Pin type IOPin
IOPin test_iopin;

// Electrical Tri-State Pin type TriStatePin
TriStatePin test_tristatepin;

// Electrical Component type Component
Component test_component;

// Electrical Node type ElectricalNode
ElectricalNode test_electricalnode;

// Electrical 8-bit Bus type Bus
Bus test_bus;

// EEPROM/SRAM Component
AT28C64 test_eeprom;

// Latch Component, BusComponent
Latch test_latch;

// Buffer Component, BusComponent
Buffer test_buffer;

// Binary Counter, BusComponent
Counter test_counter;

// Ring Counter
RingCounter test_ringcounter(5);

// Clock
Clock test_clk;

class TestComp : public Component
{

      IOPin data_io;

      InputPin OutputEnable;

      pinGroup_t data_io_pins {N_BUS_BITS, (Pin*)data_io};

      TestComp() {
            for (Pin* pin : data_io)
            {
                  add_pin(pin);
            }
            add_pin(OutputEnable);
            
      }
}

int sim_test(int argc, char** argv) 
{
    
//     test_component.add_pin(&test_inputpin);
//     test_bus.attach(test_pinGroup);
    
//    test_inputpin.set_state(kInput);
//    test_outputpin.set_state(kOutput);
//    test_groundpin.set_state(kGround);
//    test_sourcepin.set_state(kSource);
//    test_highZpin.set_state(kHighZ);
//    test_iopin.set_state(kInput);
//    test_tristatepin.set_state(kHighZ); 
      // test_outputpin.set_value(kLogicHigh);

      // test_electricalnode.connect(&test_inputpin);
      // test_electricalnode.connect(&test_outputpin);
      // test_electricalnode.connect(&test_tristatepin);
      // test_electricalnode.connect(&test_iopin);
      // test_electricalnode.evaluate();


      ElectricalNode groundnode;
      groundnode.connect(&test_groundpin);
      groundnode.connect(&test_ringcounter.Clear);
      groundnode.connect(&test_buffer.OutputEnable);

      ElectricalNode rc_sernode;
      rc_sernode.connect(&test_ringcounter.Ser);
      rc_sernode.connect(&test_ringcounter.Q_pins.pins[4]);

      
      test_bus.attach(TestComp.io_pins);

      test_bus.attach(test_buffer.Q_pins);
      test_bus.attach(test_latch.D_pins);

      test_electricalnode.connect(&test_latch.LatchEnable);
      
      test_electricalnode.connect(&test_ringcounter.Q_pins.pins[0]);

      


      printf("Node Value: %d\n", test_electricalnode.get_value());

      printf("input pin Value: %d\n", test_inputpin.get_value());
      printf("reistate pin Value: %d\n", test_tristatepin.get_value());
      printf("io pin Value: %d\n", test_iopin.get_value());

      groundnode.evaluate();
      rc_sernode.evaluate();

      return 0;
}

