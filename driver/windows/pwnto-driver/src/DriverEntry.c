/* Copyright (c) 2024-2024 Pwnto-driver - See the LICENSE file */

#include <Pwnto.h>

struct PwntoManager GlobalPwntoManager = { 0 };

NTSTATUS DriverEntryFilterUnloadCallback(
	_In_ FLT_FILTER_UNLOAD_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Flags);

	ComPortCleanup();

	if (GlobalPwntoManager.Filter != NULL) {
		FltUnregisterFilter(GlobalPwntoManager.Filter);
		GlobalPwntoManager.Filter = NULL;
	}

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntryInstanceSetupCallback(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
	_In_ DEVICE_TYPE VolumeDeviceType,
	_In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(VolumeDeviceType);
	UNREFERENCED_PARAMETER(VolumeFilesystemType);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntryInstanceQueryTeardownCallback(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	return STATUS_SUCCESS;
}

VOID DriverEntryInstanceTeardownStartCallback(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Reason)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Reason);
}

VOID DriverEntryInstanceTeardownCompleteCallback(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Reason)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Reason);
}


static NTSTATUS DriverEntryFilterManager(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath)
{
	CONST FLT_OPERATION_REGISTRATION OperationRegistration[] = {
		{ IRP_MJ_OPERATION_END }
	};

	CONST FLT_CONTEXT_REGISTRATION ContextRegistration[] = {
		{ FLT_CONTEXT_END }
	};

	CONST FLT_REGISTRATION Registration = {
		.Size = sizeof(FLT_REGISTRATION),
		.Version = FLT_REGISTRATION_VERSION,
		.Flags = 0,
		.ContextRegistration = ContextRegistration,
		.OperationRegistration = OperationRegistration,
		.FilterUnloadCallback = DriverEntryFilterUnloadCallback,
		.InstanceSetupCallback = DriverEntryInstanceSetupCallback,
		.InstanceQueryTeardownCallback = DriverEntryInstanceQueryTeardownCallback,
		.InstanceTeardownStartCallback = DriverEntryInstanceTeardownStartCallback,
		.InstanceTeardownCompleteCallback = DriverEntryInstanceTeardownCompleteCallback,
	};

	NTSTATUS NTStatus = STATUS_UNSUCCESSFUL;

	if (DriverObject == NULL) {
		DbgPrint("Function DriverEntryFilterManager() failed: (DriverObject == NULL)");
		return STATUS_INVALID_PARAMETER;
	}

	if (RegistryPath == NULL) {
		DbgPrint("Function DriverEntryFilterManager() failed: (RegistryPath == NULL)");
		return STATUS_INVALID_PARAMETER;
	}

	NTStatus = FltRegisterFilter(DriverObject, &Registration, &GlobalPwntoManager.Filter);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function FltRegisterFilter() failed");
		goto Cleanup;
	}

	ComPortInitialize(GlobalPwntoManager.Filter);

	NTStatus = FltStartFiltering(GlobalPwntoManager.Filter);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function FltStartFiltering() failed");
		goto Cleanup;
	}

	NTStatus = STATUS_SUCCESS;

Cleanup:
	return NTStatus;
}

// use this function instead of the .inf driver installation file
static NTSTATUS DriverEntryConfiguration(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING KeyNameInstances = RTL_CONSTANT_STRING(L"Instances");
	UNICODE_STRING KeyNameDefaultInstance = RTL_CONSTANT_STRING(L"DefaultInstance");
	UNICODE_STRING KeyNameAltitude = RTL_CONSTANT_STRING(L"Altitude");
	UNICODE_STRING KeyNameFlags = RTL_CONSTANT_STRING(L"Flags");
	WCHAR KeyValueDefaultInstance[] = L"Pwnto DefaultInstance";
	WCHAR KeyValueAltitude[] = L"1337";
	ULONG KeyValueFlags = 0;
	OBJECT_ATTRIBUTES KeyObjectAttributes = { 0 };
	HANDLE KeyHandleDriverPath = NULL;
	HANDLE KeyHandleInstances = NULL;
	HANDLE KeyHandleDefaultInstance = NULL;
	NTSTATUS NTStatusAfterCleanup = STATUS_UNSUCCESSFUL;
	NTSTATUS NTStatus = STATUS_UNSUCCESSFUL;

	if (DriverObject == NULL) {
		DbgPrint("Function DriverEntryConfiguration() failed: (DriverObject == NULL)");
		return STATUS_INVALID_PARAMETER;
	}

	if (RegistryPath == NULL) {
		DbgPrint("Function DriverEntryConfiguration() failed: (RegistryPath == NULL)");
		return STATUS_INVALID_PARAMETER;
	}

	InitializeObjectAttributes(&KeyObjectAttributes, RegistryPath, OBJ_KERNEL_HANDLE, KeyHandleDriverPath, NULL);
	NTStatus = ZwOpenKey(&KeyHandleDriverPath, KEY_WRITE, &KeyObjectAttributes);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function ZwOpenKey() failed");
		goto Cleanup;
	}

	// Instances
	InitializeObjectAttributes(&KeyObjectAttributes, &KeyNameInstances, OBJ_KERNEL_HANDLE, KeyHandleDriverPath, NULL);
	NTStatus = ZwCreateKey(&KeyHandleInstances, KEY_WRITE, &KeyObjectAttributes, 0, NULL, 0, NULL);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function ZwCreateKey() failed");
		goto Cleanup;
	}

	// DefaultInstance
	NTStatus = ZwSetValueKey(KeyHandleInstances, &KeyNameDefaultInstance, 0, REG_SZ,
		KeyValueDefaultInstance, sizeof(KeyValueDefaultInstance));
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function ZwSetValueKey() failed");
		goto Cleanup;
	}

	RtlInitUnicodeString(&KeyNameDefaultInstance, KeyValueDefaultInstance);
	InitializeObjectAttributes(&KeyObjectAttributes, &KeyNameDefaultInstance, OBJ_KERNEL_HANDLE, KeyHandleInstances, NULL);
	NTStatus = ZwCreateKey(&KeyHandleDefaultInstance, KEY_WRITE, &KeyObjectAttributes, 0, NULL, 0, NULL);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function ZwCreateKey() failed");
		goto Cleanup;
	}

	// Altitude
	NTStatus = ZwSetValueKey(KeyHandleDefaultInstance, &KeyNameAltitude, 0, REG_SZ, KeyValueAltitude, sizeof(KeyValueAltitude));
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function ZwSetValueKey() failed");
		goto Cleanup;
	}

	// Flags
	NTStatus = ZwSetValueKey(KeyHandleDefaultInstance, &KeyNameFlags, 0, REG_DWORD, &KeyValueFlags, sizeof(KeyValueFlags));
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function ZwSetValueKey() failed");
		goto Cleanup;
	}

	NTStatus = STATUS_SUCCESS;

