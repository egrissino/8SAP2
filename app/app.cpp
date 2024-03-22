/*
 * Copyright (c) GrissinoPublishing 2024
 *
 *  Licenced under MIT Open Source Licence
 *
 */

/*

            marBus
    _______       __________
    | MAR | ----- | EEPROM |- - - - ME, WE, LM
    -------       ----------
     D |              | Q
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
              
              
     _______     ______
     | CLK |- - -| RC |
     -------     ------


*/    

// #define DEBUG_PRINT

#include <cstdio>
#include <app.hpp>

using namespace DCSim;
using namespace Componenets;

// ==============================================
// Simulation params
const float timestep = 0.001;
const float start_time = 0;
const float end_time = 1;
float time_sec;

// ==============================================
// Bus components attachments

void attachComponents() 
{
    printf("Attahcing Bus Components\n");
    
    // EEPROM and MAR
    pinGroup_t addr8 = {N_BUS_BITS, eeprom.addr_pins.pins};
    marBus.attach(addr8);
    marBus.attach(mar.Q_pins);
    
    // main bus attach EEPROM subsys
    mainBus.attach({N_BUS_BITS, &eeprom.io_pins.pins[0]});
    mainBus.attach(mar.D_pins);
    
    // PC (and PC buffer)
    pcBus.attach(pc.Q_pins);
    pcBus.attach(pcb.D_pins);
    
    // main bus attach PC subsys
    mainBus.attach(pc.D_pins);
    mainBus.attach(pcb.Q_pins);
    
    // IR (and IR Buffer)
    irb.D_pins.num_pins = 4;
    irb.Q_pins.num_pins = 4;
    irBus.attach(ir.Q_pins);
    irBus.attach(irb.D_pins);
    
    // main bus attach IR subsys
    mainBus.attach(ir.D_pins);
    mainBus.attach(irb.Q_pins);

    // 16 bit control bus
    controlLBus.attach(IRDecoderL.Q_pins);
    controlHBus.attach(IRDecoderH.Q_pins);
    
}

