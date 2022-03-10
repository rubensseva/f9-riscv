#ifndef CONFIG_H
#define CONFIG_H

enum register_stack_t {
	REG_A0,
	REG_A1,
	REG_A2,
	REG_A3,
	REG_A4,
	REG_A5,
	REG_A6,
    REG_A7,
	REG_RA,
	REG_SP,
	REG_GP,
	REG_TP,
	REG_T0,
	REG_T1,
	REG_T2,
	REG_T3,
	REG_T4,
	REG_T5,
	REG_T6,
	REG_S0,
	REG_S1,
	REG_S2,
	REG_S3,
	REG_S4,
	REG_S5,
	REG_S6,
	REG_S7,
	REG_S8,
	REG_S9,
	REG_S10,
	REG_S11
};

// TODO: Find a nice value
#define IRQn_NUM 128

// These could be deleted since we dont care about
// tickless timer for now
#define CONFIG_KTIMER_MINTICKS 128


// from /f9-kernel/include/platform/armv7m.h"
// TODO: Need to know more about what this does
#define RESERVED_STACK (32 * sizeof(uint32_t))

#define CONFIG_BITMAP_BITBAND 0

#define CONFIG_SYSTEM_TIMER_CPU_INTR 7

// From old kconfig
// -----------------------------
// platform
// CONFIG_BOARD_STM32F4DISCOVERY=y
// CONFIG_BOARD_STM32F429DISCOVERY is not set
// CONFIG_BOARD_STM32P103 is not set
// CONFIG_BITMAP_BITBAND is not set
// CONFIG_FPU is not set
// CONFIG_STDIO_NODEV is not set
// CONFIG_STDIO_USE_DBGPORT=y
// CONFIG_DBGPORT_USE_USART1 is not set
// CONFIG_DBGPORT_USE_USART2 is not set
// CONFIG_DBGPORT_USE_USART4=y

// Memory regions
#define CONFIG_PERIPHERAL_MEM_START 0x60000000
/* HACK: Peripheral size is actually 836KB, but we only have 16 bit fpage and we dont
   need the peripherals at higher addresses for now, so lets just say that peripheral
   memory is smaller than it actually is */
/* FIXME: allow for larger fpages, or split them if size is too large */
#define CONFIG_PERIPHERAL_MEM_SIZE 60000

//
// Limitations
//
#define CONFIG_MAX_THREADS 32
#define CONFIG_MAX_KT_EVENTS 64
#define CONFIG_MAX_ASYNC_EVENTS 32
#define CONFIG_MAX_ADRESS_SPACES 16
#define CONFIG_MAX_FPAGES 256
/* Only have 16 bits to set size of fpage */
#define CONFIG_FPAGE_MAX_SIZE 65535
/* One fpage is always mapped to vector table, so that leaves 7 fpages that we can use */
#define CONFIG_MAX_MAPPED_THREAD_FPAGES 7

//
// Kernel Timer
//
#define CONFIG_KTIMER_TICKLESS 1
// # CONFIG_KTIMER_TICKLESS_VERIFY is not set
#define CONFIG_KTIMER_TICKLESS_COMPENSATION 0
#define CONFIG_KTIMER_TICKLESS_INT_COMPENSATION 0
#define CONFIG_KTIMER_HEARTBEAT 65536
#define CONFIG_KTIMER_MINTICKS 128

//
// Flexible page tweaks
//
#define CONFIG_LARGEST_FPAGE_SHIFT 16
#define CONFIG_SMALLEST_FPAGE_SHIFT 8

//
// Thread tweaks
//
#define CONFIG_INTR_THREAD_MAX 256

//
// KIP tweaks
//
#define CONFIG_KIP_EXTRA_SIZE 128

//
// Kernel hacking
//
#define CONFIG_DEBUG 1
#define CONFIG_DEBUG_DEV_UART 1
#define CONFIG_KDB 1
#define CONFIG_KPROBES 1
#define CONFIG_SYMMAP 1
#define CONFIG_PANIC_DUMP_STACK 1
// CONFIG_LOADER is not set

