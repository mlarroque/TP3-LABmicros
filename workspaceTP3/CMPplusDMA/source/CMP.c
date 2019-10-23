/*
 * CMP.c
 *
 *  Created on: Oct 21, 2019
 *      Author: Lu
 */

#include "MK64F12.h"

#define CMP_ANALOG_IN 1U // PTC7, CMP0_IN1				//PTC3, CMP1_IN1
#define CMP_ANALOG_REF 7U	//elijo dac
#define HYST_MASK 0U
#define DAC_VALUE 32U
#define VREF_SOURCE_VINX 0U

							//CMP0_IN1			//CMP1_IN1
uint8_t CMP_in_pins[] = {PORTNUM2PIN(PC, 7), PORTNUM2PIN(PC, 3)};

							//CMP0_OUT			//CMP1_OUT
uint8_t CMP_out_pins[] = {PORTNUM2PIN(PC, 5), PORTNUM2PIN(PC, 4)};


CMP_Type * arrayP2CMP[] = CMP_BASE_PTRS;
IRQn_Type arrayCMPirqs[] = CMP_IRQS;

static void CMPClockGating();


void CMPClockGating(){
	SIM_Type* sim;
	sim->SIM_SCGC4 |= SIM_SCGC4_CMP_MASK;
	sim->SCGC5 |= SIM_SCGC5_PORTC_MASK;

}

void CMPConfigurePins(uint8_t id){
	uint8_t cmp_in_mux = 0;
	uint8_t cmp_out_mux = 6;
	switch(channel)
	{
	case CMP_0:
		cmp_in_mux = 0;
		cmp_out_mux = 6;
		break;
	case CMP_1:
		cmp_in_mux = 0;
		cmp_out_mux = 6;
		break;
	}
	PORT_Type * portPointers[] = PORT_BASE_PTRS;

	uint8_t port_in = PIN2PORT(CMP_in_pins[id]);
	uint8_t num_pin_in= PIN2NUM(CMP_in_pins[id]);

	uint8_t port_out = PIN2PORT(CMP_out_pins[id]);
	uint8_t num_pin_out = PIN2NUM(CMP_out_pins[id]);

	setPCRmux(portPointers[port_in], num_pin_in, cmp_in_mux);
	setPCRmux(portPointers[port_out], num_pin_out, cmp_out_mux);
	setPCRirqc(portPointers[port_in], num_pin_in, IRQ_MODE_DISABLE); //deshabilito interrupciones de puerto
	setPCRirqc(portPointers[port_out], num_pin_out, IRQ_MODE_DISABLE);


	setPCRpullEnable(portPointers[port_in], num_pin_in);
	setPCRpullUp(portPointers[port_in], num_pin_in);
	setPCRpullEnable(portPointers[port_out], num_pin_out);
	setPCRpullUp(portPointers[port_out], num_pin_out);
}


void initCMP(cmps_ids id){

	CMP_Type* base = arrayP2CMP[id];	//todo hacerlo generico
	CMPClockGating();
	CMPConfigurePins();		//solo CMP0

	//windowed mode for zero crossing detection
	base->CR1 |= CMP_CR1_WE_MASK;

	//todo debug para que la salida salga por el pin
	base->CR1 |= CMP_CR1_OPE_MASK;

	//histeresis
	base->CR0 |= CMP_CR0_HYSTCTR(HYST_MASK);


	configureDAC(CMP_Type* base);
	void setInputs(CMP_Type* base, uint8_t positiveInput, uint8_t negativeInput);

	//setEnableDMA()
	base->CR1 |= CMP_CR1_EN_MASK;		//enable module
	NVIC_EnableIRQ(arrayCMPirqs[id]);
}


void setInputs(CMP_Type* base, uint8_t positiveInput, uint8_t negativeInput){

	uint8_t mask = base->MUXCR;

	//borro los selectores que tiene
	mask &= ~(uint8_t)(CMP_MUXCR_PSEL_MASK | CMP_MUXCR_MSEL_MASK);
	mask |= (CMP_MUXCR_MSEL(negativeInput) | CMP_MUXCR_PSEL(positiveInput));
	base->MUXCR = mask;
}


void configureDAC(CMP_Type* base){

	uint8_t vref_source_vinX = VREF_SOURCE_VINX;
	uint8_t dac_value = DAC_VALUE;                 // half voltage of logic high level

	uint8_t mask = 0U;
	mask |= CMP_DACCR_DACEN_MASK;	//enable dac
	if(vref_source_vinX == 1U)			//si se elige vin2
		mask |= CMP_DACCR_VRSEL_MASK;	//pongo mascara para vin2, vin1 es default

	mask |= CMP_DACCR_VOSEL(dac_value);

	base = mask;
}

void setEnableDMA(){
	//todo
}



void CMP0_IRQHandler(void){


}