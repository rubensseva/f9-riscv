#ifndef ESP32_C3_H_
#define ESP32_C3_H_

#define DEFAULT_CPU_MHZ 20 // The default is XTAL_CLK divided by 2 = 20 MHz. TRM 6.2.4.1
#define CPU_MHZ DEFAULT_CPU_MHZ

/* Base addresses */
#define UART_CONTROLLER_0_BASE 0x60000000
#define SPI_CONTROLLER_1_BASE 0x60002000
#define SPI_CONTROLLER_0_BASE 0x60003000
#define GPIO_BASE 0x60004000
#define TIMER_BASE 0x60007000
#define LOW_POWER_MANAGEMENT_BASE 0x60008000
#define IO_MUX_BASE 0x60009000
#define UART_CONTROLLER_1_BASE 0x6001000
#define I2C_CONTROLLER_BASE 0x60013000
#define UHCIO_BASE 0x60014000
#define REMOTE_CONTROL_PERIPHERAL_BASE 0x60016000
#define LED_PWM_CONTROLLER_BASE 0x60019000
#define EFUSE_CONTROLLER_BASE 0x6001A000
#define TIMER_GROUP_0_BASE 0x6001F000
#define TIMER_GROUP_1_BASE 0x60020000
#define SYSTEM_TIMER_BASE 0x60023000
#define SPI_CONTROLLER_2_BASE 0x60024000
#define APB_CONTROLLER_BASE 0x60026000
#define TWO_WIRE_AUTOMOTIVE_INTERFACE_BASE 0x6002B000
#define I2S_CONTROLLER_BASE 0x6002D000
#define AES_ACCELERATOR_BASE 0x6003A000
#define SHA_ACCELERATOR_BASE 0x6003B000
#define RSA_ACCELERATOR_BASE 0x6003C000
#define DIGITAL_SIGNATURE_BASE 0x6003D000
#define HMAC_ACCELERATOR_BASE 0x6003E000
#define GDMA_CONTROLLER_BASE 0x6003F000
#define ADC_CONTROLLER_BASE 0x60040000
#define USB_SERIAL_JTAG_CONTROLLER_BASE 0x60043000
#define SYSTEM_REGISTERS_BASE 0x600C0000
#define SENSITIVE_REGISTER_BASE 0x600C1000
#define INTERRUPT_MATRIX_BASE 0x600C2000
#define CONFIGURE_CACHE_BASE 0x600C4000
#define EXTERNAL_MEMORY_ENCRYPTION_AND_DECRYPTION_BASE 0x600CC000
#define ASSIST_DEBUG_BASE 0x600CE000
#define WORLD_CONTROLLER_BASE 0x600D0000



/* System registers */
#define SYSTEM_CPU_PERI_CLK_EN_REG 0x0000
#define SYSTEM_CPU_PERI_RST_EN_REG 0x0004
#define SYSTEM_PERIP_CLK_EN0_REG 0x0010
#define SYSTEM_PERIP_CLK_EN1_REG 0x0014
#define SYSTEM_PERIP_RST_EN0_REG 0x0018
#define SYSTEM_PERIP_RST_EN1_REG 0x001C
#define SYSTEM_CACHE_CONTROL_REG 0x0040
#define SYSTEM_CPU_PER_CONF_REG 0x0008
#define SYSTEM_BT_LPCK_DIV_FRAC_REG 0x0024
#define SYSTEM_SYSCLK_CONF_REG 0x0058
#define SYSTEM_RTC_FASTMEM_CONFIG_REG 0x0048
#define SYSTEM_RTC_FASTMEM_CRC_REG 0x004C
#define SYSTEM_CPU_INTR_FROM_CPU_0_REG 0x0028
#define SYSTEM_CPU_INTR_FROM_CPU_1_REG 0x002C
#define SYSTEM_CPU_INTR_FROM_CPU_2_REG 0x0030
#define SYSTEM_CPU_INTR_FROM_CPU_3_REG 0x0034
#define SYSTEM_RSA_PD_CTRL_REG 0x0038
#define SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG 0x0044
#define SYSTEM_CLOCK_GATE_REG 0x0054
#define SYSTEM_DATE_REG 0x0FFC
#define APB_CTRL_CLKGATE_FORCE_ON_REG 0x00A4
#define APB_CTRL_MEM_POWER_DOWN_REG 0x00A8
#define APB_CTRL_MEM_POWER_UP_REG 0X00AC

/* SYSTEM_PERIP_CLK_EN0_REG bits */
#define SYSTEM_PERIP_CLK_EN0_REG__SYSTEM_UART_CLK_EN_BIT 2
#define SYSTEM_PERIP_CLK_EN0_REG__SYSTEM_UART1_CLK_EN_BIT 5
#define SYSTEM_PERIP_CLK_EN0_REG__SYSTEM_UART_MEM_CLK_EN_BIT 24

