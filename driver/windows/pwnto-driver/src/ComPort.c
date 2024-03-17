/* Copyright (c) 2024-2024 Pwnto-driver - See the LICENSE file */

#include <ComPort.h>

static NTSTATUS ComPortPortConnectNotify(
	_In_ PFLT_PORT ClientPort,
	_In_ PVOID ServerPortCookie,
	_In_ PVOID ConnectionContext,
	_In_ ULONG SizeOfContext,
	_Out_ PVOID* ConnectionPortCookie)
{
	UNREFERENCED_PARAMETER(ServerPortCookie);
	UNREFERENCED_PARAMETER(ConnectionContext);
	UNREFERENCED_PARAMETER(SizeOfContext);
	UNREFERENCED_PARAMETER(ConnectionPortCookie);

	GlobalPwntoManager.ClientPort = ClientPort;
	
	if (ConnectionPortCookie != NULL) {
		*ConnectionPortCookie = NULL;
	}

	return STATUS_SUCCESS;
}

static VOID ComPortPortDisconnectNotify(
	_In_ PVOID ConnectionCookie)
{
	UNREFERENCED_PARAMETER(ConnectionCookie);

	FltCloseClientPort(GlobalPwntoManager.Filter, &GlobalPwntoManager.ClientPort);
	GlobalPwntoManager.ClientPort = NULL;
}

static VOID ComPortPortMessageNotifyDebug(
	_In_reads_bytes_opt_(InputBufferLength) PUCHAR InputBuffer,
	_In_ ULONG InputBufferLength)
{
	ULONG InputBufferIndex = 0;
	ULONG UserMessageIndex = 0;
	UCHAR UserMessageHex[64] = { 0 };
	UCHAR UserMessageASCII[64] = { 0 };
	CONST ULONG UserMessageSize = (sizeof(UserMessageHex) - 3); // 3 bytes (2 for hexadecimal + 1 space)
	CONST UCHAR HexNumber[] = "0123456789abcdef";

	if (InputBuffer == NULL) {
		return;
	}

	while (InputBufferIndex < InputBufferLength && UserMessageIndex < UserMessageSize) {
		UserMessageHex[UserMessageIndex++] = HexNumber[(InputBuffer[InputBufferIndex] & 0xf0) >> 4];
		UserMessageHex[UserMessageIndex++] = HexNumber[(InputBuffer[InputBufferIndex] & 0x0f)];
		UserMessageHex[UserMessageIndex++] = ' ';
		UserMessageASCII[InputBufferIndex] = (InputBuffer[InputBufferIndex] >= 0x20 && \
			InputBuffer[InputBufferIndex] <= 0x7e) ? InputBuffer[InputBufferIndex] : '.';
		++InputBufferIndex;
	}

	if (InputBufferLength > 0) {
		DbgPrint("userland message in hexadecimal: %s (%s)", UserMessageHex, UserMessageASCII);
		if (UserMessageIndex >= UserMessageSize) {
			DbgPrint("truncated");
		}
	}
}

static NTSTATUS ComPortPortMessageNotify(
	_In_ PVOID PortCookie,
	_In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
	_In_ ULONG InputBufferLength,
	_Out_writes_bytes_to_opt_(OutputBufferLength, *ReturnOutputBufferLength) PVOID OutputBuffer,
	_In_ ULONG OutputBufferLength,
	_Out_ PULONG ReturnOutputBufferLength)
{
	NTSTATUS NTStatus = STATUS_UNSUCCESSFUL;

	UNREFERENCED_PARAMETER(PortCookie);
	UNREFERENCED_PARAMETER(InputBuffer);
	UNREFERENCED_PARAMETER(InputBufferLength);
	UNREFERENCED_PARAMETER(OutputBufferLength);

	if (OutputBuffer != NULL) {
		OutputBuffer = NULL;
	}

	if (ReturnOutputBufferLength != NULL) {
		*ReturnOutputBufferLength = 0;
	}

	if (InputBuffer == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	try {
		ProbeForRead(InputBuffer, InputBufferLength, sizeof(UCHAR));
		ComPortPortMessageNotifyDebug(InputBuffer, InputBufferLength);
	}
	except(EXCEPTION_EXECUTE_HANDLER) {
		NTStatus = GetExceptionCode();
		DbgPrint("Function ComPortPortMessageNotify() failed");
		goto Cleanup;
	}

	NTStatus = STATUS_SUCCESS;

Cleanup:
	return NTStatus;
}

NTSTATUS ComPortInitialize(
	_In_ _Pre_defensive_ CONST PFLT_FILTER Filter)
{
	UNICODE_STRING ComPortName = RTL_CONSTANT_STRING(COMMON_COM_PORT_NAME);
	PSECURITY_DESCRIPTOR SecurityDescriptor = { 0 };
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	NTSTATUS NTStatus = STATUS_UNSUCCESSFUL;

	if (Filter == NULL) {
		DbgPrint("(Filter == NULL)");
		goto Cleanup;
	}

	NTStatus = FltBuildDefaultSecurityDescriptor(&SecurityDescriptor, FLT_PORT_ALL_ACCESS);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function FltBuildDefaultSecurityDescriptor() failed");
		goto Cleanup;
	}

	InitializeObjectAttributes(&ObjectAttributes, &ComPortName,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, SecurityDescriptor);

	NTStatus = FltCreateCommunicationPort(Filter, &GlobalPwntoManager.Port, &ObjectAttributes,
		NULL, ComPortPortConnectNotify, ComPortPortDisconnectNotify, ComPortPortMessageNotify, 1);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function FltCreateComPort() failed: %x", NTStatus);
		goto Cleanup;
	}

	NTStatus = STATUS_SUCCESS;

Cleanup:
	if (SecurityDescriptor != NULL) {
		FltFreeSecurityDescriptor(SecurityDescriptor);
		SecurityDescriptor = NULL;
	}

	return NTStatus;
}


VOID ComPortCleanup()
{
	if (GlobalPwntoManager.Port != NULL) {
		FltCloseCommunicationPort(GlobalPwntoManager.Port);
		GlobalPwntoManager.Port = NULL;
	}
}
