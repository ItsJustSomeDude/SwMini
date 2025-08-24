#ifdef __arm__

#include <string.h>
#include "32patch.h"
#include "../symbol.h"
#include "../lua/lua.h"
#include "../log.h"
#include "../tools/prober.h"

#define LOG_TAG "Mini32Patch"

/**
 * NOTE!
 *
 * This file contains a lot of failures in trying to make the 32-bit engine patch work
 * with a NULL Bytes field. Many bones lie ahead.
 */

//char* newDefaultBytesBuffer = "ABC123";

//STATIC_DL_HOOK_SYMBOL(
//        programConstructor,
//        "_ZN5Caver7ProgramC2ERKSs",
//        void*,
//        (void* this, size_t **param_1, int param_2, unsigned int param_3)
//) {
//    LOGD("Hooked Caver::Program constructor called, this = %p", this);
//
//    return orig_programConstructor(this, param_1, param_2, param_3);
//}

//STATIC_DL_HOOK_SYMBOL(
//        programProtoConstructor1,
//        "_ZN5Caver5Proto7ProgramC1Ev",
//        void,
//        (void* this)
//) {
////    LOGD("Hooked Caver::Proto::Program constructor called, this: %p (I want +16: %p)", this, this + 16);
//
//    orig_programProtoConstructor1(this);
//
////    LOGD("So... I think this should be an empty buffer? '%s'", **(char ***)(this + 16));
//}

//STATIC_DL_HOOK_SYMBOL(
//        programProtoNew,
//        "_ZNK5Caver5Proto7Program3NewEv",
//        void,
//        (void* this, int param_2, int param_3, unsigned int param_4)
//) {
//    LOGD("Hooked Caver::Proto::Program::New called, this: %p", this);
//
//    orig_programProtoNew(this, param_2, param_3, param_4);
//
//    LOGD("Empty buffer? '%s'", **(char ***)(this + 16));
//
////    **(void***)(this + 16) = newDefaultBytesBuffer;
//}



//STATIC_DL_HOOK_SYMBOL(idk, "_ZN5Caver7Program14InitWithStringERKSsPSs", void, (void* this, size_t **param_1, size_t **param_2, unsigned int param_3)) {
//    LOGD("Hooked lol");
//    orig_idk(this, param_1, param_2, param_3);
//}
//
//STATIC_DL_HOOK_SYMBOL(exec, "_ZN5Caver12ProgramState14ExecuteProgramERKNS_7ProgramE", void, (void* this, void* program)) {
//    LOGD("ExecuteProgram");
//    orig_exec(this, program);
//}
//
//STATIC_DL_HOOK_SYMBOL(loadPrgm, "_ZN5Caver12ProgramState11LoadProgramERKNS_7ProgramE", void, (void* this, void* program)) {
//    LOGD("LoadProgram");
//    orig_loadPrgm(this, program);
//}


//STATIC_DL_HOOK_SYMBOL(
//        loadFromProto,
//        "_ZN5Caver7Program23LoadFromProtobufMessageERKNS_5Proto7ProgramE",
//        void,
//        (void* this, void* message, void* somePointer, unsigned int param_3)
//) {
//    LOGD("Hooked loadFromProto called - this: %p, message: %p, something: %p, (%i)", this, message, somePointer, param_3);
//
//    if(message == somePointer) {
//        LOGD("Well I found something... I think this is going to try to be executed maybe: %p", message);
//
//        LOGD("");
//        LOGD("Destructor ** = %p", *(void**) message);
//        LOGD("Cached Size = %i", *(int *)(message + 4));
//        LOGD("Name ** = %p", *(void **)(message + 8));
//        LOGD("String ** = %p", *(void **)(message + 12));
//        LOGD("Bytes ** = %p", *(void **)(message + 16));
//
//        LOGD("Deref String * = %p", **(void ***)(message + 12));
//        LOGD("Deref Bytes * = %p", **(void ***)(message + 16));
//
//        // Write the pointer to String to the pointer at Bytes???
//
//        // This is what the struct actually holds for String.
//        void** messageStructString = *(void **)(message + 12);
//
//        // This is what the struct actually holds for Bytes.
//        void** messageStructBytes = *(void **)(message + 16);
//
//        // I think both of those are pointers to pointers to the actual data.
//
//        char* string = *messageStructString;
//        char* bytes = *messageStructBytes;
//
//        // Ok, so now those two should be the pointers to buffers???
//
////        LOGD("Uh. Bytes Buffer? (%p) %s", **(char***)(message + 16), **(char***)(message + 16));
//
//        LOGD("%p, %p", messageStructString, messageStructBytes);
//        LOGD("%p, %p", string, bytes);
//
//        // (Struct) -> (Pointer) -> Data
//        //
//
////        *messageStructBytes = messageStructString;
//
////        *string = *bytes;
//        **(void ***)(message + 16) = strdup(**(void ***)(message + 12));
//
//        LOGD("");
//        LOGD("Changes made...");
//        LOGD("");
//        LOGD("Destructor ** = %p", *(void**) message);
//        LOGD("Cached Size = %i", *(int *)(message + 4));
//        LOGD("Name ** = %p", *(void **)(message + 8));
//        LOGD("String ** = %p", *(void **)(message + 12));
//        LOGD("Bytes ** = %p", *(void **)(message + 16));
//
//        LOGD("Deref String * = %p", **(void ***)(message + 12));
//        LOGD("Deref Bytes * = %p", **(void ***)(message + 16));
//
//
//
////        *(void **)(message + 16) = *(void **)(message + 12);
//
//        char* stringBuf = **(char***)(message + 12);
//        char* bytesBuf = **(char***)(message + 16);
//
//        LOGD("Uh. String Buffer? (%p) %s", stringBuf, stringBuf);
//        LOGD("Uh. Bytes Buffer? (%p) %s", bytesBuf, bytesBuf);
//    }
//
//    orig_loadFromProto(this, message, somePointer, param_3);
//
//}

