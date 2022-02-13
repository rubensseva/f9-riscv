#ifndef ESP32-C3_H_
#define ESP32-C3_H_


/* Registers for system timer
   the comments after the entries are taken from the "description" collumn in the docs */

/* Clock Control Register */
#define SYSTIMER_CONF_REG 0x0000 // Configure system timer clock
/* UNIT0 Control and Configuration Registers */
#define SYSTIMER_UNIT0_OP_REG 0x0004 // Read UNIT0 value to registers
#define SYSTIMER_UNIT0_LOAD_HI_REG 0x000C // High 20 bits to be loaded to UNIT0
#define SYSTIMER_UNIT0_LOAD_LO_REG 0x0010 // Low 32 bits to be loaded to UNIT0
#define SYSTIMER_UNIT0_VALUE_HI_REG 0x0040 // UNIT0 value, high 20 bits
#define SYSTIMER_UNIT0_VALUE_LO_REG 0x0044 // UNIT0 value, low 32 bits
#define SYSTIMER_UNIT0_LOAD_REG 0x005C // UNIT0 synchronization register
/* UNIT1 Control and Configuration Registers */
#define SYSTIMER_UNIT1_OP_REG 0x0008 // Read UNIT1 value to registers
#define SYSTIMER_UNIT1_LOAD_HI_REG 0x0014 // High 20 bits to be loaded to UNIT1
#define SYSTIMER_UNIT1_LOAD_LO_REG 0x0018 // Low 32 bits to be loaded to UNIT1
#define SYSTIMER_UNIT1_VALUE_HI_REG 0x0048 // UNIT1 value, high 20 bits
#define SYSTIMER_UNIT1_VALUE_LO_REG 0x004C // UNIT1 value, low 32 bits
#define SYSTIMER_UNIT1_LOAD_REG 0x0060 // UNIT1 synchronization register
/* Comparator0 Control and Configuration Registers */
#define SYSTIMER_TARGET0_HI_REG 0x001C // Alarm value to be loaded to COMP0, high 20 bits
#define SYSTIMER_TARGET0_LO_REG 0x0020 // Alarm value to be loaded to COMP0, low 32 bits
#define SYSTIMER_TARGET0_CONF_REG 0x0034 // Configure COMP0 alarm mode
#define SYSTIMER_COMP0_LOAD_REG 0x0050 // COMP0 synchronization register
/* Comparator1 Control and Configuration Registers */
#define SYSTIMER_TARGET1_HI_REG 0x0024 // Alarm value to be loaded to COMP1, high 20 bits
#define SYSTIMER_TARGET1_LO_REG 0x0028 // Alarm value to be loaded to COMP1, low 32 bits
#define SYSTIMER_TARGET1_CONF_REG 0x0038 // Configure COMP1 alarm mode
#define SYSTIMER_COMP1_LOAD_REG 0x0054 // COMP1 synchronization register
/* Comparator2 Control and Configuration Registers */
#define SYSTIMER_TARGET2_HI_REG 0x002C // Alarm value to be loaded to COMP2, high 20 bits
#define SYSTIMER_TARGET2_LO_REG 0x0030 // Alarm value to be loaded to COMP2, low 32 bits
#define SYSTIMER_TARGET2_CONF_REG 0x003C // Configure COMP2 alarm mode
#define SYSTIMER_COMP2_LOAD_REG 0x0058 // COMP2 synchronization register
/* Interrupt Registers */
#define SYSTIMER_INT_ENA_REG 0x0064 // Interrupt enable register of system timer
#define SYSTIMER_INT_RAW_REG 0x0068 // Interrupt raw register of system timer
#define SYSTIMER_INT_CLR_REG 0x006C // Interrupt clear register of system timer
#define SYSTIMER_INT_ST_REG  0x0070 // Interrupt status register of system timer
/* Version Regiser */
#define SYSTIMER_DATE_REG 0x00FC // Version control register

#endif // ESP32-C3_H_