void setup() 
{
    // One time setup
    attachComponents();

    // Print Buses
    printf ("Main Bus: %d\n", mainBus.get_uuid());
    printf ("MAR Bus: %d\n",marBus.get_uuid());
    printf ("PC Bus: %d\n", pcBus.get_uuid());
    printf ("IR Bus: %d\n", irBus.get_uuid() );
    
    // Print Components
    printf("Mar ID : %d \n", mar.get_uuid());
    printf("Ir ID  : %d \n", ir.get_uuid());
    printf("IrB ID : %d \n", irb.get_uuid());
    printf("PcB ID : %d \n", pcb.get_uuid());
    printf("Pc ID  : %d \n", pc.get_uuid());
    printf("Rc ID  : %d \n", rc.get_uuid());
    printf("clk ID : %d \n", clk.get_uuid());
    
    // ==========================
    // GND and VCC Connections
    ground_node.connect(&GND);
    source_node.connect(&Source);

    // EEprom control nodes
    NME_node.connect(&eeprom.OutputEnable);
    NME_node.connect(&Not_OE.Out1);

    // Disable address bits 8->12
    for (int i = eeprom.io_pins.num_pins; i < eeprom.addr_pins.num_pins; i++) 
    {
        ground_node.connect(&eeprom.addr_pins.pins[i]);
    }
    
    // EEPROM NWE pin connect to node, ground in put to not gate
    NWE_node.connect(&eeprom.WriteEnable);
    NWE_node.connect(&Not_WE.Out1);
    ground_node.connect(&Not_WE.In1);
    
    // EEProm Chip enable LOW
    ground_node.connect(&eeprom.ChipEnable);

    // Connect MAR Latch
    source_node.connect(&mar.OutputEnable);
    
    // Clock enable
    clk.set_frequency(100.0);
    source_node.connect(&clk.Enable);
    
    // unclear counters
    source_node.connect(&rc.Clear);
    source_node.connect(&pc.Clear);

    // disable laod for now
    source_node.connect(&pc.Load);

    // PC setup
    source_node.connect(&pc.OutputEnable);

    // IR setup
    source_node.connect(&ir.OutputEnable);
    #ifdef DISABLE_IR_OUT
    ground_node.connect(&irb.OutputEnable);
    #endif
    
    // Connect clock to ring counter
    CLK_Node.connect(&clk.Clk);
    CLK_Node.connect(&rc.Clock);
    CLK_Node.connect(&pc.Clock);
    
    // Connect ring counter to 
    RC1_Node.connect(&rc.Q_pins.pins[0]);
    RC2_Node.connect(&rc.Q_pins.pins[1]);
    RC3_Node.connect(&rc.Q_pins.pins[2]);
    RC4_Node.connect(&rc.Q_pins.pins[3]);
    RC5_Node.connect(&rc.Q_pins.pins[4]);

    /*
    op    Seq     Control
    4  ->  16  ->  20
    */

    /*
    -> 4 bit op code           
        (IR[4:]) 
        0  |  2^0 (1)
        1  |  2^1 (2)
        2  |  2^2 (4)
        3  |  2^4 (8)
    */
    // Connect Op decoder to high 4 bits of IR
    for (int i = 0;i < 4;i++) 
    {
        OPCode[i].connect(&ir.Q_pins.pins[i+4]);
    }

    // Connect OPCode to the decoders
    for (int i = 0;i < 3;i++) 
    {
        OPCode[i].connect(&IRDecoderL.D_pins.pins[i]);
        OPCode[i].connect(&IRDecoderH.D_pins.pins[i]);
    }
    
    // Op code high bit should be tied to both deoceders enable, IRL_OE = ! Opcode[3]
    OPCode[3].connect(&Not_IRLe.In1);
    OPCode[3].connect(&IRDecoderH.OutputEnable);
    
    NOPC4.connect(&Not_IRLe.Out1);
    NOPC4.connect(&IRDecoderL.OutputEnable);
    /*
    -> 16 bit Instruction bus
        ( 2 x Decoder3to8 ) decodes to 16 channels
        0  |  NOP
        1  |  LDA
        2  |  LDB
        3  |  STR 
        4  |  ADD
        5  |  SUB
        6  |  CLR
        7  |  OUT
        8  |  LDM
        9  |  JPZ
        10 |  JPC
        11 |  JMP
        12 |  STM
        13 |  MOV
        14 |  SFT
        15 |  HLT

         instruction decoder     ( OR gates )
    */
    

    /*
    -> 16 bit Sequencer
        ( 2 x Buffers )
        Buffer1 - Sequence RC4 : Instruction Execute 1
            0  |  LM_1
            1  |  LS_1
            2  |  EI_1
            3  |  AE_1
            4  |  LA_1
            5  |  LB_1
            6  |  SH_1
            7  |  RS_1
        Buffer2 - Sequence RC5 : Instruction Execute 2
            0  |  LA_2
            1  |  LB_2
            2  |  LO_2
            3  |  ME_2
            4  |  AE_2
            5  |  LP_2
            6  |  UE_2
            7  |  WE_2

        sequence decoder
            ( OR gates )
    */
    // Sequencing buffers for instructions
    RC4_Node.connect(&Seq1Buffer.OutputEnable);
    RC5_Node.connect(&Seq2Buffer.OutputEnable);

    
    /*
    -> 20 bit Control output Bus      (8 Fixed control) + (12 bit Sequenced control)

        // Fixed Sequence Outs (Fetch & Modify)
        0 |  PC_Out              // Fetch Outputs
        1 |  PC_Count
        2 |  IR_Load
        3 |  - reserved
        4 |  Subtract            // Instruction Modify outputs
        5 |  JumpZero
        6 |  JumpCarry
        7 |  HALT

        // Buffered Outs (Execute)
        0  |  MAR_Load 
        1  |  A_Load
        2  |  B_Load
        3  |  PC_Load
        4  |  Out_Load
        5  |  RAM_Out
        6  |  A_Out
        7  |  IR_Out
        8  |  Adder_Out
        9  |  SR_Load
        10 |  RAM_WE
        11 |  SFT
    */

    // Connect control to ring counter
    // Fixed Instruction is Fetch
    RC1_Node.connect(&pcb.OutputEnable);
    RC1_Node.connect(&mar.LatchEnable); // add to OR Gate
    RC2_Node.connect(&pc.Count);
    RC3_Node.connect(&Not_OE.In1);      // Add to OR Gate
    RC3_Node.connect(&ir.LatchEnable);

    // LA 4OR LDA, ADD, LDM

    // LDA
    // IE, LM | ME, LA

    // LDB
    // IE, LM | ME, LB

    // STR
    // IE, LM, | AE, WE

    // ADD (SUB)
    // (SU)
    // LF | UE, LA

    // CLR
    // | <LA; LB; LO; LP; LM; LF;> 

    // OUT
    // | AE, LO

    // LDM
    //AE, LM | ME, LA

    // AO, LP?
    // JPZ
    // Z !
    // EI, LM | ME, LP

    // JPC
    // C !
    // EI, LM | ME, LP

    // JMP
    // EI, LM | ME, LP

    // STM
    // AE, LM | UE, WE

    // MOV
    // | AE, LB

    // Print info for init
    printf("VCC   PinID: %d \t| PinState   : %d\n", Source.get_id(), Source.get_state());
    printf("ClkEn PinID: %d \t| PinState : %d\n", clk.Enable.get_id(),clk.Enable.get_state());
}

