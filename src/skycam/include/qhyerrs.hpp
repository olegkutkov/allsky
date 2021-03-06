/*
 QHY camera capture tool
 Based on the original QHYCCD SDK

 Copyright (c) 2014 QHYCCD
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

#ifndef QHYERRS_HPP
#define QHYERRS_HPP

/**
 * It means the camera is color one */
#define QHYCCD_COLOR                    4

/**
 * It means the camera is mono one*/
#define QHYCCD_MONO                     3

/**
 * It means the camera has cool function */
#define QHYCCD_COOL                     2

/**
 * It means the camera do not have cool function */
#define QHYCCD_NOTCOOL                  1

/**
 * camera works well */
#define QHYCCD_SUCCESS                  0

/**
 * Other error */
#define QHYCCD_ERROR                   -1

/**
 * There is no camera connected */
#define QHYCCD_ERROR_NO_DEVICE         -2

/**
 * Do not support the function */
#define QHYCCD_ERROR_NOTSUPPORT        -3

/**
 * Set camera params error */
#define QHYCCD_ERROR_SETPARAMS         -4

/**
 * Get camera params error */
#define QHYCCD_ERROR_GETPARAMS         -5

/**
 * The camera is exposing now */
#define QHYCCD_ERROR_EXPOSING          -6

/**
 * The camera expose failed */
#define QHYCCD_ERROR_EXPFAILED         -7

/**
 * There is another instance is getting data from camera */
#define QHYCCD_ERROR_GETTINGDATA       -8

/**
 * Get data from camera failed */
#define QHYCCD_ERROR_GETTINGFAILED     -9

/**
 * Init camera failed */
#define QHYCCD_ERROR_INITCAMERA        -10

/**
 * Release SDK resouce failed */
#define QHYCCD_ERROR_RELEASERESOURCE   -11

/**
 * Init SDK resouce failed */
#define QHYCCD_ERROR_INITRESOURCE      -12

/**
 * There is no match camera */
#define QHYCCD_ERROR_INDEX             -13

/**
 * Open cam failed */
#define QHYCCD_ERROR_OPENCAM           -14

/**
 * Init cam class failed */
#define QHYCCD_ERROR_INITCLASS         -15

/**
 * Set Resolution failed */
#define QHYCCD_ERROR_RESOLUTION        -16

/**
 * Set usbtraffic failed */
#define QHYCCD_ERROR_USBTRAFFIC        -17

/**
 * Set usb speed failed */
#define QHYCCD_ERROR_SETSPEED          -18

/**
 * Set expose time failed */
#define QHYCCD_ERROR_SETEXPOSE         -19

/**
 * Set cam gain failed */
#define QHYCCD_ERROR_SETGAIN           -20

/**
 * Set cam white balance red failed */
#define QHYCCD_ERROR_SETRED            -21

/**
 * Set cam white balance blue failed */
#define QHYCCD_ERROR_SETBLUE           -22


/**
 * Set cam white balance blue failed */
#define QHYCCD_ERROR_EVTCMOS           -23


/**
 * Set cam white balance blue failed */
#define QHYCCD_ERROR_EVTUSB            -24

/**
 * Set cam white balance blue failed */
#define QHYCCD_ERROR_BINMODE           -25

#endif

