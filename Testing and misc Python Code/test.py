import utime
from machine import Pin

# Pin assignments
TX_PIN = Pin(14, Pin.OUT)
RX_PIN = Pin(17, Pin.IN, Pin.PULL_DOWN)

TIMEOUT_MS = 100  # 100ms timeout

# Test statistics counters
tests_sent = 0
tests_received = 0           
tests_failed = 0

# Function to perform one test
def perform_test():
    global tests_sent, tests_received, tests_failed
    
    # Send a pulse to trigger the Tx EFR32
    TX_PIN.high()
    TX_PIN.low()
    tests_sent += 1
    
    # Wait for the response from the Rx EFR32
    start_time = utime.ticks_ms()  # Get the current time in ms
    
    # Poll RX_PIN until timeout or signal is received
    while utime.ticks_ms() - start_time < TIMEOUT_MS:
        if RX_PIN.value() == 1:  # Signal received from Rx EFR32
            tests_received += 1
            return True  # Success
    
    # Timeout, no signal received
    return False

# Run tests
def run_tests(num_tests: int = 100, inter_test_delay_ms = 25):
    global tests_sent, tests_received, tests_failed
    
    print("Starting tests...")
    
    for i in range(num_tests):
        success = perform_test()
        if success:
            print(f"Test {i + 1}: Passed")
        else:
            print(f"Test {i + 1}: Failed")
        
        # Small delay between tests to avoid overwhelming the hardware
        utime.sleep_ms(inter_test_delay_ms)
    
    make_statistics()
    
def make_statistics():
    tests_failed = tests_sent - tests_received

    print("\nTest Results:")
    print(f"Total tests: {tests_sent}")
    print(f"Total tests passed: {tests_received}")
    print(f"Total tests failed: {tests_failed}")
    error_percentage = (tests_failed / tests_sent) * 100 if tests_sent > 0 else 0
    print(f"Error rate: {error_percentage:.2f}%")

# Execute the test sequence
run_tests(1000)
