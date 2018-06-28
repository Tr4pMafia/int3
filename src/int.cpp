#include <bfvmm/hve/arch/intel_x64/exit_handler/exit_handler.h>
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
handle_cpuid_mafia(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
{
    if (vmcs->save_state()->rax == 0xBF01) {
        bfdebug_info(0, "[MAFIA] host os is" bfcolor_green " now " bfcolor_end "in a vm");
        return false;
    }

    if (vmcs->save_state()->rax == 0xBF00) {
        bfdebug_info(0, "[MAFIA] host os is" bfcolor_red " not " bfcolor_end "in a vm");
        return false;
    }
    return false;
}
static bool
handle_exception_or_non_maskable_interrupt(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
{
    bfdebug_info(0, "exception_or_non_maskable_interrupt");
    return false;
}

class exit_handler_mafia : public bfvmm::intel_x64::exit_handler
{
public:
    exit_handler_mafia(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
    : bfvmm::intel_x64::exit_handler{vmcs}
    {
        using namespace ::intel_x64::vmcs;
        bfdebug_info(0, "mafia hype you");
        add_handler(
            exit_reason::basic_exit_reason::exception_or_non_maskable_interrupt,
            handler_delegate_t::create<mafia::intel_x64::handle_exception_or_non_maskable_interrupt>()
        );
        add_handler(
            exit_reason::basic_exit_reason::cpuid,
            handler_delegate_t::create<mafia::intel_x64::handle_cpuid_mafia>()
        );
    }
    ~exit_handler_mafia() = default;
};

class mafia_vcpu : public bfvmm::intel_x64::vcpu
{
public:
    mafia_vcpu(vcpuid::type id)
    : bfvmm::intel_x64::vcpu{id}
    {
        m_exit_handler_mafia = std::make_unique<mafia::intel_x64::exit_handler_mafia>(vmcs());
    }
    ~mafia_vcpu() = default;
    mafia::intel_x64::exit_handler_mafia *exit_handler()
    { return m_exit_handler_mafia.get(); }
private:
    std::unique_ptr<mafia::intel_x64::exit_handler_mafia> m_exit_handler_mafia;
};
}
}

namespace bfvmm
{
WEAK_SYM std::unique_ptr<bfvmm::vcpu>
vcpu_factory::make_vcpu(vcpuid::type vcpuid, bfobject *obj)
{
    bfignored(obj);
    return std::make_unique<mafia::intel_x64::mafia_vcpu>(vcpuid);
}
}