/* SYSTEM_PERIP_RST_EN0_REG bits */
#define SYSTEM_PERIP_RST_EN0_REG__SYSTEM_UART_MEM_RST_BIT 24
#define SYSTEM_PERIP_RST_EN0_REG__SYSTEM_UART_RST 2
#define SYSTEM_PERIP_RST_EN0_REG__SYSTEM_UART1_RST 5


/* GPIO registers */
/* Configuration Registers */
#define GPIO_BT_SELECT_REG 0x0000
#define GPIO_OUT_REG 0x0004
#define GPIO_OUT_W1TS_REG 0x0008
#define GPIO_OUT_W1TC_REG 0x000C
#define GPIO_ENABLE_REG 0x0020
#define GPIO_ENABLE_W1TS_REG 0x0024
#define GPIO_ENABLE_W1TC_REG 0x0028
#define GPIO_STRAP_REG 0x0038
#define GPIO_IN_REG 0x003C
#define GPIO_STATUS_REG 0x0044
#define GPIO_STATUS_W1TS_REG 0x0048
#define GPIO_STATUS_W1TC_REG 0x004C
#define GPIO_PCPU_INT_REG 0x005C
#define GPIO_PCPU_NMI_INT_REG 0x0060
#define GPIO_STATUS_NEXT_REG 0x014C
/* Pin Configuration Registers (other pins are offset from this address) */
#define GPIO_PIN0_REG 0x0074
/* Input Function Configuration Registers (other pins are offset from this address) */
#define GPIO_FUNC0_IN_SEL_CFG_REG 0x0154
/* Output Function Configuration Registers (other pins are offset from this address) */
#define GPIO_FUNC0_OUT_SEL_CFG_REG 0x0554
/* Version Register */
#define GPIO_DATE_REG 0x06FC
/* Clock Gate Register */
#define GPIO_CLOCK_GATE_REG 0x062C


/* IO MUX registers */
#define IO_MUX_PIN_CTRL_REG 0x0000
#define IO_MUX_GPIO0_REG 0X0004 // (other pins are offset from this address)




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

/* SYSTIMER_TARGET0_CONF_REG bits */
#define SYSTIMER_TARGET0_CONF_REG__SYSTIMER_TARGET0_PERIOD 0 // bits 0-25
#define SYSTIMER_TARGET0_CONF_REG__SYSTIMER_TARGET0_PERIOD_MODE 30
#define SYSTIMER_TARGET0_CONF_REG__SYSTIMER_TARGET0_TIMER_UNIT_SEL 31

/* SYSTIMER_CONF_REG bits */
#define SYSTIMER_CONF_REG__SYSTIMER_TARGET2_WORK_EN 22
#define SYSTIMER_CONF_REG__SYSTIMER_TARGET1_WORK_EN 23
#define SYSTIMER_CONF_REG__SYSTIMER_TARGET0_WORK_EN 24
#define SYSTIMER_CONF_REG__SYSTIMER_TIMER_UNIT1_CORE0_STALL_EN 26
#define SYSTIMER_CONF_REG__SYSTIMER_TIMER_UNIT0_CORE0_STALL_EN 28
#define SYSTIMER_CONF_REG__SYSTIMER_TIMER_UNIT1_WORK_EN 29
#define SYSTIMER_CONF_REG__SYSTIMER_TIMER_UNIT0_WORK_EN 30
#define SYSTIMER_CONF_REG__SYSTIMER_CLK_EN 31

