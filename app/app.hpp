/*
 * Copyright (c) GrissinoPublishing 2024
 *
 *  Licenced under MIT Open Source Licence
 *
 */


#pragma once

#include <sim.hpp>
#include <Bus.hpp>
#include <EEPROM.hpp>
#include <Latch.hpp>
#include <Buffer.hpp>
#include <Counter.hpp>
#include <Clock.hpp>
#include <Logic.hpp>
#include <Decoder3to8.hpp>

#include <AT28C64.hpp>

#include <program.h>

#define DISABLE_IR_OUT

// #define MAIN_BUS_ID 250
// #define MAR_BUS_ID  259
// #define PC_BUS_ID   268
// #define IR_BUS_ID   277

// #define TEST_BENCH

#ifdef TEST_BENCH
extern int sim_test(int argc, char** argv);
// extern int run_test(int argc, char** argv);
#endif

using namespace DCSim;
using namespace Componenets;
using namespace Vendor;
using namespace Atmel;


// ==========================
// Electrical Sources

SourcePin Source;
GroundPin GND;

// ==========================
// Electrical Nodes

ElectricalNode ground_node;
ElectricalNode source_node;

ElectricalNode ME_node;
ElectricalNode WE_node;
ElectricalNode MCE_node;
ElectricalNode LM_Node;
ElectricalNode CP_Node;
ElectricalNode LP_Node;
ElectricalNode PE_Node;
ElectricalNode LI_Node;
ElectricalNode IE_Node;
ElectricalNode CLK_Node;

ElectricalNode RC1_Node;
ElectricalNode RC2_Node;
ElectricalNode RC3_Node;
ElectricalNode RC4_Node;
ElectricalNode RC5_Node;

ElectricalNode OPCode[4];

ElectricalNode NOPC4;

ElectricalNode NME_node;
ElectricalNode NWE_node;

// Add ability to automatically add itself to global list?
std::vector<ElectricalNode*> nodes = {
    &ground_node,
    &source_node,
    &ME_node,
    &WE_node,
    &MCE_node,
    &LM_Node,
    &CP_Node,
    &LP_Node,
    &PE_Node,
    &LI_Node,
    &IE_Node,
    &CLK_Node,
    &RC1_Node,
    &RC2_Node,
    &RC3_Node,
    &RC4_Node,
    &RC5_Node,
    &OPCode[0],
    &OPCode[1],
    &OPCode[2],
    &OPCode[3],
    &NOPC4,
    &NME_node,
    &NWE_node
};

// ==========================
// Components

// EEPROM/SRAM
AT28C64 eeprom;

// Memory Address Register and Instruction Register
Latch mar;
Latch ir;

// IR and PC bus buffers
Buffer irb;
Buffer pcb;

// Program counter
Counter pc;

// Ring Counter
RingCounter rc(5);

// Clock
Clock clk;

NotGate Not_OE;
NotGate Not_WE;
NotGate Not_IRLe;

// decoders
Decoder3to8 IRDecoderL;
Decoder3to8 IRDecoderH;

// Instruction Control Logic
// OR EEPROM Output Enable
OrGate OrOE;

// Buffer
Buffer Seq1Buffer;
Buffer Seq2Buffer;

// Sequencing Logic 
//OR gates

std::vector<Component*> parts = {
    &eeprom,
    &mar,
    &ir,
    &irb,
    &pc,
    &pcb,
    &rc,
    &clk,
    &Not_OE,
    &Not_WE,
    &Not_IRLe,
    &IRDecoderL,
    &IRDecoderH,
    &Seq1Buffer,
    &Seq2Buffer
};

// ==========================
// Bus nodes
Bus8bit mainBus;
Bus8bit marBus;
Bus8bit pcBus;
Bus8bit irBus;

Bus8bit controlLBus;
Bus8bit controlHBus;

std::vector<Bus8bit*> buses = {
    &mainBus,
    &marBus,
    &pcBus,
    &irBus,
    &controlLBus,
    &controlHBus
};

int main(int argc, char** argv);


const int MAIN_BUS_ID   {mainBus.get_uuid()};
const int MAR_BUS_ID    {marBus.get_uuid()};
const int PC_BUS_ID     {pcBus.get_uuid()};
const int IR_BUS_ID     {irBus.get_uuid()};

const int MAR_ID        {mar.get_uuid()};
const int MAR_D_PINS_ID {MAR_ID + 1};
const int MAR_Q_PINS_ID {MAR_ID + 9};

const int INSTR_ID          {ir.get_uuid()};
const int IR_D_PINS_ID      {INSTR_ID + 1};
const int IR_Q_PINS_ID      {INSTR_ID + 9};
