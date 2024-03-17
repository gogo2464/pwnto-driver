/* Copyright (c) 2024-2024 Pwnto-driver - See the LICENSE file */

#pragma once

#include <fltKernel.h>

#include <Common.h>

#include <ComPort.h>

#define PWNTO_DEVICE		(L"\\Device\\pwnto-driver")
#define PWNTO_SYMBOLIC_NAME (L"\\??\\pwnto-driver")

struct PwntoManager {
	PFLT_FILTER Filter;
	PFLT_PORT Port;
	PFLT_PORT ClientPort;
};

extern struct PwntoManager GlobalPwntoManager;
