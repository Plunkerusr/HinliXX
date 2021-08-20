#include "DevFS.hpp"
#include "DevFSNode.hpp"

#include <Drivers/Base/CharacterDevice.hpp>
#include <Drivers/DriverManager.hpp>
#include <Filesystem/Base/FS.hpp>
#include <Filesystem/Base/VNode.hpp>
#include <Libkernel/Logger.hpp>
#include <Memory/vmm.hpp>

namespace Kernel::FS {

using namespace Drivers;
using namespace Memory;

DevFS::DevFS(VNodeStorage& vnode_storage)
    : m_vnode_storage(vnode_storage)
    , m_root(DevFSNode(this, devnodes++))
{
}

bool DevFS::init()
{
    auto devices = DriverManager::the().get_by_type(Driver::DriverType::CharacterDevice);
    for (size_t device = 0; device < devices.size(); device++) {
        auto node = new DevFSNode(this, devnodes++, static_cast<CharacterDevice*>(devices[device]));
        m_vnode_storage.push(node);
        m_root.m_childs.push_back(node);
    }
    return true;
}

VNode* DevFS::finddir(VNode& parent, const String& devname)
{
    auto d_parent = ToDevFSNode(parent);
    for (size_t device = 0; device < d_parent.m_childs.size(); device++) {
        if (d_parent.m_childs[device]->m_device->name() == devname) {
            return d_parent.m_childs[device];
        }
    }
    return nullptr;
}

Vector<String> DevFS::listdir(VNode& directory)
{
    auto dev_dir = ToDevFSNode(directory);
    Vector<String> result;
    for (size_t device = 0; device < dev_dir.m_childs.size(); device++) {
        if (dev_dir.m_childs[device] && dev_dir.m_childs[device]->m_device) {
            result.push_back(dev_dir.m_childs[device]->m_device->name());
        }
    }
    return result;
}

uint32_t DevFS::read(VNode& file, uint32_t offset, uint32_t size, void* buffer)
{
    return ToDevFSNode(file).m_device->read(offset, size, buffer);
}

bool DevFS::mmap(VNode& file, uint32_t addr, uint32_t size)
{
    auto dev = ToDevFSNode(file);
    return dev.m_device->mmap(addr, size);
}

bool DevFS::ioctl(VNode& file, uint32_t request)
{
    return ToDevFSNode(file).m_device->ioctl(request);
}

bool DevFS::can_read(VNode& vnode, uint32_t offset)
{
    return ToDevFSNode(vnode).m_device->can_read(offset);
}

}