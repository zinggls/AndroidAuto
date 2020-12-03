#ifndef __ZING_HW_H__
#define __ZING_HW_H__

//I2C
#define I2C_DeviceAddress				(0x08)

//etc
#define ZING_HW_TIMEOUT     5000                /* 5000 ms */
#define AFC_N	64

//Zing Header
#define ZING_HDR_SIZE               8       /* 8 bytes */
#define ZING_HDR_DIR_EGRESS         0       /* Direction SW -> HW */
#define ZING_HDR_DIR_INGRESS        1       /* Direction HW -> SW */
#define ZING_HDR_TARGET_BUFFER      0
#define ZING_HDR_TARGET_REG         1
#define ZING_HDR_ACTION_WRITE       1
#define ZING_HDR_ACTION_READ        0

typedef struct zing_header
{
    /* Word 0 [31:0] */
    unsigned int dir        : 1;    /* 0 (header direction) */
    unsigned int interrupt  : 1;    /* 1 (interrupt ) */
    unsigned int target     : 1;    /* 2 (header command target) */
    unsigned int type       : 1;    /* 3 action type */
    unsigned int req_resp   : 1;    /* 4 request processing result */
    unsigned int rsvd0      : 3;    /* 7:5 reserved */
    unsigned int fr_type    : 2;    /* 9:8 frame type 0:beacon, 1:command, 2:data, 3:vendor */
    unsigned int rsvd1      : 6;    /* 15:10 reserved */
    unsigned int intr_flags : 16;   /* 31:16 interrupt flags */

    /* Word 1 [63:32] */
    unsigned int addr       : 16;   /* 47:32 Address in word */
    unsigned int length     : 16;   /* 63:48 Data length in bytes */
} ZingHdr_t;

//Register
#define REG_HW_CFG              0x8000          /* Enable, software reset */
#define REG_IFS                 0x8001
#define REG_SUPERFRAME_CFG      0x8002
#define REG_PPID                0x8003
#define REG_DEVICE_ID           0x8004
#define REG_PLL_CTRL_100        0x8005          /* ASIC only, 100MHz PLL */
#define REG_PLL_CTRL_110        0x8007          /* ASIC only, 110MHz PLL */
#define REG_PHY_CTRL            0x8006
#define REG_PLL_CTRL_SERDES     0x8008          /* PLL in SERDES, both FPGA and ASIC */
#define REG_SERDES_TEST_PATTERN 0x8009
#define REG_MAC_TIMEOUT_CFG     0x800a
#define REG_PHY_TIMEOUT_CFG     0x800b
#define REG_MAC_RETX_LIMIT      0x800c
#define REG_SERDES_TEST_CONFIG  0x800e
#define REG_RTL_VERSION         0x801f
#define REG_RF_RX_CONTROL_0     0x8024
#define REG_RF_RX_CONTROL_1     0x8025
#define REG_RF_TX_CONTROL_0     0x8026
#define REG_RF_CONTROL_0        0x8027
#define REG_RF_CONTROL_1        0x8028
#define REG_PLL_CONTROL_MONITOR 0x802a
#define REG_SERDES_PLL_AFC      0x802b
#define REG_SERDES_TRIM_1       0x802c
#define REG_SERDES_TRIM_2       0x802d
#define REG_SERDES_TRIM_3       0x802e
#define REG_SERDES_TRIM_4       0x802f
#define REGISTER_START_ADDR		0x8000
#define REGISTER_END_ADDR		0x83FF

//Register bit field & init value
#define PPC		(1)
#define DEV		(0)

#define REG_CFG_B(txen, rxen, ppc_mode, associated, sf, channel, golay, agg_no, max_len, msdu_only, boundary_1k ,gpif16, phy_init_n, init_n) \
                                    ((txen&0x1) | ((rxen&0x1) << 1) | ((ppc_mode&0x1) << 4) | \
                                    ((associated&0x1) << 8) | ((sf&0x1) << 12) | ((channel&0x1) << 13) |  ((golay&0x1) << 14) | \
                                    ((agg_no&0xF) << 16) | ((max_len&0x1) << 20) |  ((msdu_only&0x1) << 24) | ((boundary_1k&0x1) << 25) | \
                                    ((gpif16&0x1) << 26) | ((phy_init_n&0x1) << 30) | ((init_n&0x1) << 31) )
