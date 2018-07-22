//
// Bareflank Hypervisor Trap INT3
// Copyright (C) 2018 morimolymoly (Tr4pMafia)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include <bfvmm/hve/arch/intel_x64/vcpu/vcpu.h>
#include <bfvmm/vcpu/vcpu.h>
#include <bfdebug.h>
#include <bfvmm/vcpu/vcpu_factory.h>
#include <bfgsl.h>
#include <bfstring.h>
#include <string>

namespace mafia
{
namespace intel_x64
{

static bool
handle_exception_or_non_maskable_interrupt(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
{
    using namespace ::intel_x64::vmcs;
    vm_exit_interruption_information::dump(0);
    bfdebug_info(0, "exception_or_non_maskable_interrupt");

    // reinject
    vm_entry_interruption_information::vector::set(vm_exit_interruption_information::vector::get());
    vm_entry_interruption_information::interruption_type::set(vm_exit_interruption_information::interruption_type::get());
    vm_entry_interruption_information::deliver_error_code_bit::set(vm_exit_interruption_information::error_code_valid::is_enabled());
    vm_entry_interruption_information::reserved::set(vm_exit_interruption_information::reserved::get());
    vm_entry_interruption_information::valid_bit::set(vm_exit_interruption_information::valid_bit::is_enabled());

    vm_entry_instruction_length::set(vm_exit_instruction_length::get());
    return advance(vmcs);
}

class mafia_vcpu : public bfvmm::intel_x64::vcpu
{
public:
    mafia_vcpu(vcpuid::type id)
    : bfvmm::intel_x64::vcpu{id}
    {
        ::intel_x64::vmcs::exception_bitmap::set((1u << 3));
        ::intel_x64::vmcs::exception_bitmap::dump(0);
        exit_handler()->add_handler(
            ::intel_x64::vmcs::exit_reason::basic_exit_reason::exception_or_non_maskable_interrupt,
            handler_delegate_t::create<mafia::intel_x64::handle_exception_or_non_maskable_interrupt>()
        );
    }
    ~mafia_vcpu() = default;
};
}
}

namespace bfvmm
{
std::unique_ptr<bfvmm::vcpu>
vcpu_factory::make_vcpu(vcpuid::type vcpuid, bfobject *obj)
{
    bfignored(obj);
    return std::make_unique<mafia::intel_x64::mafia_vcpu>(vcpuid);
}
}

