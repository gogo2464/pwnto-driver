/* Copyright (c) 2024-2024 Pwnto-driver - See the LICENSE file */

#pragma once

#include <Pwnto.h>

NTSTATUS ComPortInitialize(
	_In_ _Pre_defensive_ CONST PFLT_FILTER Filter);

VOID ComPortCleanup();