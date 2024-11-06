/* Gage_Ctrl.DLL  Header*******************************************/
/* Gage_Ctrl Library Inteface Header                               */
/* For ACCURETECH Digital USB-Gage                                 */
/* Ver 0.7.0                                                       */
/*                                             ToseiEng.Co.,Ltd.   */
/*******************************************************************/
/** Environment Configration ***************************************/ 
#ifndef _GAGE_CTRL_H
#define _GAGE_CTRL_H 

#ifdef  _MAKE_GAGE_CTRL_DLL_
	#define  __PORT  __declspec(dllexport)   /*  Make DLL  */
#else
	#define  __PORT  __declspec(dllimport)   /*  Use DLL */
#endif
#ifdef __cplusplus
extern "C"{
#endif
/*Software & Firmware Version Consist 8 Byte(char)*/
#define VERSIONCHAR_SIZE                        (8)
/** Return Code Definition *****************************************/
#define IGS_ERR_SUCCESS                         (0)
/** Init/Fin *******************************************************/
#define IGS_ERR_PARAM                          (-1)
#define IGS_ERR_DEVICE                          (1)
#define IGS_ERR_FILE_VERSION                    (2)
#define IGS_ERR_FIRM_VERSION                    (3)
#define IGS_ERR_CREATE_THREAD                   (4)
#define IGS_ERR_GAGE_COUNT                      (5)
/** Setting ********************************************************/
#define IGS_ERR_NOT_INIT                     (1000)
#define IGS_ERR_INVALID_GAGENUMBER           (1001)
#define IGS_ERR_NO_NAME                      (1002)
#define IGS_ERR_NO_ID                        (1003)
#define IGS_ERR_DISCONNECTED                 (1004)
#define IGS_ERR_INVALID_BUFF                 (1005)
#define IGS_ERR_INVALID_VALUE                (1006)
#define IGS_ERR_INVALID_TYPE                 (1007)

#define IGS_ERR_NO_GAGE                      (2000)
#define IGS_ERR_GAGENAME                     (2001)
#define IGS_ERR_INVALID_GAGENAME             (2002)
/** Gage Err *******************************************************/
#define IGS_ERR_SPEED                        (9001)
#define IGS_ERR_LEVEL                        (9002)
#define IGS_ERR_SYSTEM                       (9003)
#define IGS_ERR_SNAP                         (9004)

/** MEASURED TYPE  **********************************************/
#define GAGE_MEASUREDDATA                    (0x00)
#define GAGE_RAWDATA                         (0x01)
#define GAGE_SMOOTHING_DATA                  (0x80)
/** MAX SIZE DEFINITION  ****************************************/
#define GAGE_MAX_NAMESIZE                      (16)
#define GAGE_MAX_COUNT                        (127)

/** GAGE TYPE ***************************************************/
#define GAGE_TYPE_LVDT                       (0x10)
#define GAGE_TYPE_ATC                        (0x20)
#define GAGE_TYPE_PHA                        (0x30)
#define GAGE_TYPE_AE                         (0x40)
/** Gage Info Structure ******************************************************/
#pragma pack(4)
typedef struct Gage_Info_Public{
	BYTE   GageNo     ;
	BYTE   GageType   ;
	INT    GageID     ;
	INT    GagePol    ;
	double GageGain   ; 
	double GageOffset ;
	INT   FullRange;
    BYTE  GageName[GAGE_MAX_NAMESIZE] ;
}GAGE_LIST;
#pragma
#pragma pack(4)
typedef struct Gage_Product_Info{
	BYTE  GageNo      ;
	INT   GageModel   ;
	INT   GageSerial  ;
	INT   GageSubinfo ;
	BYTE  GageLibInfo[VERSIONCHAR_SIZE];
	BYTE  GageFirmInfo[VERSIONCHAR_SIZE];
}GAGE_PRODUCT_INFO;
#pragma
/* Function ******************************************************************/
__PORT INT  __stdcall IGS_SystemStatus_Set     (LPSTR SystemStatus ) ;
__PORT INT  __stdcall IGS_Gage_Connect         (SHORT *GageCount, GAGE_LIST GageList[]);
__PORT INT  __stdcall IGS_Gage_DisConnect      (void);
__PORT INT  __stdcall IGS_Get_ProductInfo      (SHORT *GageCount, GAGE_PRODUCT_INFO GageProductInfoList[]);
__PORT INT  __stdcall IGS_Set_GageName         (INT GageID ,LPSTR GageName) ;
__PORT INT  __stdcall IGS_Get_GageID           (LPSTR GageName, INT *GageID);
__PORT INT  __stdcall IGS_Get_GageNo           (LPSTR GageName, INT *GageNo);
__PORT INT  __stdcall IGS_Get_MeasuredValue    (INT GageID ,BYTE MeasuredType, double *data) ;
__PORT INT  __stdcall IGS_Set_GageGain         (INT GageID, double GageGain) ;
__PORT INT  __stdcall IGS_Get_GageGain         (INT GageID, double *GageGain);
__PORT INT  __stdcall IGS_Set_GageOffset       (INT GageID, double GageOffset);
__PORT INT  __stdcall IGS_Get_GageOffset       (INT GageID, double *GageOffset);
__PORT INT  __stdcall IGS_Set_GagePolarity     (INT GageID, INT GagePolarity);
__PORT INT  __stdcall IGS_Get_GagePolarity     (INT GageID, INT *GagePolarity);
__PORT INT  __stdcall IGS_Set_GageFilter       (INT GageID, BYTE Degree, double Cutoff);
__PORT INT  __stdcall IGS_Get_GageFilter       (INT GageID, BYTE *Degree, double *Cutoff);
__PORT INT  __stdcall IGS_Set_GageZero         (INT GageID, double GageZero);
__PORT INT  __stdcall IGS_Set_GageCAL          (INT GageID, double Desired, double GageCAL);
__PORT INT  __stdcall IGS_AutoSet_GageZero     (INT GageID);
__PORT INT  __stdcall IGS_AutoSet_GageCAL      (INT GageID, double Desired);
__PORT INT  __stdcall IGS_Set_Led              (INT GageID, BYTE LedBlueStatus ,BYTE LedRedStatus) ;
__PORT INT  __stdcall IGS_Recall_LedStatus     (INT GageID) ;
__PORT INT  __stdcall IGS_Gage_Reset           (INT GageID) ;
#ifdef __cplusplus
}
#endif

#endif