//
// User IRQ
//
/* # CONFIG_WWDG_USER_IRQ is not set */
/* # CONFIG_PVD_USER_IRQ is not set */
/* # CONFIG_TAMP_STAMP_USER_IRQ is not set */
/* # CONFIG_RTC_WKUP_USER_IRQ is not set */
/* # CONFIG_FLASH_USER_IRQ is not set */
/* # CONFIG_RCC_USER_IRQ is not set */
/* # CONFIG_EXTI0_USER_IRQ is not set */
/* # CONFIG_EXTI1_USER_IRQ is not set */
/* # CONFIG_EXTI2_USER_IRQ is not set */
/* # CONFIG_EXTI3_USER_IRQ is not set */
/* # CONFIG_EXTI4_USER_IRQ is not set */
/* # CONFIG_DMA1_Stream0_USER_IRQ is not set */
/* # CONFIG_DMA1_Stream1_USER_IRQ is not set */
/* # CONFIG_DMA1_Stream2_USER_IRQ is not set */
/* # CONFIG_DMA1_Stream3_USER_IRQ is not set */
/* # CONFIG_DMA1_Stream4_USER_IRQ is not set */
/* # CONFIG_DMA1_Stream5_USER_IRQ is not set */
/* # CONFIG_DMA1_Stream6_USER_IRQ is not set */
/* # CONFIG_ADC_USER_IRQ is not set */
/* # CONFIG_CAN1_TX_USER_IRQ is not set */
/* # CONFIG_CAN1_RX_USER_IRQ is not set */
/* # CONFIG_CAN1_RX1_USER_IRQ is not set */
/* # CONFIG_CAN1_SCE_USER_IRQ is not set */
/* # CONFIG_EXTI9_5_USER_IRQ is not set */
/* # CONFIG_TIM1_BRK_TIM9_USER_IRQ is not set */
/* # CONFIG_TIM1_UP_TIM10_USER_IRQ is not set */
/* # CONFIG_TIM1_TRG_COM_TIM11_USER_IRQ is not set */
/* # CONFIG_TIM1_CC_USER_IRQ is not set */
/* # CONFIG_TIM2_USER_IRQ is not set */
/* # CONFIG_TIM3_USER_IRQ is not set */
/* # CONFIG_TIM4_USER_IRQ is not set */
/* # CONFIG_I2C1_EV_USER_IRQ is not set */
/* # CONFIG_I2C1_ER_USER_IRQ is not set */
/* # CONFIG_I2C2_EV_USER_IRQ is not set */
/* # CONFIG_I2C2_ER_USER_IRQ is not set */
/* # CONFIG_SPI1_USER_IRQ is not set */
/* # CONFIG_SPI2_USER_IRQ is not set */
/* # CONFIG_USART1_USER_IRQ is not set */
/* # CONFIG_USART2_USER_IRQ is not set */
/* # CONFIG_USART3_USER_IRQ is not set */
/* # CONFIG_EXTI15_10_USER_IRQ is not set */
/* # CONFIG_RTC_Alarm_USER_IRQ is not set */
/* # CONFIG_OTG_FS_WKUP_USER_IRQ is not set */
/* # CONFIG_TIM8_BRK_TIM12_USER_IRQ is not set */
/* # CONFIG_TIM8_UP_TIM13_USER_IRQ is not set */
/* # CONFIG_TIM8_TRG_COM_TIM14_USER_IRQ is not set */
/* # CONFIG_TIM8_CC_USER_IRQ is not set */
/* # CONFIG_DMA1_Stream7_USER_IRQ is not set */
/* # CONFIG_FSMC_USER_IRQ is not set */
/* # CONFIG_SDIO_USER_IRQ is not set */
/* # CONFIG_TIM5_USER_IRQ is not set */
/* # CONFIG_SPI3_USER_IRQ is not set */
/* # CONFIG_UART4_USER_IRQ is not set */
/* # CONFIG_UART5_USER_IRQ is not set */
/* # CONFIG_TIM6_DAC_USER_IRQ is not set */
/* # CONFIG_TIM7_USER_IRQ is not set */
/* # CONFIG_DMA2_Stream0_USER_IRQ is not set */
/* # CONFIG_DMA2_Stream1_USER_IRQ is not set */
/* # CONFIG_DMA2_Stream2_USER_IRQ is not set */
/* # CONFIG_DMA2_Stream3_USER_IRQ is not set */
/* # CONFIG_DMA2_Stream4_USER_IRQ is not set */
/* # CONFIG_ETH_USER_IRQ is not set */
/* # CONFIG_ETH_WKUP_USER_IRQ is not set */
/* # CONFIG_CAN2_TX_USER_IRQ is not set */
/* # CONFIG_CAN2_RX0_USER_IRQ is not set */
/* # CONFIG_CAN2_RX1_USER_IRQ is not set */
/* # CONFIG_CAN2_SCE_USER_IRQ is not set */
/* # CONFIG_OTG_FS_USER_IRQ is not set */
/* # CONFIG_DMA2_Stream5_USER_IRQ is not set */
/* # CONFIG_DMA2_Stream6_USER_IRQ is not set */
/* # CONFIG_DMA2_Stream7_USER_IRQ is not set */
/* # CONFIG_USART6_USER_IRQ is not set */
/* # CONFIG_I2C3_EV_USER_IRQ is not set */
/* # CONFIG_I2C3_ER_USER_IRQ is not set */
/* # CONFIG_OTG_HS_EP1_OUT_USER_IRQ is not set */
/* # CONFIG_OTG_HS_EP1_IN_USER_IRQ is not set */
/* # CONFIG_OTG_HS_WKUP_USER_IRQ is not set */
/* # CONFIG_OTG_HS_USER_IRQ is not set */
/* # CONFIG_DCMI_USER_IRQ is not set */
/* # CONFIG_CRYP_USER_IRQ is not set */
/* # CONFIG_HASH_RNG_USER_IRQ is not set */
/* # CONFIG_FPU_USER_IRQ is not set */

//
// User Space
//
#define CONFIG_BUILD_USER_APPS 1

//
// Apps
//
#define CONFIG_PINGPONG 1

//
// Test Cases
//
// # CONFIG_EXTI_INTERRUPT_TEST is not set
#define CONFIG_L4_TEST 1
// # CONFIG_LCD_TEST is not set
//
// ---------------------------------------------


// from xv6
#define NPROC        64  // maximum number of processes
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE       1000  // size of file system in blocks
#define MAXPATH      128   // maximum file path name

#endif