#define REG_HW_CFG_INIT_STAGE0	REG_CFG_B(1,1,1, 0,0,0,0, 1,0,0, 0,0,0,0) /* ppc, max_aggregation 1 */
#define REG_HW_CFG_INIT_STAGE1	REG_CFG_B(1,1,1, 0,0,0,0, 1,1,1, 1,0,1,1) /* MSDU max length 8K, MSDU only mode, 1k boundary */
#define REG_HW_CFG_INIT_STAGE2	REG_CFG_B(1,1,1, 1,0,1,0, 4,1,1, 1,0,1,1)  /* associated(ACK), bonding 2CH, max_aggregation 4 */

#define REG_IFS_B(sifs,rifs)    				((sifs&0xFF) | ((rifs&0xFF) <<8))
#define REG_IFS_PPC_INIT            			REG_IFS_B(25, 94)
#define REG_IFS_DEV_INIT            			REG_IFS_B(25, 230)

#define REG_SUPERFRAME_B(duration, access_time)	((duration&0xFFFF) | ((access_time&0xFFFF) <<16))
/* It is recommended that an HRCP DEV should use short ATP length value less than or equal to 500 ms. */
#define ZING_ATP_TIMEOUT						200	/* 200 ms temporal */
#define ZING_SUPERFRAME_DURATION				50000	/* 5 ms (0.1us resolution) */
#define REG_SUPERFRAME_INIT						REG_SUPERFRAME_B(ZING_SUPERFRAME_DURATION, 500)
#define REG_PPID_INIT							0xABCD
#define REG_PHY_CONTROL_INIT					0xD6010
#define REG_PLL_SERDES_INIT2					0x20013	/* 0x10 : RF I2C block clock gating off, 0x03 : Serdes Tx Amplify */
#define REG_DEVID_B(ppcid,devid)				((ppcid&0xFF) | ((devid&0xFF) << 8) )
#define REG_DEVID_INIT							REG_DEVID_B(0x0, 0x35)
#define REG_MAC_TIMEOUT_B(link, command)		((link&0xFFFF) | ((command&0xFFFF) <<16))
#define REG_MAC_TIMEOUT_INIT					REG_MAC_TIMEOUT_B(10000, 5000)    /* 1ms, 500us 0.1us resolution */
#define REG_PHY_TIMEOUT_INIT					2000    /* 200us */
#define REG_RETRANSMIT_LIMIT_INIT				1000

//Register Structure
typedef struct {
	unsigned int tx_en        		: 1; // 0
	unsigned int rx_en        		: 1; // 1
	unsigned int rsvd0        		: 2; //# 2:3
	unsigned int ppc_mode       	: 1; // 4
	unsigned int rsvd1        		: 3; //# 5:7
	unsigned int associated       	: 1; // 8
	unsigned int rsvd2        		: 3; //# 9:11
	unsigned int spreading_factor   : 1; // 12
	unsigned int bonding_ch         : 1; // 13
	unsigned int golaycode_en       : 1; // 14
	unsigned int rsvd3        		: 1; //# 15
	unsigned int max_aggregation    : 4; // 16:19
	unsigned int msdu_max_len       : 1; // 20
	unsigned int rsvd4        		: 3; //# 21:23
	unsigned int msdu_only_mode     : 1; // 24
	unsigned int boundary_1k_en     : 1; // 25
	unsigned int gpif_ctl        	: 2; // 26:27
	unsigned int rsvd5        		: 2; //# 28:29
	unsigned int phy_init_n         : 1; // 30
	unsigned int init_n        	 	: 1; // 31
} HW_CFG;

typedef struct REG_Block_0
{
    /* Configuration Registers */
	HW_CFG hw_cfg;            /* 0x00, H/W Configuration */
    unsigned int ifs_cfg;           /* 0x01, IFS configuration */
    unsigned int superframe_cfg;    /* 0x02, Super frame configuration */
    unsigned int ppid;
    unsigned int device_id;         /* 0x04 */
    unsigned int pll_ctrl_100;      /* 0x05 */
    unsigned int phy_control;       /* 0x06 */
    unsigned int pll_ctrl_110;      /* 0x07 */
    unsigned int pll_ctrl_serdes;   /* 0x08 */
    unsigned int rsvd0;             /* 0x09 */
    unsigned int mac_timeout;       /* 0x0a */
    unsigned int phy_timeout;       /* 0x0b */
    unsigned int mac_limit_cfg;     /* 0x0c */
    unsigned int rsvd1[3];          /* 0x0d ~ 0x0f */

    /* Status Registers */
    unsigned int phy_intf_status;   /* 0x10 */
    unsigned int mac_ctrl_status;   /* 0x11 */
    unsigned int mac_data_status;   /* 0x12 */
    unsigned int mac_bd_status;     /* 0x13 */
    unsigned int phy_status;        /* 0x14 */
    unsigned int phy_error_cnt;     /* 0x15 */
    unsigned int mac_tx_frame_cnt;  /* 0x16 */
    unsigned int mac_tx_subfr_cnt;  /* 0x17 */
    unsigned int mac_rx_frame_cnt;  /* 0x18 */
    unsigned int mac_rx_subfr_cnt;  /* 0x19 */
    unsigned int mac_seq_status;    /* 0x1a */
    unsigned int mac_retx_cnt;      /* 0x1b */
    unsigned int phy_tx_cnt;        /* 0x1c */
    unsigned int phy_rx_cnt;        /* 0x1d */
    unsigned int gpif_count;        /* 0x1e */
    unsigned int rtl_version;       /* 0x1f, RTL Revision Number */
} REG_B0_t;

