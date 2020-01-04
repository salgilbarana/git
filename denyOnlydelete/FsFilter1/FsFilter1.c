/*++

Module Name:

    FsFilter1.c

Abstract:

    This is the main module of the FsFilter1 miniFilter driver.

Environment:

    Kernel mode

--*/

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>

//typedef struct {
//	/* The filter that results from a call to FltRegisterFilter. */
//	PFLT_FILTER filter;
//} DRIVER_DATA;
//
//static DRIVER_DATA driver_data;


#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")


PFLT_FILTER gFilterHandle;
ULONG_PTR OperationStatusCtx = 1;

#define PTDBG_TRACE_ROUTINES            0x00000001
#define PTDBG_TRACE_OPERATION_STATUS    0x00000002

ULONG gTraceFlags = 0;


#define PT_DBG_PRINT( _dbgLevel, _string )          \
    (FlagOn(gTraceFlags,(_dbgLevel)) ?              \
        DbgPrint _string :                          \
        ((int)0))

/*************************************************************************
    Prototypes
*************************************************************************/

EXTERN_C_START

BOOLEAN
equal_tail_unicode_string(
	_In_ const PUNICODE_STRING full,
	_In_ const PUNICODE_STRING tail,
	_In_ BOOLEAN case_insensitive
);

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    );

NTSTATUS
InstanceSetup (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
    );

//VOID
//InstanceTeardownStart (
//    _In_ PCFLT_RELATED_OBJECTS FltObjects,
//    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
//    );

//VOID
//FsFilter1InstanceTeardownComplete (
//    _In_ PCFLT_RELATED_OBJECTS FltObjects,
//    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
//    );

NTSTATUS
InstanceQueryTeardown (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    );

BOOLEAN get_file_name_information(PFLT_CALLBACK_DATA data,
	PFLT_FILE_NAME_INFORMATION* name_info
);

FLT_PREOP_CALLBACK_STATUS process_irp(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext,
	_In_ BOOLEAN bit
);

FLT_PREOP_CALLBACK_STATUS
PreOperationCallback(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext
);


NTSTATUS
FilterUnload(
	_In_ FLT_FILTER_UNLOAD_FLAGS Flags
);

//FLT_PREOP_CALLBACK_STATUS
//FsFilter1PreOperation (
//    _Inout_ PFLT_CALLBACK_DATA Data,
//    _In_ PCFLT_RELATED_OBJECTS FltObjects,
//    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
//    );

//VOID
//OperationStatusCallback (
//    _In_ PCFLT_RELATED_OBJECTS FltObjects,
//    _In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
//    _In_ NTSTATUS OperationStatus,
//    _In_ PVOID RequesterContext
//    );

//FLT_POSTOP_CALLBACK_STATUS
//FsFilter1PostOperation (
//    _Inout_ PFLT_CALLBACK_DATA Data,
//    _In_ PCFLT_RELATED_OBJECTS FltObjects,
//    _In_opt_ PVOID CompletionContext,
//    _In_ FLT_POST_OPERATION_FLAGS Flags
//    );
//
//FLT_PREOP_CALLBACK_STATUS
//FsFilter1PreOperationNoPostOperation (
//    _Inout_ PFLT_CALLBACK_DATA Data,
//    _In_ PCFLT_RELATED_OBJECTS FltObjects,
//    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
//    );
//
//BOOLEAN
//FsFilter1DoRequestOperationStatus(
//    _In_ PFLT_CALLBACK_DATA Data
//    );

EXTERN_C_END

//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, FilterUnload)
#pragma alloc_text(PAGE, InstanceQueryTeardown)
#pragma alloc_text(PAGE, InstanceSetup)
//#pragma alloc_text(PAGE, InstanceTeardownStart)
//#pragma alloc_text(PAGE, InstanceTeardownComplete)
#endif

//
//  operation registration
//

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {

	  { IRP_MJ_CREATE,
		0,
		PreOperationCallback,
		NULL },

	  { IRP_MJ_SET_INFORMATION,
		0,
		PreOperationCallback,
		NULL },

    { IRP_MJ_OPERATION_END }
};


//
//  This defines what we want to filter with FltMgr
//