/* Registers for UART */
/* FIFO Configuration */
#define UART_FIFO_REG 0x0000 // RO // FIFO data register
#define UART_MEM_CONF_REG 0x0060 // R/W // 0x0060
/* UART Interrupt Register */
#define UART_INT_RAW_REG 0x0004 // R/WTC/SS // Raw interrupt status
#define UART_INT_ST_REG 0x0008 // RO // Masked interrupt status
#define UART_INT_ENA_REG 0x00C // R/W // Interrupt enable bits
#define UART_INT_CLR_REG 0x0010 // WT // Interrupt clear bits
/* Configuration Register */
#define UART_CLKDIV_REG 0x0014 // R/W // Clock divider configuration
#define UART_RX_FILT_REG 0x0018 // R/W // RX filter configuration
#define UART_CONF0_REG 0x0020 // R/W // Configuration register 0
#define UART_CONF1_REG 0x0024 // R/W // Configuration register 1
#define UART_FLOW_CONF_REG 0x0034 // varies // Software flow control configuration
#define UART_SLEEP_CONF_REG 0X0038 // R/W // Sleep mode configuration
#define UART_SWFC_CONF0_REG 0x003C // R/W // Software flow control character configuration
#define UART_SWFC_CONF1_REG 0x0040 // R/W // Software flow control character configuration
#define UART_TXBRK_CONF_REG 0x0044 // R/W // TX break character configuration
#define UART_IDLE_CONF_REG 0x0048 // R/W // Frame end idle time configuration
#define UART_RS485_CONF_REG 0x004C // R/W // RS485 mode configuration
#define UART_CLK_CONF_REG 0x0078 // R/W // UART core clock configuration
/* Status Register */
#define UART_STATUS_REG 0x001C // RO // UART status register
#define UART_MEM_TX_STATUS_REG 0x0064 // RO // TX FIFO write and read offset address
#define UART_MEM_RX_STATUS_REG 0x0068 // RO // RX FIFO write and read offset address
#define UART_FSM_STATUS_REG 0x006C // RO // UART transmitter and receiver status
/* Autobaud Register */
#define UART_LOWPULSE_REG 0x0028 // RO // Autobaud minimum low pulse duration register
#define UART_HIGHPULSE_REG 0x002C // RO // Autobaud minimum high pulse duration register
#define UART_RXD_CNT_REG 0x0030 // RO // Autobaud edge change count register
#define UART_POSPULSE_REG 0x0070 // RO // Autobaud high pulse register
#define UART_NEGPULSE_REG 0x0074 // RO // Autobaud low pulse register
/* AT Escape Sequence Selection Configuration */
#define UART_AT_CMD_PRECNT_REG 0x0050 // R/W // Pre-sequence timing configuration
#define UART_AT_CMD_POSTCNT_REG 0x0054 // R/W // Post-sequence timing configuration
#define UART_AT_CMD_GAPTOUT_REG 0x0058 // R/W // Timeout configuration
#define UART_AT_CMD_CHAR_REG 0x005C // R/W // AT escape sequence detection configuration
/* Version Register */
#define UART_DATE_REG 0x007C // R/W // UART version control register
#define UART_ID_REG 0x0080 // varies // UART ID register


/* UART_CONF0_REG bits */
#define UART_CONF0_REG__UART_PARITY 0
#define UART_CONF0_REG__UART_PARITY_EN 1
#define UART_CONF0_REG__UART_BIT_NUM 2
#define UART_CONF0_REG__UART_STOP_BIT_NUM 4

/* UART_CONF1_REG bits */
#define UART_CONF1_REG__UART_RXFIFO_FULL_THRHD 0 // bits 0 to 8
#define UART_CONF1_REG__UART_TXFIFO_EMPTY_THRHD 9 // bits 9 to 17
#define UART_CONF1_REG__UART_DIS_RX_DAT_OVF 18
#define UART_CONF1_REG__UART_RX_TOUT_FLOW_DIS 19
#define UART_CONF1_REG__UART_RX_FLOW_EN 20
#define UART_CONF1_REG__UART_RX_TOUT_EN 21

/* UART_CLK_CONF_REG bits */
#define UART_CLK_CONF_REG__UART_SCLK_DIV_B_BIT 0 // bits 0 to 5
#define UART_CLK_CONF_REG__UART_SCLK_DIV_A_BIT 6 // bits 6 to 11
#define UART_CLK_CONF_REG__UART_SCLK_DIV_NUM_BIT 12 // bits 12 to 19
#define UART_CLK_CONF_REG__UART_RST_CORE_BIT 23
#define UART_CLK_CONF_REG__UART_SCLK_SEL_BIT 20 // bits 20 and 21
#define UART_CLK_CONF_REG__UART_SCLK_EN_BIT 22
#define UART_CLK_CONF_REG__UART_TX_SCLK_EN_BIT  24
#define UART_CLK_CONF_REG__UART_RX_SCLK_EN_BIT  25

/* UART_CLKDIV_REG bits */
#define UART_CLKDIV_REG__UART_CLKDIV_BIT 0 // bits 0 to 11
#define UART_CLKDIV_REG__UART_CLKDIV_FRAG 20 // bits 20 to 23

/* UART clock sources */
#define APB_CLK 1
#define FOSC_CLK 2
#define XTAL_CLK 3

/* UART_ID_REG bits */
#define UART_ID_REG__UART_UPDATE_CTRL 30
#define UART_ID_REG__UART_REG_UPDATE 31

/* UART_STATUS_REG bits */
#define UART_STATUS_REG__UART_RXFIFO_CNT_BIT 0 // bits 0 to 9
#define UART_STATUS_REG__UART_DSRN_BIT 13
#define UART_STATUS_REG__UART_CTSN_BIT 14
#define UART_STATUS_REG__UART_RXD_BIT 15
#define UART_STATUS_REG__UART_TXFIFO_CNT_BIT 16 // bits 16 to 25
#define UART_STATUS_REG__UART_DTRN_BIT 29
#define UART_STATUS_REG__UART_RTSN_BIT 30
#define UART_STATUS_REG__UART_TXD_BIT 31