//STATIC_DL_HOOK_SYMBOL(
//        newStateForProgram,
//        "_ZN5Caver5Scene25NewProgramStateForProgramERKNS_7ProgramE",
//        void,
//        (void **this, void* param_2, void *param_3, double *param_4)
//) {
//    LOGD("Hooked newStateForProgram called - this: %p, 1: %p, 2: %p, 3: %p", this, param_2, param_3, param_4);
//    orig_newStateForProgram(this, param_2, param_3, param_4);
////    __builtin_trap();
//}

//STATIC_DL_HOOK_SYMBOL(
//        progCompExec,
//        "_ZN5Caver16ProgramComponent7ExecuteEv",
//        void,
//        (void* param_1, void* param_2, void* param_3, double* param_4)
//) {
//    LOGD("Hooked progCompExec called - this: %p, 1: %p, 2: %p, 3: %p", param_1, param_2, param_3, param_4);
//    orig_progCompExec(param_1, param_2, param_3, param_4);
////    __builtin_trap();
//}

//STATIC_DL_HOOK_OFFSET(irdk, 0x1de644, int, (void *param_1, void* param_2, void* param_3, double *param_4)) {
//    LOGD("Hooked... something... called.");
//    return orig_irdk(param_1, param_2, param_3, param_4);
//}

STATIC_DL_HOOK_SYMBOL(
        codedStream,
        "_ZN5Caver5Proto7Program27MergePartialFromCodedStreamEPN6google8protobuf2io16CodedInputStreamE",
        long,
        (void* this, void* stream, int p2, uint p3)
) {
    LOGD("codedStream: this: %p, stream: %p, %d %d", this, stream, p2, p3);

    long ret = orig_codedStream(this, stream, p2, p3);
    LOGD("Return: %p", ret);
    return ret;
}

STATIC_DL_HOOK_SYMBOL(
        loadIntoState,
        "_ZNK5Caver7Program13LoadIntoStateEP9lua_State",
        void,
        (void *this, lua_State *L)
) {
    LOGD("Hooked loadIntoState called - this: %p", this);

    char* stringLua = **(char***)(this + 4);
//    char* bytesLua = **(char***)(this + 12);

//    LOGD("Bytes (%p): %s", bytesLua, bytesLua);
//    LOGD("String (%p): %s", stringLua, stringLua);

    *(void **)(this + 12) = &stringLua;

//    **(void ***)(this + 12) = stringLua;
//    **(void ***)(this + 12) = **(char***)(this + 4);

//    LOGD("After patch Bytes (%p): %s", **(char***)(this + 12), **(char***)(this + 12));

    orig_loadIntoState(this, L);
}

void setup32Patch() {
//    updateAreas();

    LOGD("Applying 32bit patch");
    hook_loadIntoState();

    hook_codedStream();

//    hook_programConstructor();
//    hook_idk();

//    hook_exec();
//    hook_loadPrgm();
//    hook_newStateForProgram();
//    hook_progCompExec();
//    hook_programProtoConstructor1();
//    hook_programProtoNew();

//    hook_loadFromProto();
}

#endif //__arm__