Cleanup:

	if (KeyHandleDriverPath != NULL) {
		NTStatusAfterCleanup = ZwClose(KeyHandleDriverPath);
		if (!NT_SUCCESS(NTStatusAfterCleanup)) {
			DbgPrint("Function ZwClose() failed");
			if (NT_SUCCESS(NTStatus)) {
				NTStatus = NTStatusAfterCleanup;
			}
		}
		KeyHandleDriverPath = NULL;
	}

	if (KeyHandleInstances != NULL) {
		if (!NT_SUCCESS(NTStatus)) {
			// will delete Instances key and DefaultInstance subkey
			NTStatusAfterCleanup = ZwDeleteKey(KeyHandleInstances);
			if (!NT_SUCCESS(NTStatusAfterCleanup)) {
				DbgPrint("Function ZwDeleteKey() failed");
				if (NT_SUCCESS(NTStatus)) {
					NTStatus = NTStatusAfterCleanup;
				}
			}
		}

		NTStatusAfterCleanup = ZwClose(KeyHandleInstances);
		if (!NT_SUCCESS(NTStatusAfterCleanup)) {
			DbgPrint("Function ZwClose() failed");
			if (NT_SUCCESS(NTStatus)) {
				NTStatus = NTStatusAfterCleanup;
			}
		}
	}

	if (KeyHandleDefaultInstance != NULL) {
		NTStatusAfterCleanup = ZwClose(KeyHandleDefaultInstance);
		if (!NT_SUCCESS(NTStatusAfterCleanup)) {
			DbgPrint("Function ZwClose() failed");
			if (NT_SUCCESS(NTStatus)) {
				NTStatus = NTStatusAfterCleanup;
			}
		}
		KeyHandleDefaultInstance = NULL;
	}

	return NTStatus;
}

static VOID DriverEntryUnload(
	_In_ PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING DeviceSymbolicName = RTL_CONSTANT_STRING(PWNTO_SYMBOLIC_NAME);
	NTSTATUS NTStatus = STATUS_UNSUCCESSFUL;

	if (DriverObject == NULL) {
		DbgPrint("Function DriverEntryUnload() failed: (DriverObject == NULL)");
		return;
	}

	if (DriverObject->DeviceObject == NULL) {
		DbgPrint("Function DriverEntryUnload() failed: (DriverObject->DeviceObject == NULL)");
		return;
	}

	DbgPrint("Driver unloaded");

	IoDeleteDevice(DriverObject->DeviceObject);

	NTStatus = IoDeleteSymbolicLink(&DeviceSymbolicName);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Could not create device %wZ", DeviceSymbolicName);
	}
}

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(PWNTO_DEVICE);
	UNICODE_STRING DeviceSymbolicName = RTL_CONSTANT_STRING(PWNTO_SYMBOLIC_NAME);
	NTSTATUS NTStatus = STATUS_UNSUCCESSFUL;

	DbgPrint("entry");

	if (DriverObject == NULL) {
		DbgPrint("Function DriverEntry() failed: (DriverObject == NULL)");
		return STATUS_INVALID_PARAMETER;
	}

	if (RegistryPath == NULL) {
		DbgPrint("Function DriverEntry() failed: (RegistryPath == NULL)");
		return STATUS_INVALID_PARAMETER;
	}

	DbgPrint("Driver loaded");

	DriverObject->DriverUnload = DriverEntryUnload;

	NTStatus = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DriverObject->DeviceObject);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Could not create device %wZ", DeviceName);
		goto Cleanup;
	}
		
	DbgPrint("Device %wZ created", DeviceName);

	NTStatus = IoCreateSymbolicLink(&DeviceSymbolicName, &DeviceName);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Error creating symbolic link %wZ", DeviceSymbolicName);
		goto Cleanup;
	}

	DbgPrint("Symbolic link %wZ created", DeviceSymbolicName);

	NTStatus = DriverEntryConfiguration(DriverObject, RegistryPath);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function DriverEntryConfiguration() failed");
		goto Cleanup;
	}

	NTStatus = DriverEntryFilterManager(DriverObject, RegistryPath);
	if (!NT_SUCCESS(NTStatus)) {
		DbgPrint("Function DriverEntryFilterManager() failed");
		goto Cleanup;
	}

	NTStatus = STATUS_SUCCESS;

Cleanup:
	return NTStatus;
}