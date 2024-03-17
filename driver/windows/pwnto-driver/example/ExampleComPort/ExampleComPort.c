/* Copyright (c) 2024-2024 Pwnto-driver - See the LICENSE file */

#include <stdio.h>
#include <windows.h>
#include <fltuser.h>

#include <Common.h>

int main(void)
{
    HANDLE HandleComPort = INVALID_HANDLE_VALUE;
    UCHAR Message[] = "Hello World";
    DWORD ByteReturned = 0;
    HRESULT HResult = E_FAIL;

    printf("ExampleComPort\n");

    HResult = FilterConnectCommunicationPort(COMMON_COM_PORT_NAME, 0, NULL, 0, NULL, &HandleComPort);
    if (FAILED(HResult)) {
        printf("Function FilterConnectCommunicationPort() failed (HRESULT: 0x%X)\n",
            HRESULT_FROM_WIN32(GetLastError()));
        goto Cleanup;
    }

    HResult = FilterSendMessage(HandleComPort, &Message, (sizeof(Message)-1), NULL, 0, &ByteReturned);
    if (FAILED(HResult)) {
        printf("Function FilterSendMessage() failed (HRESULT: 0x%X)\n",
            HRESULT_FROM_WIN32(GetLastError()));
        goto Cleanup;
    }

    HResult = S_OK;

Cleanup:
    if (HandleComPort != NULL && HandleComPort != INVALID_HANDLE_VALUE) {
        CloseHandle(HandleComPort);
        HandleComPort = INVALID_HANDLE_VALUE;
    }

    if (FAILED(HResult)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
