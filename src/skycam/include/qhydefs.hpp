/*
 QHY camera capture tool
 Based on the original QHYCCD SDK and lin_guider source code

 Copyright (c) 2014 QHYCCD
 Copyright (c) gm
 Copyright 2016  Oleg Kutkov <kutkov.o@yandex.ru>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.  
*/

#ifndef QHYDEFS_HPP
#define QHYDEFS_HPP

///
#define QHYCCD_REQUEST_READ  0xC0
#define QHYCCD_REQUEST_WRITE 0x40

#define DEVICETYPE_QHY5II  51
#define DEVICETYPE_QHY5LII 56
#define DEVICETYPE_QHY5RII 58
#define DEVICETYPE_UNKOWN 0

#define QHYCCD_INTERRUPT_READ_ENDPOINT 0x81
#define QHYCCD_INTERRUPT_WRITE_ENDPOINT 0x01
#define QHYCCD_DATA_READ_ENDPOINT 0x82
///

enum DEVICE_TYPE
{
	DEVICE_UNKNOWN_DEVICE = 1,
	DEVICE_QHY5,
	DEVICE_QHY6,
	DEVICE_QHY5II
};

enum DEVICE_PARAMS
{
    CONTROL_BRIGHTNESS = 0, /* image brightness */
    CONTROL_CONTRAST,       /* image contrast */
    CONTROL_WBR,            /* red of white balance */
    CONTROL_WBB,            /* blue of white balance */
    CONTROL_WBG,            /* the green of white balance */
    CONTROL_GAMMA,          /* screen gamma */
    CONTROL_GAIN,           /* camera gain */
    CONTROL_OFFSET,         /* camera offset */
    CONTROL_EXPOSURE,       /* expose time */
	CONTROL_RESOLUTION,     /* image resolution */
    CONTROL_SPEED,          /* transfer speed */
    CONTROL_TRANSFERBIT,    /* image depth bits */
    CONTROL_CHANNELS,       /* image channels */
    CONTROL_USBTRAFFIC,     /* hblank */
    CONTROL_ROWNOISERE,     /* row denoise */
    CONTROL_CURTEMP,        /* current cmos or ccd temprature */
    CONTROL_CURPWM,         /* current cool pwm */
    CONTROL_MANULPWM,       /* set the cool pwm */
    CONTROL_CFWPORT,        /* control camera color filter wheel port */
    CONTROL_COOLER,
    CONTROL_ST4PORT,
    CAM_BIN1X1MODE,         /* check if camera has bin1x1 mode */
    CAM_BIN2X2MODE,         /* check if camera has bin2x2 mode */
    CAM_BIN3X3MODE,         /* check if camera has bin3x3 mode */
    CAM_BIN4X4MODE          /* check if camera has bin4x4 mode */
};


typedef struct param_val
{
	param_val(double v1 = 0, double v2 = 0)
	{
		values[0] = v1;
		values[1] = v2;
	}

	void set(double v1 = 0, double v2 = 0)
	{
		values[0] = v1;
		values[1] = v2;
	}

	double values[2];
} param_val_t;

///
typedef struct ccdreg
{
    unsigned char Gain;                //!< ccd gain
    unsigned char Offset;              //!< ccd offset
    unsigned long Exptime;             //!< expose time
    unsigned char HBIN;                //!< width bin
    unsigned char VBIN;                //!< height bin
    unsigned short LineSize;           //!< almost match image width
    unsigned short VerticalSize;       //!< almost match image height
    unsigned short SKIP_TOP;           //!< Reserved
    unsigned short SKIP_BOTTOM;        //!< Reserved
    unsigned short LiveVideo_BeginLine;//!< Reserved
    unsigned short AnitInterlace;      //!< Reserved
    unsigned char MultiFieldBIN;       //!< Reserved
    unsigned char AMPVOLTAGE;          //!< Reserved
    unsigned char DownloadSpeed;       //!< transfer speed
    unsigned char TgateMode;           //!< Reserved
    unsigned char ShortExposure;       //!< Reserved
    unsigned char VSUB;                //!< Reserved
    unsigned char CLAMP;               //!< Reserved
    unsigned char TransferBIT;         //!< Reserved
    unsigned char TopSkipNull;         //!< Reserved
    unsigned short TopSkipPix;         //!< Reserved
    unsigned char MechanicalShutterMode;//!< Reserved
    unsigned char DownloadCloseTEC;    //!< Reserved
    unsigned char SDRAM_MAXSIZE;       //!< Reserved
    unsigned short ClockADJ;           //!< Reserved
    unsigned char Trig;                //!< Reserved
    unsigned char MotorHeating;        //!< Reserved
    unsigned char WindowHeater;        //!< Reserved
    unsigned char ADCSEL;              //!< Reserved
} CCDREG;

#endif