typedef struct REG_Block_1
{
    unsigned int gpio0_out_en;      /* 0x20 */
    unsigned int gpio0_out;
    unsigned int gpio0_in;
    unsigned int rsvd0;
    unsigned int gpio1_out_en;      /* 0x24 */
    unsigned int gpio1_out;
    unsigned int gpio1_in;
    unsigned int rsvd1;
    unsigned int I2C_reg_in_0;      /* 0x28 */
    unsigned int I2C_reg_in_1;
    unsigned int I2C_reg_in_2;
    unsigned int I2C_reg_in_3;
    unsigned int rsvd2[4];          /* 0x2C ~ 0x2F */

    /* MAC error count */
    unsigned int phy_deadlock;      /* 0x30 */
    unsigned int tx_underrun;       /* 0x31 */
    unsigned int abnormal_tx_end;   /* 0x32 */
    unsigned int abnormal_rx_end;   /* 0x33 */

    unsigned int phy_rx_header_err; /* 0x34 */
    unsigned int rxbd_invalid;      /* 0x35 */
    unsigned int proto_ver_err;     /* 0x36 */
    unsigned int unknown_fr_type;   /* 0x37 */

    unsigned int ppid_error;        /* 0x38 */
    unsigned int destid_error;
    unsigned int srcid_error;
    unsigned int subheader_error;

    unsigned int fcs_error;         /* 0x3C */
    unsigned int duplicated_frame;
    unsigned int phy_rx_error;
    unsigned int rsvd3;
} REG_B1_t;

/* Tx/Rx Buffer Descriptor */
typedef struct BD
{
    /* Word 0 */
    unsigned int tbd0       : 6;    /* [5:0] */     /* Reserved */
    unsigned int bdtype     : 2;    /* [7:6] */     /* 0:Data TxBD, 1: RxBD, 2:Command TxBD */
    unsigned int bdnum      : 3;    /* [10:8] */    /* Bd number */
    unsigned int frtype     : 2;    /* [12:11] */   /* 0:beacon, 1:command, 2:data, 3:vendor */
    unsigned int wrap       : 1;    /* [13] */      /* Control */
    unsigned int tbd1       : 1;    /* [14] */      /* Reserved */
    unsigned int bdready    : 1;    /* [15] */      /* ownership? */
    unsigned int len        : 16;   /* [31:16] */   /* MSDU length */

    /* Word 1 */
    unsigned int pointer    : 16;   /* [15:0] */    /* in words (addressable unit=32bit) */
    unsigned int tbd2       : 16;   /* [31:16] */
} BD_t;

typedef struct BD_Block_0
{
    BD_t    txbd[8];
    BD_t    rxbd[8];
} BD_B0_t;

typedef struct BD_Block_1
{
    BD_t    mgmtbd[2];
    unsigned int rsvd[28];
} BD_B1_t;

typedef struct REG_Resp_Block
{
    ZingHdr_t   hdr;
    union
    {
        REG_B0_t block0; // addr : 0x8000 ~ 0x801F , REG 0x000 ~ REG 0x01F
        REG_B1_t block1; // addr : 0x8020 ~ 0x803F , REG 0x020 ~ REG 0x03F
        BD_B0_t  bd0; // addr : 0x8400 ~ 0x841F , Data BD(Buffer Descriptor)
        BD_B1_t  bd1; // addr : 0x8420 ~ 0x8423 , Management BD(Buffer Descriptor)
    } regs;
} REG_Resp_t;

#endif
