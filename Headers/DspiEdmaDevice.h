#if !defined(__FSL_DSPI_EDMA_MASTER_DRIVER_H__)
#define __FSL_DSPI_EDMA_MASTER_DRIVER_H__

#include "T:\Workspaces\MSP430\TestCompilabuild\TestCompilabuild\Headers\fsl_dspi_hal.h"
#include "T:\Workspaces\MSP430\TestCompilabuild\TestCompilabuild\Headers\fsl_os_abstraction.h"
#include "T:\Workspaces\MSP430\TestCompilabuild\TestCompilabuild\Headers\fsl_edma_driver.h"
#include "T:\Workspaces\MSP430\TestCompilabuild\TestCompilabuild\Headers\fsl_dspi_edma_master_driver.h"

#if FSL_FEATURE_SOC_DSPI_COUNT

extern SPI_Type* const g_dspiBase[SPI_INSTANCE_COUNT];
extern const IRQn_Type g_dspiIrqId[SPI_INSTANCE_COUNT];

typedef struct DspiEdmaDevice {
    uint32_t bitsPerSec;
    dspi_data_format_config_t dataBusConfig;
} dspi_edma_device_t;

typedef struct DspiEdmaMasterState {
    dspi_ctar_selection_t whichCtar;
    uint32_t bitsPerFrame;
    dspi_which_pcs_config_t whichPcs;
    bool isChipSelectContinuous;
    uint32_t dspiSourceClock;
    volatile bool isTransferInProgress;
    volatile bool isTransferBlocking;
    semaphore_t irqSync;
    edma_chn_state_t dmaCmdData2Fifo;
    edma_chn_state_t dmaSrc2CmdData;
    edma_chn_state_t dmaFifo2Receive;
    edma_software_tcd_t* stcdSrc2CmdDataLast;
    bool extraByte;
    uint8_t* rxBuffer;
    uint32_t rxTransferByteCnt;
} dspi_edma_master_state_t;

typedef struct DspiEdmaMasterUserConfig {
    dspi_ctar_selection_t whichCtar;
    bool isSckContinuous;
    bool isChipSelectContinuous;
    dspi_which_pcs_config_t whichPcs;
    dspi_pcs_polarity_config_t pcsPolarity;
} dspi_edma_master_user_config_t;

#if defined(__cplusplus)
extern "C" {
#endif

    dspi_status_t DSPI_DRV_EdmaMasterInit(uint32_t instance,
        dspi_edma_master_state_t* dspiEdmaState,
        const dspi_edma_master_user_config_t* userConfig,
        edma_software_tcd_t* stcdSrc2CmdDataLast);

    dspi_status_t DSPI_DRV_EdmaMasterDeinit(uint32_t instance);

    dspi_status_t DSPI_DRV_EdmaMasterSetDelay(uint32_t instance, dspi_delay_type_t whichDelay,
        uint32_t delayInNanoSec, uint32_t* calculatedDelay);

    dspi_status_t DSPI_DRV_EdmaMasterConfigureBus(uint32_t instance,
        const dspi_edma_device_t* device,
        uint32_t* calculatedBaudRate);

    dspi_status_t DSPI_DRV_EdmaMasterTransferBlocking(uint32_t instance,
        const dspi_edma_device_t* device,
        const uint8_t* sendBuffer,
        uint8_t* receiveBuffer,
        size_t transferByteCount,
        uint32_t timeout);

    dspi_status_t DSPI_DRV_EdmaMasterTransfer(uint32_t instance,
        const dspi_edma_device_t* device,
        const uint8_t* sendBuffer,
        uint8_t* receiveBuffer,
        size_t transferByteCount);

    dspi_status_t DSPI_DRV_EdmaMasterGetTransferStatus(uint32_t instance, uint32_t* framesTransferred);

#if defined(__cplusplus)
}
#endif

#endif /* FSL_FEATURE_SOC_DSPI_COUNT */
#endif /* __FSL_DSPI_EDMA_MASTER_DRIVER_H__ */
