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

static bool handle_interrupt_window(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
{
    using namespace ::intel_x64::vmcs::idt_vectoring_information;
    std::string msg = "mafia";
    dump(0, &msg);
    return advance(vmcs);
}

static bool handle_taskswitch(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
{
    bfdebug_info(0, "TASK SWTICH");
    using namespace ::intel_x64::vmcs;
    std::string msg = "mafia";
    exit_reason::basic_exit_reason::dump(0, &msg);
    return advance(vmcs);
}

class exit_handler_mafia : public bfvmm::intel_x64::exit_handler
{
public:
    exit_handler_mafia(gsl::not_null<bfvmm::intel_x64::vmcs *> vmcs)
    : exit_handler(vmcs)
    {
        using namespace ::intel_x64::vmcs;
        bfdebug_info(0, "mafia hype you");
        add_handler(
            exit_reason::basic_exit_reason::interrupt_window,
            handler_delegate_t::create<handle_interrupt_window>()
        );
        add_handler(exit_reason::basic_exit_reason::task_switch,
            handler_delegate_t::create<handle_taskswitch>()
        );
    }
};

class mafia_vcpu : public bfvmm::intel_x64::vcpu
{
public:
    mafia_vcpu(vcpuid::type id)
    : vcpu(id)
    {
        m_exit_handler_mafia = std::make_unique<mafia::intel_x64::exit_handler_mafia>(vmcs());
    }
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