/* UART interrupt bits */
#define UART_INTR__UART_RXFIFO_FULL 0
#define UART_INTR__UART_TXFIFO_EMPTY 1
#define UART_INTR__UART_PARITY_ERR 2
#define UART_INTR__UART_FRM_ERR 3
#define UART_INTR__UART_RXFIFO_OVF 4
#define UART_INTR__UART_DSR_CHG 5
#define UART_INTR__UART_CTS_CHG 6
#define UART_INTR__UART_BRK_DET 7
#define UART_INTR__UART_RXFIFO_TOUT 8
#define UART_INTR__UART_SW_XON 9
#define UART_INTR__UART_SW_XOFF 10
#define UART_INTR__UART_GLITCH_DET 11
#define UART_INTR__UART_TX_BRK_DONE 12
#define UART_INTR__UART_TX_BRK_IDLE_DONE 13
#define UART_INTR__UART_TX_DONE 14
#define UART_INTR__UART_RS485_PARITY_ERR 15
#define UART_INTR__UART_RS485_FROM_ERR 16
#define UART_INTR__UART_RS485_CLASH 17
#define UART_INTR__UART_AT_CMD_CHAR_DET 18
#define UART_INTR__UART_WAKEUP 19


/* IO MUX */
#define IO_MUX_GPIOn_REG 0x0004 // n: 0-21, (0x0004 + 4*n)
#define GPIO_FUNCn_IN_SEL_CFG_REG 0x0154 // (n: 0­127) (0x0154+4*n)

#define IO_MUX_GPIOn_REG__IO_MUX_GPIOn_MCU_OE_BIT 0
#define IO_MUX_GPIOn_REG__IO_MUX_GPIOn_SLP_SEL_BIT 1
#define IO_MUX_GPIOn_REG__IO_MUX_GPIOn_MCU_WPD_BIT 2
#define IO_MUX_GPIOn_REG__IO_MUX_GPIOn_MCU_WPU_BIT 3
#define IO_MUX_GPIOn_REG__IO_MUX_GPIOn_MCU_IE_BIT 4
#define IO_MUX_GPIOn_REG__IO_MUX_GPIOn_FUN_WPD_BIT 7
#define IO_MUX_GPIOn_REG__IO_MUX_GPIOn_FUN_WPU_BIT 8
#define IO_MUX_GPIOn_REG__IO_MUX_GPIOn_FUN_IE_BIT 9
#define IO_MUX_GPIOn_REG__IO_MUX_GPIOn_FUN_DRV_BIT 10 // bits 10 - 11
#define IO_MUX_GPIOn_REG__IO_MUX_GPIOn_MCU_SEL_BIT 12 // bits 12 - 14
#define IO_MUX_GPIOn_REG__IO_MUX_GPIOn_FILTER_EN_BIT 15

#define GPIO_FUNCn_IN_SEL_BIT 0 // bits 0 - 4
#define GPIO_FUNCn_IN_INV_SEL_BIT 5
#define GPIO_SIGn_IN_SEL_BIT 6


/* TIMERS */
#define TIMG_T0CONFIG_REG 0x0000
#define TIMG_T0LO_REG 0x0004
#define TIMG_T0HI_REG 0x0008
#define TIMG_T0UPDATE_REG 0x000C
#define TIMG_T0ALARMLO_REG 0x0010
#define TIMG_T0ALARMHI_REG 0x0014
#define TIMG_T0LOADLO_REG 0x0018
#define TIMG_T0LOADHI_REG 0x001C
#define TIMG_T0LOAD_REG 0x0020

/* INTERRUPT MATRIX */
#define INTERRUPT_CORE0_CPU_INT_ENABLE_REG 0x0104

/* Interrupt mapping registers */
#define INTERRUPT_CORE0_UART_TARGET0_INT_MAP_REG 0x0054
#define INTERRUPT_CORE0_UART1_TARGET0_INT_MAP_REG 0x0058
#define INTERRUPT_CORE0_SYSTIMER_TARGET0_INT_MAP_REG 0x0094
#define INTERRUPT_CORE0_CPU_INT_TYPE_REG 0x0108
#define INTERRUPT_CORE0_CPU_INT_CLEAR_REG 0x010C
#define INTERRUPT_CORE0_CPU_INT_PRI_n_REG 0x0118 // to get intr n, use + 0x4*n. BUT WATCH OUT! n goes from 1 - 31, but I believe this formula assumes 0 - 30
#define INTERRUPT_CORE0_CPU_INT_THRESH_REG 0x0194


#endif // ESP32_C3_H_
