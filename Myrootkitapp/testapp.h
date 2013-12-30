#ifndef _TESTAPP_H
#define _TESTAPP_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DRIVER_FUNC_INSTALL     0x01
#define DRIVER_FUNC_REMOVE      0x02

#define MAXTHREADS 10
#define DRIVER_NAME       "Myrootkit"

BOOLEAN
ManageDriver(
    _In_ LPCTSTR  DriverName,
    _In_ LPCTSTR  ServiceName,
    _In_ USHORT   Function
    );

#endif