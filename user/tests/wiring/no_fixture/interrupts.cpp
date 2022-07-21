
#include "application.h"
#include "unit-test/unit-test.h"

// TODO for HAL_PLATFORM_NRF52840
// TestHandler is currently unused but leaving in for refernece
namespace
{

class TestHandler
{
public:
	TestHandler()
	{
		++count;
	}

	TestHandler(const TestHandler&)
	{
		++count;
	}

	~TestHandler()
	{
		--count;
	}

	void operator()()
	{
	}

	static int count;
};

} // namespace

test(INTERRUPTS_01_isisr_willpreempt_servicedirqn)
{
	static volatile bool cont = false;
	attachInterruptDirect(SysTick_IRQn, []() {
		detachInterruptDirect(SysTick_IRQn);
		assertTrue(hal_interrupt_is_isr());
		assertEqual((int)hal_interrupt_serviced_irqn(), (int)SysTick_IRQn);
		cont = true;
	});
	while (!cont);
	assertFalse(hal_interrupt_will_preempt(SysTick_IRQn, SysTick_IRQn));
	assertTrue(hal_interrupt_will_preempt(NonMaskableInt_IRQn, SysTick_IRQn));
	assertFalse(hal_interrupt_will_preempt(SysTick_IRQn, NonMaskableInt_IRQn));

#if HAL_PLATFORM_RTL872X
	// Using arbitrary interrupt
	assertTrue(attachInterruptDirect(I2C0_IRQ, []() {return;}));
	assertFalse(attachInterruptDirect(I2C0_IRQ, []() {return;}));
	assertTrue(detachInterruptDirect(I2C0_IRQ));
#endif
}
