#ifndef __OMAP4_DEBUG_H
#define __OMAP4_DEBUG_H

#define REG_DEBUG 1
#define REG_VERBOSE 1

// !!!! CHECK CFG_HZ=150000, V_SCLK=38400000, CFG_PVT=7, (2 << CFG_PVT)=256

void dump_gpio(unsigned int base);
int dump_clock_control(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int dump_dss(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#if REG_VERBOSE
int dump_musb(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int dump_control_general_core(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int dump_control_general_wkup(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int dump_control_padconf_core(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int dump_control_padconf_wkup(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
void dump_mcspi(unsigned int base);
int dump_i2c_twl6030(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int dump_dpll(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#endif
int dump_prcm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
void dump_gptimer_1_2_10(unsigned int base);
void dump_gptimer_3_4_5_6_7_8_9_11(unsigned int base);

#endif