CONST FLT_REGISTRATION filter_registration = {

    sizeof( FLT_REGISTRATION ),		//  Size
    FLT_REGISTRATION_VERSION,		//  Version
    0,								//  Flags
    NULL,							//  Context
    Callbacks,						//  Operation callbacks

	FilterUnload,							//  MiniFilterUnload

    InstanceSetup,					//  InstanceSetup
	InstanceQueryTeardown,			//  InstanceQueryTeardown
    NULL,							//  InstanceTeardownStart
    NULL,							//  InstanceTeardownComplete

    NULL,							//  GenerateFileName
    NULL,							//  GenerateDestinationFileName
    NULL							//  NormalizeNameComponent
};




/*************************************************************************
    MiniFilter initialization and unload routines.
*************************************************************************/

NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This is the initialization routine for this miniFilter driver.  This
    registers with FltMgr and initializes all global data structures.

Arguments:

    DriverObject - Pointer to driver object created by the system to
        represent this driver.

    RegistryPath - Unicode string identifying where the parameters for this
        driver are located in the registry.

Return Value:

    Routine can return non success error codes.

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER( RegistryPath );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("FsFilter1!DriverEntry: Entered\n") );

    //
    //  Register with FltMgr to tell it our callback routines
    //

    status = FltRegisterFilter( DriverObject,
                                &filter_registration,
                                &gFilterHandle );

    FLT_ASSERT( NT_SUCCESS( status ) );

    if (NT_SUCCESS( status )) {

        //
        //  Start filtering i/o
        //

        //status = FltStartFiltering( gFilterHandle );

		status = FltStartFiltering(gFilterHandle);

        if (!NT_SUCCESS( status )) {

//			FltUnregisterFilter(gFilterHandle);

			FltUnregisterFilter(gFilterHandle);
        }
		DbgPrint("[ miniFilter ] Start Filtering \n");
    }

    return status;
}

NTSTATUS
FilterUnload (
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
    )
/*++

Routine Description:

    This is the unload routine for this miniFilter driver. This is called
    when the minifilter is about to be unloaded. We can fail this unload
    request if this is not a mandatory unload indicated by the Flags
    parameter.

Arguments:

    Flags - Indicating if this is a mandatory unload.

Return Value:

    Returns STATUS_SUCCESS.

--*/
{
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("FsFilter1!FsFilter1Unload: Entered\n") );

    FltUnregisterFilter( gFilterHandle );

    return STATUS_SUCCESS;
}


/*************************************************************************
    MiniFilter callback routines.
*************************************************************************/


NTSTATUS
InstanceSetup(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
	_In_ DEVICE_TYPE VolumeDeviceType,
	_In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
)
/*++

Routine Description:

	This routine is called whenever a new instance is created on a volume. This
	gives us a chance to decide if we need to attach to this volume or not.

	If this routine is not defined in the registration structure, automatic
	instances are always created.

Arguments:

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
		opaque handles to this filter, instance and its associated volume.

	Flags - Flags describing the reason for this attach request.

Return Value:

	STATUS_SUCCESS - attach
	STATUS_FLT_DO_NOT_ATTACH - do not attach

--*/
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(VolumeDeviceType);
	UNREFERENCED_PARAMETER(VolumeFilesystemType);

	PAGED_CODE();

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("FsFilter1!FsFilter1InstanceSetup: Entered\n"));

	//return STATUS_SUCCESS;
	return  (VolumeDeviceType != FILE_DEVICE_CD_ROM_FILE_SYSTEM) ?
		STATUS_SUCCESS :
		STATUS_FLT_DO_NOT_ATTACH;
}


NTSTATUS
InstanceQueryTeardown(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
)
/*++

Routine Description:

	This is called when an instance is being manually deleted by a
	call to FltDetachVolume or FilterDetach thereby giving us a
	chance to fail that detach request.

	If this routine is not defined in the registration structure, explicit
	detach requests via FltDetachVolume or FilterDetach will always be
	failed.

Arguments:

	FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
		opaque handles to this filter, instance and its associated volume.

	Flags - Indicating where this detach request came from.

Return Value:

	Returns the status of this operation.

--*/
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	PAGED_CODE();

	PT_DBG_PRINT(PTDBG_TRACE_ROUTINES,
		("FsFilter1!FsFilter1InstanceQueryTeardown: Entered\n"));

	return STATUS_SUCCESS;
}

