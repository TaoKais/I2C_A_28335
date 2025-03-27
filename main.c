/********************************************
 * DISENO ELECTRONICO AVANZADO
 * Practica 4 - Interrupciones
 * Apartado 1
 *
 * Ramon J. Aliaga, Francisco J. Gimeno
 * Adaptado del original de Frank Bormann
 ********************************************/

 #include "headers/DSP2833x_Device.h"
 #include <math.h>
 #include "headers/DSP2833x_Examples.h"
 #include "headers/DSP2833x_GlobalPrototypes.h"


// Funciones externas
 extern void InitSysCtrl(void);
 extern void InitPieCtrl(void);
 extern void InitPieVectTable(void);
 extern void InitCpuTimers(void);
 extern void ConfigCpuTimer(struct CPUTIMER_VARS *, float, float);
//void delay_loop(long);

void Contador_INTS(void);
void AMBAR_INT(void);
void AMBAR_Peaton(void);
void ROJO(void);
void VERDE(void);
void Modif_Tiempos(void);



int i=0;
int A=0;
int p=0;
int counter=0;
unsigned long T=0;
// Declaracion de funciones
void Gpio_select(void);
interrupt void cpu_timer0_isr(void);


// Funcion main
void main(void)
 {

    // Inicializacion basica
    InitSysCtrl();  // DSP2833x_SysCtrl.c

    EALLOW;
    SysCtrlRegs.WDCR = 0x00AF;  // reactiva Watchdog
    EDIS;

    // Deshabilita interrupciones
    DINT;

    // Inicializa E/S
    Gpio_select();

    // Inicializa interrupciones
    InitPieCtrl();  // DSP2833x_PieCtrl.c
    InitPieVectTable();

    EALLOW;
    PieVectTable.TINT0 = &cpu_timer0_isr;  // registra ISR de Timer-0
    EDIS;

    // Inicializa timers
    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0,150,100000); // configura Timer-0 a 100 ms

    // Activa interrupcion del Timer-0
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    IER |= 1;

    // Habilita interrupciones
    EINT;
    ERTM;

    // Pone Timer-0 en marcha
    CpuTimer0Regs.TCR.bit.TSS = 0;

    // Bucle infinito
    while(1)
    {
        // Espera interrupcion
        while (CpuTimer0.InterruptCount == 0) ;
        CpuTimer0.InterruptCount = 0;

        EALLOW;
        SysCtrlRegs.WDKEY = 0x55;  // atiende watchdog #1
        EDIS;
        // Evalua contador

         Contador_INTS();
         Modif_Tiempos();



            if (i==0)   ROJO();
            if (i==2)   VERDE();
            if (i==1)   AMBAR_INT();
            if (p == 1 && i != 1) AMBAR_Peaton();


        }
}

void Modif_Tiempos(void)
{


          //   T = GpioDataRegs.GPADAT.bit.GPIO12*pow(2,0) + GpioDataRegs.GPADAT.bit.GPIO13*2*pow(2,1) + GpioDataRegs.GPADAT.bit.GPIO14*2*pow(2,2) + GpioDataRegs.GPADAT.bit.GPIO15*2*pow(3,2);

                    }



void ROJO(void)
{
                        GpioDataRegs.GPASET.bit.GPIO9 = 1;
                        GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
                        GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
                        GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;

                    }




void VERDE(void)

{
            GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
            GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
            GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
            GpioDataRegs.GPBSET.bit.GPIO49 = 1;

        }

void AMBAR_Peaton(void)

{

            GpioDataRegs.GPASET.bit.GPIO11 = 1;



        }

void AMBAR_INT(void)
{
          unsigned int blink=0;

 p=0;
          {
                  blink = counter;

                      if(blink & 1)
                      {
                          GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
                          GpioDataRegs.GPASET.bit.GPIO11 = 1;
                          GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
                          GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
                      }
                      else
                      {
                          GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
                          GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
                          GpioDataRegs.GPBSET.bit.GPIO34 = 1;
                          GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
                      }
          }
}



void Contador_INTS(void)
{
    //if (cpu_timer0_isr != 0) counter++;


        if(counter <= 100)   // esto act a de preescaler         ROJO 10s
        {
        i=0;

        }


        if(counter >= 100 && counter <= 100)  // esto act a de preescaler        Ambar 10s  (Intermitentes LED2 Y LED4)
            {
            i=1;
            p=0;
            }

        if(counter >= 200)  // esto act a de preescaler            Verde 20s
            {
            i=2;
            }



        if (counter >= 300)
        {
            i=0;
            counter=0;
             if (p == 1) p=0;
        }
}




// Inicializa E/S
void Gpio_select(void)
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.all = 0;       // GPIO15 ... GPIO0  = General Purpose I/O
    GpioCtrlRegs.GPAMUX2.all = 0;       // GPIO31 ... GPIO16 = General Purpose I/O
    GpioCtrlRegs.GPBMUX1.all = 0;       // GPIO47 ... GPIO32 = General Purpose I/O
    GpioCtrlRegs.GPBMUX2.all = 0;       // GPIO63 ... GPIO48 = General Purpose I/O
    GpioCtrlRegs.GPCMUX1.all = 0;       // GPIO79 ... GPIO64 = General Purpose I/O
    GpioCtrlRegs.GPCMUX2.all = 0;       // GPIO87 ... GPIO80 = General Purpose I/O

    GpioCtrlRegs.GPADIR.all = 0;        // GPIO31-0 --> entradas
    GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;  // GPIO9 --> salida (LED LD1 en Peripheral Explorer)
    GpioCtrlRegs.GPADIR.bit.GPIO11 = 1; // GPIO11 --> salida (LED LD2 en Peripheral Explorer)

    GpioCtrlRegs.GPBDIR.all = 0;        // GPIO63-32 --> entradas
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1; // GPIO34 --> salida (LED LD3 en Peripheral Explorer)
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1; // GPIO49 --> salida (LED LD4 en Peripheral Explorer)
    GpioCtrlRegs.GPCDIR.all = 0;        // GPIO87-64 --> entradas
    EDIS;
}


// ISR del Timer 0
interrupt void cpu_timer0_isr(void)
{
    CpuTimer0.InterruptCount++;
    EALLOW;
    SysCtrlRegs.WDKEY = 0xAA;  // atiende watchdog #2
    EDIS;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    counter++;
       if (GpioDataRegs.GPADAT.bit.GPIO17 == 0) p=1;



}
