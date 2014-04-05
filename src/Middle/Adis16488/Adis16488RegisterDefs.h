/*
 * Adis16488RegisterDefs.h
 *
 *  Created on: 2014/04/05
 *      Author: sa
 */

#ifndef ADIS16488REGISTERDEFS_H_
#define ADIS16488REGISTERDEFS_H_

#define PAGE_ID    		0x00
#define SEQ_CNT    		0x06
#define SYS_E_FLAG    	0x08
#define DIAG_STS    	0x0A
#define ALM_STS    		0x0C
#define TEMP_OUT    	0x0E
#define X_GYRO_LOW    	0x10
#define X_GYRO_OUT    	0x12
#define Y_GYRO_LOW    	0x14
#define Y_GYRO_OUT    	0x16
#define Z_GYRO_LOW    	0x18
#define Z_GYRO_OUT    	0x1A
#define X_ACCL_LOW    	0x1C
#define X_ACCL_OUT    	0x1E
#define Y_ACCL_LOW    	0x20
#define Y_ACCL_OUT    	0x22
#define Z_ACCL_LOW    	0x24
#define Z_ACCL_OUT    	0x26
#define X_MAGN_OUT    	0x28
#define Y_MAGN_OUT    	0x2A
#define Z_MAGN_OUT    	0x2C
#define BAROM_LOW    	0x2E
#define BAROM_OUT    	0x30
#define X_DELTANG_LOW   0x40
#define X_DELTANG_OUT   0x42
#define Y_DELTANG_LOW   0x44
#define Y_DELTANG_OUT   0x46
#define Z_DELTANG_LOW   0x48
#define Z_DELTANG_OUT   0x4A
#define X_DELTVEL_LOW   0x4C
#define X_DELTVEL_OUT   0x4E
#define Y_DELTVEL_LOW   0x50
#define Y_DELTVEL_OUT   0x52
#define Z_DELTVEL_LOW   0x54
#define Z_DELTVEL_OUT   0x56
#define TIME_MS_OUT    	0x78
#define TIME_DH_OUT    	0x7A
#define TIME_YM_OUT    	0x7C
#define PROD_ID    		0x7E
#define X_GYRO_SCALE    0x04
#define Y_GYRO_SCALE    0x06
#define Z_GYRO_SCALE    0x08
#define X_ACCL_SCALE    0x0A
#define Y_ACCL_SCALE    0x0C
#define Z_ACCL_SCALE    0x0E
#define XG_BIAS_LOW    	0x10
#define XG_BIAS_HIGH    0x12
#define YG_BIAS_LOW    	0x14
#define YG_BIAS_HIGH    0x16
#define ZG_BIAS_LOW    	0x18
#define ZG_BIAS_HIGH    0x1A
#define XA_BIAS_LOW    	0x1C
#define XA_BIAS_HIGH    0x1E
#define YA_BIAS_LOW    	0x20
#define YA_BIAS_HIGH    0x22
#define ZA_BIAS_LOW    	0x24
#define ZA_BIAS_HIGH    0x26
#define HARD_IRON_X    	0x28
#define HARD_IRON_Y    	0x2A
#define HARD_IRON_Z    	0x2C
#define SOFT_IRON_S11   0x2E
#define SOFT_IRON_S12   0x30
#define SOFT_IRON_S13   0x32
#define SOFT_IRON_S21   0x34
#define SOFT_IRON_S22   0x36
#define SOFT_IRON_S23   0x38
#define SOFT_IRON_S31   0x3A
#define SOFT_IRON_S32   0x3C
#define SOFT_IRON_S33   0x3E
#define BR_BIAS_LOW    	0x40
#define BR_BIAS_HIGH    0x42
#define USER_SCR_1    	0x74
#define USER_SCR_2    	0x76
#define USER_SCR_3    	0x78
#define USER_SCR_4    	0x7A
#define FLSHCNT_LOW    	0x7C
#define FLSHCNT_HIGH    0x7E
#define GLOB_CMD    	0x02
#define FNCTIO_CTRL    	0x06
#define GPIO_CTRL    	0x08
#define CONFIG    		0x0A
#define DEC_RATE    	0x0C
#define NULL_CNFG    	0x0E
#define SLP_CNT    		0x10
#define FILTR_BNK_0    	0x16
#define FILTR_BNK_1    	0x18
#define ALM_CNFG_0    	0x20
#define ALM_CNFG_1    	0x22
#define ALM_CNFG_2    	0x24
#define XG_ALM_MAGN    	0x28
#define YG_ALM_MAGN    	0x2A
#define ZG_ALM_MAGN    	0x2C
#define XA_ALM_MAGN    	0x2E
#define YA_ALM_MAGN    	0x30
#define ZA_ALM_MAGN    	0x32
#define XM_ALM_MAGN    	0x34
#define YM_ALM_MAGN    	0x36
#define ZM_ALM_MAGN    	0x38
#define BR_ALM_MAGN    	0x3A
#define FIRM_REV    	0x78
#define FIRM_DM    		0x7A
#define FIRM_Y    		0x7C
#define SERIAL_NUM    	0x20


#endif /* ADIS16488REGISTERDEFS_H_ */