BOOLEAN get_file_name_information(PFLT_CALLBACK_DATA data,
	PFLT_FILE_NAME_INFORMATION* name_info)
{
	/* Get name information. */
	if (NT_SUCCESS(FltGetFileNameInformation(
		data,
		FLT_FILE_NAME_NORMALIZED |
		FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP,
		name_info
	))) {
		/* Parse file name information. */
		if (NT_SUCCESS(FltParseFileNameInformation(*name_info))) {
			return TRUE;
		}

		FltReleaseFileNameInformation(*name_info);
	}

	return FALSE;
}

FLT_PREOP_CALLBACK_STATUS process_irp(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext,
	_In_ BOOLEAN bit)
{

	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);

	PFLT_FILE_NAME_INFORMATION name_info;
	//PFLT_DEFERRED_IO_WORKITEM work;

	if (get_file_name_information(Data, &name_info))
		if (bit == FALSE) {
			DbgPrint("[ miniFilter ] [ Deleted ] Filename: '%wZ'.", &name_info->Name);

			UNICODE_STRING txt_file;
			RtlInitUnicodeString(&txt_file, L".txt");

			if (TRUE == equal_tail_unicode_string(&name_info->Name, &txt_file, TRUE))
			{
				//
				//	matched
				// 
				Data->IoStatus.Status = STATUS_ACCESS_DENIED;
				Data->IoStatus.Information = 0;

				return FLT_PREOP_COMPLETE;
			}
			else
			{

			}
		}

	///* Get name information. */
	//if (get_file_name_information(Data, &name_info)) {
	//	if (bit == TRUE)
	//		DbgPrint("[ miniFilter ] [ Writed ] Filename: '%wZ'.", &name_info->Name);
	//	else if (bit == FALSE)
	//		DbgPrint("[ miniFilter ] [ Deleted ] Filename: '%wZ'.", &name_info->Name);
	//}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS
PreOperationCallback(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext
) {
	//DbgPrint("##newDriver [ miniFilter ] PreOperationCallback Called \n");

	if (FLT_IS_IRP_OPERATION(Data)) {
		/* Open file? */
		if (Data->Iopb->MajorFunction == IRP_MJ_CREATE) {
			/* Open file for writing/appending? */
			if (Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess &
				(FILE_WRITE_DATA | FILE_APPEND_DATA)) {
				return process_irp(Data, FltObjects, CompletionContext, TRUE);
			}
		}
		else if (Data->Iopb->MajorFunction == IRP_MJ_SET_INFORMATION) {
			if (Data->Iopb->Parameters.SetFileInformation.FileInformationClass == FileDispositionInformation) {
				if (((FILE_DISPOSITION_INFORMATION*)
					Data->Iopb->Parameters.SetFileInformation.InfoBuffer
					)->DeleteFile) {
					return process_irp(Data, FltObjects, CompletionContext, FALSE);
				}
			}
		}
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

/// @brief	full: ABCDEFGHIJKLMNOPQ
	///			tail :            MNOPQ
	///							 순서로 문자열을 비교하고, 
	///			문자열이 매칭되면 true 를 리턴
BOOLEAN
equal_tail_unicode_string(
	_In_ const PUNICODE_STRING full,
	_In_ const PUNICODE_STRING tail,
	_In_ BOOLEAN case_insensitive
)
{
	ULONG i;
	USHORT full_count;
	USHORT tail_count;

	if (full == NULL || tail == NULL) return FALSE;

	full_count = full->Length / sizeof(WCHAR);
	tail_count = tail->Length / sizeof(WCHAR);

	if (full_count < tail_count) return FALSE;
	if (tail_count == 0) return FALSE;

	if (case_insensitive)
	{
		for (i = 1; i <= tail_count; ++i)
		{
			if (RtlUpcaseUnicodeChar(full->Buffer[full_count - i]) !=
				RtlUpcaseUnicodeChar(tail->Buffer[tail_count - i]))
				return FALSE;
		}
	}
	else
	{
		for (i = 1; i <= tail_count; ++i)
		{
			if (full->Buffer[full_count - i] != tail->Buffer[tail_count - i])
				return FALSE;
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
