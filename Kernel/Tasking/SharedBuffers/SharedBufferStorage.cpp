#include "SharedBufferStorage.hpp"

#include <Memory/pmm.hpp>
#include <Tasking/Process.hpp>
#include <Tasking/Scheduler.hpp>

#include <Macaronlib/ABI/Syscalls.hpp>
#include <Macaronlib/ObjectPool.hpp>

namespace Kernel {

using namespace Tasking;
using namespace Memory;

CreateBufferResult SharedBufferStorage::create_buffer(uint32_t size)
{
    auto buffer = m_free_buffers.allocate<>();

    auto cur_proc = Scheduler::the().cur_process();
    auto size_in_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    uint32_t start_frame = PMM::the().allocate_frames(size_in_pages);
    uint32_t start_page = cur_proc->memory_description().allocate_memory_area<VMArea>(size, VM_READ | VM_WRITE).result()->vm_start() / PAGE_SIZE;

    buffer.object->frame = start_frame;
    buffer.object->pages = size_in_pages;

    VMM::the().psized_map(
        cur_proc->memory_description().memory_descriptor(),
        start_page,
        start_frame,
        size_in_pages,
        0x7);

    return { buffer.id, start_page * PAGE_SIZE };
}

uint32_t SharedBufferStorage::get_buffer(uint32_t id)
{
    auto cur_proc = Scheduler::the().cur_process();

    auto buffer = m_free_buffers.get(id);
    uint32_t start_page = cur_proc->memory_description().allocate_memory_area<VMArea>(buffer->pages * PAGE_SIZE, VM_READ | VM_WRITE).result()->vm_start() / PAGE_SIZE;

    VMM::the().psized_map(
        cur_proc->memory_description().memory_descriptor(),
        start_page,
        buffer->frame,
        buffer->pages,
        0x7);

    return start_page * PAGE_SIZE;
}

}