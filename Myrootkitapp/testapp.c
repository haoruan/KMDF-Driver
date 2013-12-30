#include "testapp.h"

#include <strsafe.h>

#define THREAD_NUM 2

HANDLE hDevice;
HANDLE hThreads[THREAD_NUM];
DWORD WINAPI Reader(LPVOID lpThreadParameter);
DWORD WINAPI Writer(LPVOID lpThreadParameter);
VOID Cleanup();
PTHREAD_START_ROUTINE TSR[THREAD_NUM] = {Reader, Writer};

BOOLEAN
SetupDriverName(
    _Inout_updates_all_(BufferLength) PCHAR DriverLocation,
    _In_ ULONG BufferLength
    );

int main(int argc, char * argv[])
{
	ULONG i, Id;
	DWORD errNum = 0;
	TCHAR driverLocation[MAX_PATH] = {'\0'};

	if ((hDevice = CreateFile("\\\\.\\MyDevice",
                                 GENERIC_READ | GENERIC_WRITE,
                                 0,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL
                                 )) == INVALID_HANDLE_VALUE) {

        errNum = GetLastError();

        if (errNum != ERROR_FILE_NOT_FOUND) {

            printf("CreateFile failed!  Error = %d\n", errNum);

            return EXIT_FAILURE;
        }
		//
        // Setup full path to driver name.
        //

        if (!SetupDriverName(driverLocation, sizeof(driverLocation))) {

            return EXIT_FAILURE;
        }


        //
        // Install driver.
        //

        if (!ManageDriver(DRIVER_NAME,
                          driverLocation,
                          DRIVER_FUNC_INSTALL
                          )) {

            printf("Unable to install driver. \n");

            //
            // Error - remove driver.
            //

            Cleanup(driverLocation);
            return EXIT_FAILURE;
        }
        //
        // Try to open the newly installed driver.
        //

        hDevice = CreateFile( "\\\\.\\MyDevice",
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);

        if ( hDevice == INVALID_HANDLE_VALUE ){
            printf ( "Error: CreatFile Failed : %d\n", GetLastError());
			Cleanup(driverLocation);
            return EXIT_FAILURE;
        }
    }
	for(i=0; i < THREAD_NUM; i++) {
		hThreads[i] = CreateThread( NULL,      // security attributes
										0,         // initial stack size
										TSR[i],    // Main() function
										NULL,      // arg to Reader thread
										0,         // creation flags
										(LPDWORD)&Id); // returned thread id

			if ( NULL == hThreads[0] ) {
				printf( " Error CreateThread[%d] Failed: %d\n", i, GetLastError());
				Cleanup(driverLocation);
				return EXIT_FAILURE;
			}
	}

	printf("Enter 'q' to exit gracefully:");
	if (getchar() == 'q')
    {

        WaitForMultipleObjects( THREAD_NUM, hThreads, TRUE, INFINITE);

        for(i=0; i < THREAD_NUM; i++) {
            CloseHandle(hThreads[i]);
		}

    }

	Cleanup(driverLocation);

	system("pause");
	return EXIT_SUCCESS;
}

VOID Cleanup(_Inout_updates_all_(BufferLength) PCHAR DriverLocation)
{
	if (hDevice) {
		CloseHandle(hDevice);
	}

    //
    // Unload the driver.  Ignore any errors.
    //

    ManageDriver(DRIVER_NAME,
                 DriverLocation,
                 DRIVER_FUNC_REMOVE
                 );
}

DWORD WINAPI Reader(LPVOID lpThreadParameter)
{
	ULONG data;
	ULONG byte_read;
	if (!ReadFile(hDevice, (PVOID)&data, sizeof(ULONG), &byte_read, NULL)) {
		printf ( "Error: Read Failed: %d\n", GetLastError());
        ExitThread(1);
	}

    printf("Exiting thread %d \n", GetCurrentThreadId());

    ExitThread(0);
}

DWORD WINAPI Writer(LPVOID lpThreadParameter)
{
	ULONG data = 0;
	ULONG byte_write;
	if (!WriteFile(hDevice, (PVOID)&data, sizeof(ULONG), &byte_write, NULL)) {
		printf ( "Error: Write Failed: %d\n", GetLastError());
        ExitThread(1);
	}
	printf("Exiting thread %d \n", GetCurrentThreadId());

    ExitThread(0);
}

BOOLEAN
SetupDriverName(
    _Inout_updates_all_(BufferLength) PCHAR DriverLocation,
    _In_ ULONG BufferLength
    )
{
    HANDLE fileHandle;
    DWORD driverLocLen = 0, i;

    //
    // Get the current directory.
    //

    driverLocLen = GetCurrentDirectory(BufferLength,
                                       DriverLocation
                                       );

    if (driverLocLen == 0) {

        printf("GetCurrentDirectory failed!  Error = %d \n", GetLastError());

        return FALSE;
    }
	
	for (i = driverLocLen - 1 ; i >= 0 ; i--) {
		if (DriverLocation[i] == '\\') {
			DriverLocation[i + 1] = '\0';
			break;
		}
	}

    //
    // Setup path name to driver file.
    //
    if (FAILED( StringCbCat(DriverLocation, BufferLength, "x64\\Win7Debug\\"DRIVER_NAME".sys") )) {
        return FALSE;
    }

    //
    // Insure driver file is in the specified directory.
    //

    if ((fileHandle = CreateFile(DriverLocation,
                                 GENERIC_READ,
                                 0,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL
                                 )) == INVALID_HANDLE_VALUE) {


        printf("%s.sys is not loaded.\n", DRIVER_NAME);

        //
        // Indicate failure.
        //

        return FALSE;
    }

    //
    // Close open file handle.
    //

    if (fileHandle) {

        CloseHandle(fileHandle);
    }

    //
    // Indicate success.
    //

    return TRUE;


}   // SetupDriverName