void debugPrint ()
{
    // printf("RC1: %d |\t", RC1_Node.get_value());
    // printf("RC2: %d |\t", RC2_Node.get_value());
    // printf("RC3: %d |\t", RC3_Node.get_value());
    // printf("RC4: %d |\t", RC4_Node.get_value());
    // printf("RC5: %d |\t", RC5_Node.get_value());
    
    printf("T: %f |\t C: %d |\t", time_sec, clk.Clk.get_value());
    printf("Bus: %d |\t", mainBus.get_value().byte);
    printf("PC: %d |\t", pc.get_value());

    // printf("PCp: %d \t", getPinGroup(pc.Q_pins));
    // printf("PCBus: %d \t", pcBus.get_value().byte);
    // printf("PCBufferI: %d \t", getPinGroup(pcb.D_pins));
    // printf("PCBufferO: %d \t", getPinGroup(pcb.Q_pins));
    // printf("Mar: %d \t", getPinGroup(mar.Q_pins));

    printf("Mar: %d |\t", mar.getLatchValue());
    // printf("MarBus: %d \n", marBus.get_value().byte);
    // printf("MemIO: %d \n", getPinGroup(eeprom.io_pins));

    printf("Ir:  0x%02x |\t", (uint8_t)ir.getLatchValue());
    printf("Op:  0b%1d%1d%1d%1d |\t",(bool)OPCode[3].get_value(), (bool)OPCode[2].get_value(), (bool)OPCode[1].get_value(), (bool)OPCode[0].get_value());
    printf("CT:  0x%02x%02x |\t", (uint8_t)controlHBus.get_value().byte, (uint8_t)controlLBus.get_value().byte );
    
    printf ("IRDLO: %d |\t", getPinGroup(IRDecoderL.Q_pins));
    printf ("IRDHO: %d |\t", getPinGroup(IRDecoderH.Q_pins));

    // printf ("RC4 : %d |\t", (bool)OPCode[3].get_value());
    // printf ("NRC4: %d |\t", NOPC4.get_value());
    // printf("PC bits: %d %d %d %d |\t", pc.Q[0].get_value(), pc.Q[1].get_value(), pc.Q[2].get_value(), pc.Q[3].get_value());
}


void run () 
{
    printf(" = = = 8SAP1 = = = \n");
    
    time_sec = start_time;
    int const n_steps = (end_time - start_time)/timestep + 1;
    printf("timesteps: %d \n", n_steps);
    
    bool clock = false;
    
    // Initialize logs
    float time_sig[n_steps] = {0};
    bool clk_sig[n_steps] = {0};
    bool rc1_sig[n_steps] = {0};
    bool rc2_sig[n_steps] = {0};
    bool rc3_sig[n_steps] = {0};
    bool rc4_sig[n_steps] = {0};
    bool rc5_sig[n_steps] = {0};

    // TODO : - FIX this! its crashig the progam
    eeprom.loadProgram((uint8_t*)test_program_01, PROGRAM_SZIE);

    for (int i = 0; i < n_steps; i++) 
    {
       printf("\nTimestep: %d |\t", i);
        
        // record time and set clock
        time_sig[i] = time_sec;
        clk.set_time(time_sec);
        
        // Evaluate Nodes
        for (ElectricalNode* node : nodes) 
        {
            node->evaluate();
        }
        
        // Evaluate Components
        for (Component* part : parts) 
        {
            //printf("p: %0lx\n", (uint64_t)part);
            part->evaluate();
        }
        
        // Evaluate Buses
        for (Bus8bit* bus : buses)
        {
            bus->evaluate();
        }
        
        // Evaluate Clock
        if ( (i==0) || (!clock && (clk.Clk.get_value() == kLogicHigh)) ) 
        {
            debugPrint ();
            clock = (bool)kLogicHigh;
        }
        
        if ( clock && (clk.Clk.get_value() == kLogicLow) ) 
        {
            clock = (bool)kLogicLow;
        }
        
        // Logging dump
        clk_sig[i] = clk.Clk.get_value();
        rc1_sig[i] = RC1_Node.get_value();
        rc2_sig[i] = RC2_Node.get_value();
        rc3_sig[i] = RC3_Node.get_value();
        rc4_sig[i] = RC4_Node.get_value();
        rc5_sig[i] = RC5_Node.get_value();
                
        time_sec += timestep;
    }
}

int main(int argc, char** argv)
{

    #ifdef TEST_BENCH
    sim_test(argc, (char**)argv);
    #else
    // Setup routine
    setup();
    
    // Run simulation
    run();
    #endif
    
    return 0;
}
