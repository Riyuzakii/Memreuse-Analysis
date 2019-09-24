/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2018 Intel Corporation. All rights reserved.
Copyright (c) 2019, Aditya Rohan <raditya@iitk.ac.in>
Copyright (c) 2019, Aniket Pandey <aniketp@iitk.ac.in>
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
/*
 * This file contains an ISA-portable PIN tool for tracing memory accesses.
 * 
 * Submitted to:
 * CS622A: 2019-20 Fall Semester. Assignment 2
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "pin.H"

#define BLOCK_SIZE   64
#define INIT_STRIDE   8


FILE * trace;
PIN_LOCK pinLock;

// This routine is executed every time a thread is created.
VOID ThreadStart(THREADID tid, CONTEXT *ctxt, INT32 flags, VOID *v)
{
    PIN_GetLock(&pinLock, tid+1);
    fprintf(stdout, "thread begin %d\n",tid);
    fflush(stdout);
    PIN_ReleaseLock(&pinLock);
}

// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID tid, const CONTEXT *ctxt, INT32 code, VOID *v)
{
    PIN_GetLock(&pinLock, tid+1);
    fprintf(stdout, "thread end %d code %d\n",tid, code);
    fflush(stdout);
    PIN_ReleaseLock(&pinLock);
}

// Divide x86 memory accesses to atomic machine accesses.
VOID DivideMemAccess(THREADID tid, VOID * addr, UINT32 size) {
    uint64_t addr_t = reinterpret_cast <uint64_t> (addr);
    uint64_t size_t = static_cast <uint64_t> (size);
    // Try to identify the next block boundary.
    uint64_t next_bdry = addr_t;
    uint64_t reminder = next_bdry % BLOCK_SIZE;
    while (reminder) {
        next_bdry++;
        reminder = next_bdry % BLOCK_SIZE;
    }
    
    // Here, we need to check if the total memory access crosses block boundary.
    if (((addr_t == next_bdry) && (size_t <= BLOCK_SIZE ))
            || (addr_t + size_t <= next_bdry)) {
        for (;;) {
            fprintf(trace, "%d  %12lu\n", tid, addr_t);
            uint64_t stride = INIT_STRIDE;
            while (stride > size_t) {
                stride = stride / 2;
                // Something really bad happened
                if (stride < 1) exit(EXIT_FAILURE);
            }
            assert((stride <= size_t) || (addr_t + stride <= next_bdry));
            addr_t = addr_t + stride;
            size_t = size_t - stride;
            if (size_t == 0) break;
        }
    } else {    // Memory access crosses block boundary
        assert((size_t > BLOCK_SIZE) || (addr_t + size_t > next_bdry));
        uint64_t max_stride = next_bdry - addr_t;
        size_t = size_t - max_stride; assert(size_t);
        for (;;) {
            fprintf(trace, "%d  %12lu\n", tid, addr_t);
            uint64_t stride = INIT_STRIDE;
            while (stride > max_stride) {
                stride = stride / 2;
                // Something really bad happened
                if (stride < 1) exit(EXIT_FAILURE);
            }
            assert((stride <= max_stride) && (addr_t + stride <= next_bdry));
            addr_t = addr_t + stride;
            max_stride = max_stride - stride;
            if (max_stride == 0) break;
        }
        // We have recorded all accesses pre block boundary. Now do same stuff
        // for the remaining strides.
        //
        // IMP: It's possible that access size is still larger than BLOCK_SIZE
        // In that case, we'll have clean 8 byte machine accesses within that
        // block. Record this and keep doing so until size_t < BLOCK_SIZE.
        while (size_t >= BLOCK_SIZE) {
            for (int i = 0; i < (BLOCK_SIZE / INIT_STRIDE); i++) {
                fprintf(trace, "%d  %12lu\n", tid, addr_t);
                addr_t = addr_t + INIT_STRIDE;
            }
            size_t = size_t - BLOCK_SIZE;
        }
        assert(size_t < BLOCK_SIZE);

        // Here, repeat the true case of the parent if-statement.
        for (;;) {
            fprintf(trace, "%d  %12lu\n", tid, addr_t);
            uint64_t stride = INIT_STRIDE;
            while (stride > size_t) {
                stride = stride / 2;
                // Something really bad happened
                if (stride < 1) exit(EXIT_FAILURE);
            }
            assert((stride <= size_t) || (addr_t + stride <= next_bdry));
            addr_t = addr_t + stride;
            size_t = size_t - stride;
            if (size_t == 0) break;
        }
    }
}

// Print a memory read record
VOID RecordMemRead(THREADID thid, VOID * ip, VOID * addr, UINT32 size)
{
    PIN_GetLock(&pinLock, thid+1);
    DivideMemAccess(thid, addr, size);
    // fprintf(trace, "READ  :: Thread : %d | Address : %p | Size : %d\n",
    //         thid, addr, size);
    PIN_ReleaseLock(&pinLock);
}

// Print a memory write record
VOID RecordMemWrite(THREADID thid, VOID * ip, VOID * addr, UINT32 size)
{
    PIN_GetLock(&pinLock, thid+1);
    DivideMemAccess(thid, addr, size);
    // fprintf(trace, "WRITE :: Thread : %d | Address : %p | Size : %d\n",
    //         thid, addr, size);
    PIN_ReleaseLock(&pinLock);
}

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP 
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        // Record the size of x86 memory access to break it down to
        // individual machine accesses.
        UINT32 memSize = INS_MemoryOperandSize(ins, memOp);
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
                IARG_THREAD_ID,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_UINT32, memSize,
                IARG_END);
        }
        // Note that in some architectures a single memory operand can be 
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
                IARG_THREAD_ID,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_UINT32, memSize,
                IARG_END);
        }
    }
}

VOID Fini(INT32 code, VOID *v)
{
    fprintf(trace, "#eof\n");
    fclose(trace);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
   
INT32 Usage()
{
    PIN_ERROR( "This Pintool prints a trace of memory addresses\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    trace = fopen("../traces/addrtrace.out", "w");
    PIN_InitLock(&pinLock);

    // Try to initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddThreadStartFunction(ThreadStart, 0);
    PIN_AddThreadFiniFunction(ThreadFini, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
